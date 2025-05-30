#include "kernel/print.h"
#include "kernel/kstdlib.h"
#include "kernel/utility.h"

static const size_t NUM_COLS = 80;
static const size_t NUM_ROWS = 25;

struct Char {
    uint8_t character;
    uint8_t color;
};

struct Char *buffer = (struct Char *) 0xb8000;
size_t col = 0;
size_t row = 0;
uint8_t color = PRINT_COLOR_WHITE | PRINT_COLOR_BLACK << 4;

static void print_update_cursor(void) {
    uint16_t pos = (uint16_t) (row * NUM_COLS + col);
    out_port_b(0x3D4, 14);
    out_port_b(0x3D5, (uint8_t) ((pos >> 8) & 0xFF));
    out_port_b(0x3D4, 15);
    out_port_b(0x3D5, (uint8_t) (pos & 0xFF));
}

static void set_empty(size_t location) {
    struct Char empty = (struct Char) {
            .character =  ' ',
            .color =  color,
    };

    buffer[location] = empty;
}

static void clear_row(size_t r) {
    for (size_t c = 0; c < NUM_COLS; c++) {
        set_empty(c + NUM_COLS * r);
    }
}

void kcls(void) {
    for (size_t i = 0; i < NUM_ROWS; i++) {
        clear_row(i);
    }
}

static void print_newline(void) {
    col = 0;

    if (row < NUM_ROWS - 1) {
        row++;
        print_update_cursor();
        return;
    }

    for (size_t r = 1; r < NUM_ROWS; r++) {
        for (size_t c = 0; c < NUM_COLS; c++) {
            struct Char character = buffer[c + NUM_COLS * r];
            buffer[c + NUM_COLS * (r - 1)] = character;
        }
    }

    clear_row(NUM_ROWS - 1);
}

void kputchar(char character) {
    switch (character) {
        case '\n':
            print_newline();
            break;
        case '\r':
            col = 0;
            break;
        case '\b':
            if (col > 0) {
                col--;
                set_empty(row * NUM_COLS + col);
            } else if (row > 0) {
                row--;
                col = NUM_COLS - 1;
                set_empty(row * NUM_COLS + col);
            }
            break;
        case '\t':
            if (col == NUM_COLS) {
                print_newline();
            }
            uint16_t tab_len = 4 - (col % 4);

            while (tab_len != 0) {
                set_empty(row * NUM_COLS + (col++));
                tab_len--;
            }
            break;
        default:
            if (col >= NUM_COLS) {
                print_newline();
            }

            buffer[col + NUM_COLS * row] = (struct Char) {
                    .character =  (uint8_t) character,
                    .color =  color
            };

            col++;
    }

    print_update_cursor();
}

void kputs(const char *str) {
    while (*str) {
        kputchar(*(str++));
    }
}

void kprintf(const char *format, ...) {
    va_list args;
    va_start(args, format);

    char buf[512];

    for (size_t i = 0; format[i] != '\0'; i++) {
        if (format[i] == '%') {
            i++;

            typedef enum {
                TYPE_INT,
                TYPE_LONG,
                TYPE_LONG_LONG,
            } LengthModifier;

            size_t width = 0;
            LengthModifier lengthMod = TYPE_INT;

            while (format[i] >= '0' && format[i] <= '9') {
                width = width * 10 + (size_t) (format[i] - '0');
                i++;
            }

            if (format[i] == 'l') {
                i++;
                if (format[i] == 'l') {
                    i++;
                    lengthMod = TYPE_LONG_LONG;
                } else {
                    lengthMod = TYPE_LONG;
                }
            }

            switch (format[i]) {
                case '%': // % character
                    kputchar('%');
                    break;
                case 'd':   // signed int
                case 'i': { // signed int
                    if (lengthMod == TYPE_INT) {
                        int num = va_arg(args, int);
                        itoa(num, buf, 10);
                        // Handle padding for width
                        size_t len = strlen(buf);
                        for (size_t pad = len; pad < width; pad++) {
                            kputchar('0');  // or space if you want space padding
                        }
                        kputs(buf);
                    } else if (lengthMod == TYPE_LONG) {
                        long num = va_arg(args, long);
                        itoa(num, buf, 10);
                        size_t len = strlen(buf);
                        for (size_t pad = len; pad < width; pad++) {
                            kputchar('0');
                        }
                        kputs(buf);
                    } else {
                        long long num = va_arg(args,
                                               long long);
                        itoa(num, buf, 10);
                        size_t len = strlen(buf);
                        for (size_t pad = len; pad < width; pad++) {
                            kputchar('0');
                        }
                        kputs(buf);
                    }
                    break;
                }
                case 'u': { // unsigned int
                    if (lengthMod == TYPE_INT) {
                        unsigned int num = va_arg(args, unsigned int);
                        utoa(num, buf, 10);
                        size_t len = strlen(buf);
                        for (size_t pad = len; pad < width; pad++) {
                            kputchar('0');
                        }
                        kputs(buf);
                    } else if (lengthMod == TYPE_LONG) {
                        unsigned long num = va_arg(args, unsigned long);
                        utoa(num, buf, 10);
                        size_t len = strlen(buf);
                        for (size_t pad = len; pad < width; pad++) {
                            kputchar('0');
                        }
                        kputs(buf);
                    } else {
                        unsigned long long num = va_arg(args, unsigned long long);
                        utoa(num, buf, 10);
                        size_t len = strlen(buf);
                        for (size_t pad = len; pad < width; pad++) {
                            kputchar('0');
                        }
                        kputs(buf);
                    }
                    break;
                }
                case 's': { // string
                    const char *str = va_arg(args, const char*);
                    if (str == NULL) {
                        str = "<null>";
                    }
                    kputs(str);
                    break;
                }
                case 'c': { // character
                    kputchar((char) va_arg(args, int));
                    break;
                }
                case 'p': { // pointer
                    if (width == 0) width = sizeof(void *) * 2;
                    uint64_t num = (uint64_t) (uintptr_t) va_arg(args, void*);
                    kputs("0x");

                    utoa(num, buf, 16);

                    size_t len = 0;
                    while (buf[len] != '\0') len++;

                    for (size_t pad = len; pad < width; pad++) {
                        kputchar('0');
                    }

                    kputs(buf);
                    break;
                }
                case 'x': // hexadecimal
                case 'X': {
                    uint64_t num;
                    if (lengthMod == TYPE_INT) {
                        num = va_arg(args, unsigned int);
                    } else if (lengthMod == TYPE_LONG) {
                        num = va_arg(args, unsigned long);
                    } else {
                        num = va_arg(args, unsigned long long);
                    }

                    if (width == 0) width = 4;

                    utoa(num, buf, 16);

                    if (format[i] == 'X') {
                        for (size_t j = 0; buf[j] != '\0'; j++) {
                            if (buf[j] >= 'a' && buf[j] <= 'f') {
                                buf[j] -= ('a' - 'A');
                            }
                        }
                    }

                    size_t len = strlen(buf);
                    for (size_t pad = len; pad < width; pad++) {
                        kputchar('0');
                    }

                    kputs(buf);
                    break;
                }
                default:
                    kputchar(format[i]);
                    break;
            }
        } else {
            kputchar(format[i]);
        }
    }

    va_end(args);
}


void kset_attr(uint8_t foreground, uint8_t background) {
    color = (uint8_t) (foreground | (background << 4));
}
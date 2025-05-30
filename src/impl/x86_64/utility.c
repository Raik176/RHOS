//
// Created by RHM on 30/04/2025.
//

#include "kernel/utility.h"
#include "kernel/print.h"
#include "kernel/kstdlib.h"

void memset(void *dest, char val, uint64_t len) {
    unsigned char *temp = (unsigned char *) dest;
    for (; len != 0; --len) {
        *temp++ = (unsigned char) val;
    }
}

void out_port_b(uint16_t port, uint8_t val) {
    __asm__ volatile("outb %1, %0" : : "dN"(port), "a"(val));
}

uint8_t in_port_b(uint16_t port) {
    uint8_t rv;
    __asm__ volatile("inb %1, %0" : "=a"(rv) : "dN"(port));
    return rv;
}

void utoa(uint64_t num, char *str, int base) {
    int i = 0;
    uint64_t n = num;

    if (base < 2 || base > 36) {
        if (str) {
            str[0] = '\0';
        }
        return;
    }
    if (str == NULL) {
        return;
    }

    if (n == 0) {
        str[i++] = '0';
        str[i] = '\0';
        return;
    }

    while (n > 0) {
        uint64_t rem = n % (uint64_t) base;
        str[i++] = (char) ((rem > 9) ? (rem - 10) + 'a' : rem + '0');
        n = n / (uint64_t) base;
    }

    str[i] = '\0';

    int start = 0;
    int end = i - 1;
    while (start < end) {
        char temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        start++;
        end--;
    }
}

void itoa(int64_t num, char *str, int base) {
    int i = 0;
    bool is_negative = false;
    uint64_t n;

    if (base < 2 || base > 36) {
        if (str) str[0] = '\0';
        return;
    }
    if (str == NULL) {
        return;
    }

    if (num == 0) {
        str[i++] = '0';
        str[i] = '\0';
        return;
    }

    if (num < 0) {
        is_negative = true;
        n = (uint64_t) -num;
    } else {
        n = (uint64_t) num;
    }

    while (n > 0) {
        uint64_t rem = n % (uint64_t) base;
        str[i++] = (char) ((rem > 9) ? (rem - 10) + 'a' : rem + '0');
        n = n / (uint64_t) base;
    }

    if (is_negative) {
        str[i++] = '-';
    }

    str[i] = '\0';

    int start = 0;
    int end = i - 1;
    while (start < end) {
        char temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        start++;
        end--;
    }
}

size_t strlen(const char *str) {
    size_t length = 0;
    if (str == NULL) return 0;
    while (str[length] != '\0') {
        length++;
    }
    return length;
}
int strncmp(const char *s1, const char *s2, size_t n) {
    for (size_t i = 0; i < n; ++i) {
        unsigned char c1 = (unsigned char)s1[i];
        unsigned char c2 = (unsigned char)s2[i];

        if (c1 != c2) {
            return c1 - c2;
        }

        if (c1 == '\0') {
            return 0;
        }
    }

    return 0;
}

__attribute__((noreturn)) void panic(const char *format) {
    kset_attr(PRINT_COLOR_RED, PRINT_COLOR_BLACK);

    kputs("\nPANIC: ");
    kputs(format);

    __asm__ volatile("cli");
    halt();
}

__attribute__((noreturn)) void halt(void) {
    for (;;)
        __asm__ volatile("hlt;");
}
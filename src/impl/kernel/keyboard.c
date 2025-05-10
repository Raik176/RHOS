//
// Created by RHM on 02/05/2025.
//

#include "keyboard.h"
#include "interrupts.h"
#include "print.h"
#include "utility.h"
#include "kstdlib.h"

const uint32_t lowercase[128] = {
        UNKNOWN, ESC, '1', '2', '3', '4', '5', '6', '7', '8',
        '9', '0', '-', '=', '\b', '\t', 'q', 'w', 'e', 'r',
        't', 'Z', 'u', 'i', 'o', 'p', '[', ']', '\n', CTRL,
        'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',
        '\'', '`', LSHFT, '\\', 'y', 'x', 'c', 'v', 'b', 'n', 'm', ',',
        '.', '/', RSHFT, '*', ALT, ' ', CAPS, F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, NUMLCK, SCRLCK, HOME, UP, PGUP,
        '-', LEFT, UNKNOWN, RIGHT,
        '+', END, DOWN, PGDOWN, INS, DEL, UNKNOWN, UNKNOWN, UNKNOWN, F11, F12, UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN,
        UNKNOWN, UNKNOWN,
        UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN,
        UNKNOWN, UNKNOWN,
        UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN,
        UNKNOWN, UNKNOWN,
        UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN
};

const uint32_t uppercase[128] = {
        UNKNOWN, ESC, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b', '\t', 'Q', 'W', 'E', 'R',
        'T', 'Z', 'U', 'I', 'O', 'P', '{', '}', '\n', CTRL, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~',
        LSHFT, '|', 'Y', 'X', 'C',
        'V', 'B', 'N', 'M', '<', '>', '?', RSHFT, '*', ALT, ' ', CAPS, F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, NUMLCK,
        SCRLCK, HOME, UP, PGUP, '-',
        LEFT, UNKNOWN, RIGHT, '+', END, DOWN, PGDOWN, INS, DEL, UNKNOWN, UNKNOWN, UNKNOWN, F11, F12, UNKNOWN, UNKNOWN,
        UNKNOWN, UNKNOWN,
        UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN,
        UNKNOWN, UNKNOWN,
        UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN,
        UNKNOWN, UNKNOWN,
        UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN
};


bool shiftHeld;
bool capsLocked;

static void keyboard_input(__attribute__((unused)) struct interrupt_registers *regs) {
    unsigned char scanCode = in_port_b(0x60) & 0x7F;
    bool pressed = (in_port_b(0x60) & 0x80) == 0;

    switch (scanCode) {
        case 1:
        case 29:
        case 56:
        case 59:
        case 60:
        case 61:
        case 62:
        case 63:
        case 64:
        case 65:
        case 66:
        case 67:
        case 68:
        case 87:
        case 88: {
            break;
        }
        case 42: { // shift
            shiftHeld = pressed;
            break;
        }
        case 58: { // caps lock
            if (pressed) {
                capsLocked = !capsLocked;
            }
            break;
        }
        default:
            if (pressed) {
                uint32_t key_code;
                if (shiftHeld || capsLocked) {
                    key_code = uppercase[scanCode];
                } else {
                    key_code = lowercase[scanCode];
                }

                if (key_code < 128) {
                    kputchar((char) key_code);
                }
            }
    }
}

void init_keyboard(void) {
    shiftHeld = false;
    capsLocked = false;

    irq_install_handler(1, &keyboard_input);
}
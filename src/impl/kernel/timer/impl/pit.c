//
// Created by RHM on 02/05/2025.
//

#include "kernel/timer/timer.h"
#include "kernel/timer/impl/pit.h"
#include "kernel/utility.h"

static const uint32_t PIT_BASE_HZ = 1193180;

void pit_init(void(*tick)(struct interrupt_registers *regs), uint32_t freq) {
    irq_install_handler(0, tick);

    out_port_b(0x43, 0x36);

    set_timer_frequency(freq);
}

void pit_set_frequency(uint32_t freq) {
    uint32_t divisor = PIT_BASE_HZ / freq;

    out_port_b(0x40, (uint8_t) (divisor & 0xFF));
    out_port_b(0x40, (uint8_t) ((divisor >> 8) & 0xFF));
}
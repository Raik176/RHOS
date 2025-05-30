//
// Created by RHM on 02/05/2025.
//

#include "kernel/timer/timer.h"
#include "kernel/timer/impl/pit.h"
#include "kernel/kstdlib.h"
#include "kernel/print.h"

static void (*timer_init_func)(void(*tick)(struct interrupt_registers *regs), uint32_t freq) = 0;

static void (*timer_set_freq_func)(uint32_t) = 0;

static const uint32_t DEFAULT_FREQ = 100;
static uint32_t timer_frequency;
volatile uint64_t ticks;

static bool is_hpet_available(void) {
    return false;
}

static void tick(__attribute__((unused)) struct interrupt_registers *regs) {
    ticks++;
}

void init_timer(void) {
    ticks = 0;

    if (is_hpet_available()) {

    } else {
        timer_init_func = pit_init;
        timer_set_freq_func = pit_set_frequency;
    }

    timer_init_func(&tick, DEFAULT_FREQ);
}

void set_timer_frequency(uint32_t frequency) {
    timer_frequency = frequency;
    if (timer_set_freq_func != 0) {
        timer_set_freq_func(frequency);
    } else {
        // should probably panic or something
    }
}

uint64_t get_ticks(void) {
    return ticks;
}

void sleep(uint64_t ms) {
    uint64_t start = ticks;
    uint64_t wait_ticks = (ms * timer_frequency) / 1000;

    kprintf("start: %llu, timer freq: %lu, calculated wait ticks: %llu\n", start, timer_frequency, wait_ticks);

    while (ticks - start < wait_ticks);
}
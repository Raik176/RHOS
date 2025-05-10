//
// Created by RHM on 02/05/2025.
//

#ifndef RHOS_TIMER_H
#define RHOS_TIMER_H

#include "interrupts.h"

void init_timer(void);

void set_timer_frequency(uint32_t frequency);

uint64_t get_ticks(void);

void sleep(uint64_t ms);

#endif //RHOS_TIMER_H

//
// Created by RHM on 02/05/2025.
//

#ifndef RHOS_PIT_H
#define RHOS_PIT_H

#include "kstdlib.h"
#include "interrupts.h"

void pit_init(void(*tick)(struct interrupt_registers *regs), uint32_t freq);

void pit_set_frequency(uint32_t freq);

#endif //RHOS_PIT_H

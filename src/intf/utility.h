//
// Created by RHM on 30/04/2025.
//

#ifndef RHOS_UTILITY_H
#define RHOS_UTILITY_H

#include "kstdlib.h"
#include "kstdlib.h"

void memset(void *dest, char val, uint64_t len);

void out_port_b(uint16_t port, uint8_t val);

uint8_t in_port_b(uint16_t port);

void utoa(uint64_t num, char *str, int base);

void itoa(int64_t num, char *str, int base);

size_t strlen(const char *str);

void panic(const char *format);

void halt(void);

#endif //RHOS_UTILITY_H

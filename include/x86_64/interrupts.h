//
// Created by RHM on 30/04/2025.
//

#ifndef RHOS_INTERRUPTS_H
#define RHOS_INTERRUPTS_H

#include "kernel/kstdlib.h"

struct idt_entry {
    uint16_t base_low;
    uint16_t sel;
    uint8_t ist;
    uint8_t flags;
    uint16_t base_mid;
    uint32_t base_high;
    uint32_t zero;
} __attribute__((packed));

struct idt_ptr {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed));

struct interrupt_registers {
    uint64_t rax, rbx, rcx, rdx, rbp, rdi, rsi;
    uint64_t r8, r9, r10, r11, r12, r13, r14, r15;

    uint64_t int_number;
    uint64_t err_code;

    uint64_t rip;
    uint64_t cs;
    uint64_t rflags;
    uint64_t rsp;
    uint64_t ss;
};

typedef void(*irq_handler)(struct interrupt_registers *r);

void irq_install_handler(int irq, irq_handler handler);

void irq_uninstall_handler(int irq);

void init_idt(void);

void set_idt_gate(uint8_t num, uint64_t base, uint16_t sel, uint8_t flags);

void isr_handler(struct interrupt_registers *regs);

extern void isr0(void);

extern void isr1(void);

extern void isr2(void);

extern void isr3(void);

extern void isr4(void);

extern void isr5(void);

extern void isr6(void);

extern void isr7(void);

extern void isr8(void);

extern void isr9(void);

extern void isr10(void);

extern void isr11(void);

extern void isr12(void);

extern void isr13(void);

extern void isr14(void);

extern void isr15(void);

extern void isr16(void);

extern void isr17(void);

extern void isr18(void);

extern void isr19(void);

extern void isr20(void);

extern void isr21(void);

extern void isr22(void);

extern void isr23(void);

extern void isr24(void);

extern void isr25(void);

extern void isr26(void);

extern void isr27(void);

extern void isr28(void);

extern void isr29(void);

extern void isr30(void);

extern void isr31(void);

extern void irq0(void);

extern void irq1(void);

extern void irq2(void);

extern void irq3(void);

extern void irq4(void);

extern void irq5(void);

extern void irq6(void);

extern void irq7(void);

extern void irq8(void);

extern void irq9(void);

extern void irq10(void);

extern void irq11(void);

extern void irq12(void);

extern void irq13(void);

extern void irq14(void);

extern void irq15(void);

#endif //RHOS_INTERRUPTS_H

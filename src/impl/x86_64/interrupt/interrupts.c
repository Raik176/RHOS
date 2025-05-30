//
// Created by RHM on 30/04/2025.
//

#include "x86_64/interrupts.h"
#include "kernel/utility.h"
#include "kernel/print.h"

struct idt_entry idt_entries[256];
struct idt_ptr idt_ptr;

void *irq_routines[16] = {
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0
};

void irq_install_handler(int irq, irq_handler handler) {
    irq_routines[irq] = handler;
}

void irq_uninstall_handler(int irq) {
    irq_routines[irq] = 0;
}

__attribute__((used))
void irq_handler_common(struct interrupt_registers *regs);

__attribute__((used))
void irq_handler_common(struct interrupt_registers *regs) {
    uint64_t irq_number = regs->int_number - 32;
    irq_handler handler = irq_routines[irq_number];

    if (handler) {
        handler(regs);
    }

    if (irq_number >= 8) {
        out_port_b(0xA0, 0x20);
    }
    out_port_b(0x20, 0x20);
}

void init_idt(void) {
    idt_ptr.limit = sizeof(struct idt_entry) * 256 - 1;
    idt_ptr.base = (uint64_t) &idt_entries;

    memset(&idt_entries, 0, sizeof(struct idt_entry) * 256);

    out_port_b(0x20, 0x11);
    out_port_b(0xA0, 0x11);

    out_port_b(0x21, 0x20);
    out_port_b(0xA1, 0x28);

    out_port_b(0x21, 0x04);
    out_port_b(0xA1, 0x02);

    out_port_b(0x21, 0x01);
    out_port_b(0xA1, 0x01);

    out_port_b(0x21, 0x0);
    out_port_b(0xA1, 0x0);

    set_idt_gate(0, (uint64_t) isr0, 0x8, 0x8E);
    set_idt_gate(1, (uint64_t) isr1, 0x8, 0x8E);
    set_idt_gate(2, (uint64_t) isr2, 0x8, 0x8E);
    set_idt_gate(3, (uint64_t) isr3, 0x8, 0x8E);
    set_idt_gate(4, (uint64_t) isr4, 0x8, 0x8E);
    set_idt_gate(5, (uint64_t) isr5, 0x8, 0x8E);
    set_idt_gate(6, (uint64_t) isr6, 0x8, 0x8E);
    set_idt_gate(7, (uint64_t) isr7, 0x8, 0x8E);
    set_idt_gate(8, (uint64_t) isr8, 0x8, 0x8E);
    set_idt_gate(9, (uint64_t) isr9, 0x8, 0x8E);
    set_idt_gate(10, (uint64_t) isr10, 0x8, 0x8E);
    set_idt_gate(11, (uint64_t) isr11, 0x8, 0x8E);
    set_idt_gate(12, (uint64_t) isr12, 0x8, 0x8E);
    set_idt_gate(13, (uint64_t) isr13, 0x8, 0x8E);
    set_idt_gate(14, (uint64_t) isr14, 0x8, 0x8E);
    set_idt_gate(15, (uint64_t) isr15, 0x8, 0x8E);
    set_idt_gate(16, (uint64_t) isr16, 0x8, 0x8E);
    set_idt_gate(17, (uint64_t) isr17, 0x8, 0x8E);
    set_idt_gate(18, (uint64_t) isr18, 0x8, 0x8E);
    set_idt_gate(19, (uint64_t) isr19, 0x8, 0x8E);
    set_idt_gate(20, (uint64_t) isr20, 0x8, 0x8E);
    set_idt_gate(21, (uint64_t) isr21, 0x8, 0x8E);
    set_idt_gate(22, (uint64_t) isr22, 0x8, 0x8E);
    set_idt_gate(23, (uint64_t) isr23, 0x8, 0x8E);
    set_idt_gate(24, (uint64_t) isr24, 0x8, 0x8E);
    set_idt_gate(25, (uint64_t) isr25, 0x8, 0x8E);
    set_idt_gate(26, (uint64_t) isr26, 0x8, 0x8E);
    set_idt_gate(27, (uint64_t) isr27, 0x8, 0x8E);
    set_idt_gate(28, (uint64_t) isr28, 0x8, 0x8E);
    set_idt_gate(29, (uint64_t) isr29, 0x8, 0x8E);
    set_idt_gate(30, (uint64_t) isr30, 0x8, 0x8E);
    set_idt_gate(31, (uint64_t) isr31, 0x8, 0x8E);

    set_idt_gate(32, (uint64_t) irq0, 0x8, 0x8E);
    set_idt_gate(33, (uint64_t) irq1, 0x8, 0x8E);
    set_idt_gate(34, (uint64_t) irq2, 0x8, 0x8E);
    set_idt_gate(35, (uint64_t) irq3, 0x8, 0x8E);
    set_idt_gate(36, (uint64_t) irq4, 0x8, 0x8E);
    set_idt_gate(37, (uint64_t) irq5, 0x8, 0x8E);
    set_idt_gate(38, (uint64_t) irq6, 0x8, 0x8E);
    set_idt_gate(39, (uint64_t) irq7, 0x8, 0x8E);
    set_idt_gate(40, (uint64_t) irq8, 0x8, 0x8E);
    set_idt_gate(41, (uint64_t) irq9, 0x8, 0x8E);
    set_idt_gate(42, (uint64_t) irq10, 0x8, 0x8E);
    set_idt_gate(43, (uint64_t) irq11, 0x8, 0x8E);
    set_idt_gate(44, (uint64_t) irq12, 0x8, 0x8E);
    set_idt_gate(45, (uint64_t) irq13, 0x8, 0x8E);
    set_idt_gate(46, (uint64_t) irq14, 0x8, 0x8E);
    set_idt_gate(47, (uint64_t) irq15, 0x8, 0x8E);

    __asm__ volatile("lidt %0"::"m"(idt_ptr));
    __asm__ volatile("sti");
}

void set_idt_gate(uint8_t num, uint64_t base, uint16_t sel, uint8_t flags) {
    idt_entries[num].base_low = base & 0xFFFF;
    idt_entries[num].base_mid = (base >> 16) & 0xFFFF;
    idt_entries[num].base_high = (uint32_t) (base >> 32);

    idt_entries[num].sel = sel;
    idt_entries[num].ist = 0;
    idt_entries[num].flags = flags;
    idt_entries[num].zero = 0;
}

const char *exception_messages[] = {
        "Division by Zero",
        "Debug",
        "Non-Maskable Interrupt",
        "Breakpoint",
        "Into Detected Overflow",
        "Out of Bounds",
        "Invalid Opcode",
        "No Coprocessor",
        "Double Fault",
        "Coprocessor Segment Overrun",
        "Bad TSS",
        "Segment Not Present",
        "Stack Fault",
        "General Protection Fault",
        "Page Fault",
        "Unknown Interrupt",
        "Coprocessor Fault",
        "Alignment Fault",
        "Machine Check"
};

__attribute__((used))
void isr_handler(struct interrupt_registers *regs) {
    if (regs->int_number < 32) {
        kset_attr(PRINT_COLOR_RED, PRINT_COLOR_BLACK);

        const char *err;
        if (regs->int_number > 18) {
            err = "Reserved";
        } else {
            err = exception_messages[regs->int_number];
        }
        kprintf("Exception Message: %s (Interrupt Number: %llx)\n", err, regs->err_code);

        kprintf("\nGeneral Purpose Registers:\n");
        kprintf("  R8 : 0x%016llx  R9 : 0x%016llx\n", regs->r8, regs->r9);
        kprintf("  R10: 0x%016llx  R11: 0x%016llx\n", regs->r10, regs->r11);
        kprintf("  R12: 0x%016llx  R13: 0x%016llx\n", regs->r12, regs->r13);
        kprintf("  R14: 0x%016llx  R15: 0x%016llx\n", regs->r14, regs->r15);

        kprintf("\nOther Registers:\n");
        kprintf("  RSI: 0x%016llx  RDI: 0x%016llx\n", regs->rsi, regs->rdi);
        kprintf("  RBP: 0x%016llx  RDX: 0x%016llx\n", regs->rbp, regs->rdx);
        kprintf("  RCX: 0x%016llx  RBX: 0x%016llx\n", regs->rcx, regs->rbx);
        kprintf("  RAX: 0x%016llx\n", regs->rax);

        kprintf("\nCPU State at Exception:\n");
        kprintf("  RIP: 0x%016llx  CS:  0x%016llx\n", regs->rip, regs->cs);
        kprintf("  RFLAGS: 0x%016llx\n", regs->rflags);
        kprintf("  RSP: 0x%016llx  SS:  0x%016llx\n", regs->rsp, regs->ss);

        // no need to disable interrupts as they're already disabled before control is passed to the handler
        halt();
    }
}
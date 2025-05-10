extern isr_handler
extern irq_handler_common

%macro ISR_NOERR 1
    global isr%1
    isr%1:
        cli
        push qword 0
        push qword %1
        jmp isr_common_stub
%endmacro

%macro ISR_ERR 1
    global isr%1
    isr%1:
        cli
        push qword %1
        jmp isr_common_stub
%endmacro

%macro IRQ 2
    global irq%1
    irq%1:
        cli
        push qword 0
        push qword %2
        jmp irq_common_stub
%endmacro

ISR_NOERR 0  ; Divide By Zero
ISR_NOERR 1  ; Debug
ISR_NOERR 2  ; Non-Maskable Interrupt
ISR_NOERR 3  ; Breakpoint
ISR_NOERR 4  ; Overflow
ISR_NOERR 5  ; Bound Range Exceeded
ISR_NOERR 6  ; Invalid Opcode
ISR_NOERR 7  ; Device Not Available
ISR_ERR   8  ; Double Fault
ISR_NOERR 9  ; Coprocessor Segment Overrun
ISR_ERR   10 ; Invalid TSS
ISR_ERR   11 ; Segment Not Present
ISR_ERR   12 ; Stack-Segment Fault
ISR_ERR   13 ; General Protection Fault
ISR_ERR   14 ; Page Fault
ISR_NOERR 15 ; Reserved
ISR_NOERR 16 ; x87 Floating-Point Exception
ISR_ERR   17 ; Alignment Check
ISR_NOERR 18 ; Machine Check
ISR_NOERR 19 ; SIMD Floating-Point Exception
ISR_NOERR 20 ; Virtualization Exception
ISR_NOERR 21 ; Control Protection Exception
ISR_NOERR 22 ; Reserved
ISR_NOERR 23 ; Reserved
ISR_NOERR 24 ; Reserved
ISR_NOERR 25 ; Reserved
ISR_NOERR 26 ; Reserved
ISR_NOERR 27 ; Reserved
ISR_NOERR 28 ; Reserved
ISR_NOERR 29 ; Reserved
ISR_ERR 30 ; Security Exception
ISR_NOERR 31 ; Reserved

IRQ 0,  32
IRQ 1,  33
IRQ 2,  34
IRQ 3,  35
IRQ 4,  36
IRQ 5,  37
IRQ 6,  38
IRQ 7,  39
IRQ 8,  40
IRQ 9,  41
IRQ 10, 42
IRQ 11, 43
IRQ 12, 44
IRQ 13, 45
IRQ 14, 46
IRQ 15, 47

isr_common_stub:
    push rax
    push rbx
    push rcx
    push rdx
    push rbp
    push rdi
    push rsi
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15

    mov rdi, rsp
    call isr_handler

    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rsi
    pop rdi
    pop rbp
    pop rdx
    pop rcx
    pop rbx
    pop rax

    add rsp, 16

    sti
    iretq

irq_common_stub:
    push rax
    push rbx
    push rcx
    push rdx
    push rbp
    push rdi
    push rsi
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15

    mov rdi, rsp
    call irq_handler_common

    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rsi
    pop rdi
    pop rbp
    pop rdx
    pop rcx
    pop rbx
    pop rax

    add rsp, 16

    sti
    iretq
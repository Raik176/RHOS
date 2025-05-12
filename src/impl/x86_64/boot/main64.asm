global long_mode_start
global stack_bottom
global stack_top

extern multiboot_info_ptr
extern kmain
extern panic

section .text
bits 64
long_mode_start:
    ; load null into all data segment registers
    mov ax, 0
    mov ss, ax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    mov rdi, [rel multiboot_info_ptr]
	call kmain

    .loop:
    hlt
    jmp .loop
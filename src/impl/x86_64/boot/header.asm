MBOOT_MAGIC_NUMBER EQU 0xe85250d6                ; multiboot2
MBOOT_ARCHITECTURE EQU 0                         ; protected mode i386
MBOOT_CHECKSUM_VAL EQU 0x100000000
MBOOT_HEADER_SIZE  EQU header_end - header_start ; size of entire multiboot2 header (excluding tags)

section .mb2_header align=8
header_start:
	dd MBOOT_MAGIC_NUMBER
	dd MBOOT_ARCHITECTURE
	dd MBOOT_HEADER_SIZE
	dd MBOOT_CHECKSUM_VAL - (MBOOT_MAGIC_NUMBER + MBOOT_ARCHITECTURE + MBOOT_HEADER_SIZE)

    ; ------------------------------
    ; Request Memory Map Tag (Type 6)
    ; ------------------------------
    dw 6
    dw 0
    dd 8

    ; ------------------------------
    ; Request Bootloader Name Tag (Type 1)
    ; ------------------------------
    dw 1
    dw 0
    dd 8

    ; ------------------------------
    ; End Tag (Type 0)
    ; ------------------------------
    dw 0
    dw 0
    dd 8
header_end:
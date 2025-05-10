ENTRY(start)

VIRT_BASE = 0xFFFFFFFF80000000;
PHYS_BASE = 0x00100000;

SECTIONS {
    . = VIRT_BASE;

	.boot : AT(PHYS_BASE + (. - VIRT_BASE)) {
        KEEP(*(.mb2_header))
	} :text

    _kernel_start = .;
    _text_start = .;
    .text : AT(PHYS_BASE + (. - VIRT_BASE)) {
      *(.text .text.*)
    } :text
    _text_end = .;

    _rodata_start = .;
    .rodata : AT(PHYS_BASE + (. - VIRT_BASE)) {
      *(.rodata .rodata.*)
    } :rodata
    _rodata_end = .;

    _data_start = .;
    .data : AT(PHYS_BASE + (. - VIRT_BASE)) {
      *(.data .data.*)
    } :data
    _data_end = .;

    _bss_start = .;
    .bss : AT(PHYS_BASE + (. - VIRT_BASE)) {
      *(COMMON)
      *(.bss .bss.*)
    } :data
    _bss_end = .;
    _kernel_end = .;

    /DISCARD/ : {
        *(.comment)
        *(.eh_frame)
        *(.note.gnu.build-id)
        *(.note.ABI-tag)
    }
}

PHDRS {
    text   PT_LOAD FLAGS(4 | 1); /* R | X (read & execute) */
    rodata PT_LOAD FLAGS(4);     /* R     (read          ) */
    data   PT_LOAD FLAGS(6);     /* R | W (read & write  ) */
}

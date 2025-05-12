ENTRY(start)

SECTIONS {
    . = 0x00100000;

	.boot : {
        KEEP(*(.mb2_header))
	} :text

    _kernel_start = .;
    _text_start = .;
    .text : {
      *(.text .text.*)
    } :text
    _text_end = .;

    _rodata_start = .;
    .rodata : {
      *(.rodata .rodata.*)
    } :rodata
    _rodata_end = .;

    _data_start = .;
    .data : {
      *(.data .data.*)
    } :data
    _data_end = .;

    _bss_start = .;
    .bss : {
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

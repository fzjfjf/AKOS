ENTRY(protected)      /* Ze label in your asm vhere execution starts */
SECTIONS {
    . = 0x80000;      /* Ensure everything starts at ze right place */
    .text : { *(.text) }
    .data : { *(.data) }
    .bss  : { *(.bss)  }
    .rodata : { *(.rodata) }
}

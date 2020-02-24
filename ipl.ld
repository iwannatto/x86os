OUTPUT_FORMAT("binary")

MEMORY {
    ROM (rx) : ORIGIN = 0x7c00, LENGTH = 512
}

SECTIONS {
    /* *(.text) means .text section of input file with arbitrary name */
    .text : { *(.text) } >ROM
}
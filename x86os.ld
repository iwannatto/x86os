OUTPUT_FORMAT("binary")
OUTPUT_ARCH("i386")

MEMORY {
    /*
     * this region is loaded by ipl.
     * ORIGIN is entry point.
     * LENGTH can be calculated by
     * (ipl load offset) + (ipl load size) - ORIGIN
     */
    KERNEL (WX) : ORIGIN = 0xc200, LENGTH = (0x8000 + 0x2d000 - 0xc200)
    /* LENGTH is not confirmed */
    MEMMAN (W) : ORIGIN = 0x003c0000, LENGTH = (0x00f00000 - 0x003c0000)
}


SECTIONS {
    .text : {
        entry.o(.text)
        *(.text)
    } >KERNEL
    .rodata : { *(.rodata) } >KERNEL
    .eh_frame : { *(.eh_frame) } >KERNEL
    .data : { *(.data) } >KERNEL
    .bss : { *(.bss) } >KERNEL
    /DISCARD/ : { *(.comment) }
    /*
     * The rest of the section seems to be placed here
     * without being explicitly stated
     */

    .memman : { mman = .; } >MEMMAN
}

OUTPUT_FORMAT("binary")
OUTPUT_ARCH("i386")

MEMORY {
    /*
     * this region is loaded by ipl.
     * ORIGIN is entry point.
     * LENGTH can be calculated by
     * (ipl load offset) + (ipl load size) - ORIGIN
     */
    KERNEL (rx) : ORIGIN = 0xc200, LENGTH = (0x8000 + 0x2d000 - 0xc200)
}


SECTIONS {
    .text : { entry.o(.text) } >KERNEL
}
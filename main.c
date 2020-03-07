void main(void) {
    volatile char *vram = (volatile char *)0xa0000;
    for (int i = 0; i < 0x1000; ++i) {
        vram[i] = 15;
    }

    while (1) {
        asm volatile("hlt");
    }
}
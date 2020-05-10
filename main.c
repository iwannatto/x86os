#include "asm.h"
#include "graphic.h"
#include "interrupt.h"
#include "kbc.h"
#include "keyboard.h"
#include "mem.h"
#include "mouse.h"
#include "pic.h"
#include "util.h"

extern int memman[50];

void main(void)
{
    init_idt();
    init_kbc();
    init_pic();
    asm volatile("sti");

    init_palette();
    init_background();
    init_keyboard();
    init_mouse();

    unsigned int memtotal =
        0x00400000 + memtest((void *)0x00400000, (void *)0xbfffffff);
    init_memman();
    freeb((void *)0x00001000, 0x0009e000);
    freeb((void *)0x00400000, memtotal - 0x00400000);
    printf("mem: %dMB free: %dKB\n", (int)memtotal / 1024 / 1024,
           (int)size_of_free() / 1024);

    while (1) {
        draw_keyinput();
        draw_mouseinput();
        asm volatile("cli");
        if (keyinput_empty() && mouseinput_empty()) {
            asm volatile("sti; hlt");
            continue;
        }
        asm volatile("sti");
    }
}

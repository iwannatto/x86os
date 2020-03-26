#include "asm.h"
#include "graphic.h"
#include "interrupt.h"
#include "kbc.h"
#include "keyboard.h"
#include "mouse.h"
#include "pic.h"
#include "util.h"

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
    printf("hello, world\n", 0);

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

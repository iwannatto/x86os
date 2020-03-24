#include "asm.h"
#include "graphic.h"
#include "interrupt.h"
#include "pic.h"
#include "util.h"

void main(void)
{
    init_palette();
    init_idt();
    init_KBC();
    init_pic();
    asm volatile("sti");

    init_background();
    printf("hello, world\n", 0);
    draw_mouse_cursor();

    while (1) {
        asm volatile("cli");
        int kr = keybuf_read();
        int mr = mousebuf_read();
        if (kr == KEYBUF_READ_EMPTY && mr == MOUSEBUF_READ_EMPTY) {
            asm volatile("sti; hlt");
            continue;
        }
        asm volatile("sti");
        if (kr != KEYBUF_READ_EMPTY) {
            printf("k:%d ", kr);
        }
        if (mr != MOUSEBUF_READ_EMPTY) {
            printf("m:%d ", mr);
        }
    }
}

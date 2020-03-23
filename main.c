#include "asm.h"
#include "graphic.h"
#include "interrupt.h"
#include "pic.h"
#include "util.h"

void main(void)
{
    init_palette();
    init_idt();
    init_pic();
    asm volatile("sti");

    init_background();
    printf("hello, world\n", 0);
    draw_mouse_cursor();

    while (1) {
        asm volatile("cli");
        int r = keybuf_read();
        if (r == KEYBUF_READ_EMPTY) {
            asm volatile("sti; hlt");
            continue;
        }
        asm volatile("sti");
        printf("%d\n", r);
    }
}

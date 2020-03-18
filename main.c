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
    printf("hello, %d\nhello, %d", 12345, 98765);
    draw_mouse_cursor();

    while (1) {
        asm volatile("hlt");
    }
}

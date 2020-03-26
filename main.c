#include "asm.h"
#include "graphic.h"
#include "interrupt.h"
#include "pic.h"
#include "util.h"

struct mouse_decode_info {
    unsigned char buf[3], phase;
    int x, y, btn;
};

static struct mouse_decode_info mdec;

int decode_mouse(struct mouse_decode_info *mdec, unsigned char x)
{
    switch (mdec->phase) {
    case 0:
        if (x == 0xfa) {
            mdec->phase = 1;
        }
        break;
    case 1:
        if ((x & 0xc8) != 0x08) {
            break;
        }
        mdec->buf[0] = x;
        mdec->phase = 2;
        break;
    case 2:
        mdec->buf[1] = x;
        mdec->phase = 3;
        break;
    case 3:
        mdec->buf[2] = x;
        mdec->phase = 1;
        mdec->btn = mdec->buf[0] & 0x07;
        mdec->x = mdec->buf[1];
        mdec->y = mdec->buf[2];
        if ((mdec->buf[0] & 0x10) != 0) {
            mdec->x |= 0xffffff00;
        }
        if ((mdec->buf[0] & 0x20) != 0) {
            mdec->y |= 0xffffff00;
        }
        mdec->y = -mdec->y;
        return 1;
    default:
        break;
    }
    return 0;
}

void main(void)
{
    init_palette();
    init_idt();
    init_KBC();
    init_pic();
    mdec.phase = 0;
    asm volatile("sti");

    init_background();
    printf("hello, world\n", 0);
    int mx = 100;
    int my = 100;
    draw_mouse_cursor(mx, my);

    while (1) {
        asm volatile("cli");
        int kstatus = 0;
        int kr = keybuf_read(&kstatus);
        int mstatus = 0;
        unsigned char mr = mousebuf_read(&mstatus);
        if (kstatus == KEYBUF_READ_EMPTY && mstatus == MOUSEBUF_READ_EMPTY) {
            asm volatile("sti; hlt");
            continue;
        }
        asm volatile("sti");
        if (kstatus != KEYBUF_READ_EMPTY) {
            printf("k:%d ", kr);
        }
        if (mstatus != MOUSEBUF_READ_EMPTY) {
            if (decode_mouse(&mdec, mr)) {
                drawsq(0, 0, 160, 16, DARK_CYAN);
                fixed_printf("%d %d %d %d", mx, my, mdec.x, mdec.y);
                drawsq(mx, my, 16, 16, DARK_CYAN);
                mx += mdec.x;
                // if (-10 <= mdec.y && mdec.y <= 10) {
                my += mdec.y;
                // }
                mx = mx > 0 ? mx : 0;
                mx = mx < 320 - 16 ? mx : 320 - 16;
                my = my > 0 ? my : 0;
                my = my < 200 - 16 ? my : 200 - 16;
                draw_mouse_cursor(mx, my);
            }
        }
    }
}

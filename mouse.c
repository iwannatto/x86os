#include "mouse.h"
#include "graphic.h"
#include "kbc.h"
#include "pic.h"
#include "ringbuf.h"
#include "util.h"

static int mx = 100;
static int my = 100;

static struct ringbuf mousebuf;

struct mouse_decode_info {
    unsigned char buf[3], phase;
    int x, y, btn;
};

static struct mouse_decode_info mdec;

void init_mouse(void)
{
    init_ringbuf(&mousebuf);
    mdec.phase = 0;
    draw_mouse_cursor(mx, my);
}

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

void draw_mouseinput(void)
{
    while (1) {
        asm volatile("cli");
        int status;
        char x = ringbuf_read(&mousebuf, &status);
        asm volatile("sti");
        if (status == RINGBUF_READ_EMPTY) {
            return;
        }

        if (decode_mouse(&mdec, x)) {
            drawsq(0, 0, 160, 16, DARK_CYAN);
            fixed_printf("%d %d %d %d", mx, my, mdec.x, mdec.y);
            drawsq(mx, my, mx + 16, my + 16, DARK_CYAN);
            mx += mdec.x;
            my += mdec.y;
            mx = mx > 0 ? mx : 0;
            mx = mx < 320 - 16 ? mx : 320 - 16;
            my = my > 0 ? my : 0;
            my = my < 200 - 16 ? my : 200 - 16;
            draw_mouse_cursor(mx, my);
        }
    }
}

int mouseinput_empty(void) { return ringbuf_empty(mousebuf); }

/* called from asm_inthandler2c in asm.h */
void inthandler2c(int *esp)
{
    char r = read_mouseint();
    ringbuf_write(&mousebuf, r);
    reenable_mouseint();
}

#include "keyboard.h"
#include "kbc.h"
#include "pic.h"
#include "ringbuf.h"
#include "util.h"

static struct ringbuf keybuf;

void init_keyboard(void) { init_ringbuf(&keybuf); }

void draw_keyinput(void)
{
    while (1) {
        asm volatile("cli");
        int status;
        char x = ringbuf_read(&keybuf, &status);
        asm volatile("sti");

        if (status == RINGBUF_READ_EMPTY) {
            return;
        }

        printf("k:%d ", x);
    }
}

int keyinput_empty(void) { return ringbuf_empty(keybuf); }

/* called from asm_inthandler21 in asm.h */
void inthandler21(int *esp)
{
    char r = read_keyint();
    ringbuf_write(&keybuf, r);
    reenable_keyint();
}

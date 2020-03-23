#include "interrupt.h"

#include "asm.h"
#include "pic.h"
#include "util.h"

#define AR_INTGATE32 0x008e

struct gate_descriptor {
    short offset_low, selector;
    char hoge, access_right;
    short offset_high;
};

/* in entry.s */
extern struct gate_descriptor idt[256];

#define KEYBUF_SIZE 256

struct keyboard_buffer {
    unsigned char buf[KEYBUF_SIZE];
    int head, tail;
};

static struct keyboard_buffer keybuf;

#define KEYBUF_WRITE_SUCCESS 0
#define KEYBUF_WRITE_FULL -1

int keybuf_write(int x)
{
    int h = (keybuf.head + 1) % KEYBUF_SIZE;
    if (h == keybuf.tail) {
        return KEYBUF_WRITE_FULL;
    }

    keybuf.buf[keybuf.head] = x;
    keybuf.head = h;
    return KEYBUF_WRITE_SUCCESS;
}

int keybuf_read(void)
{
    printf("%d, %d\n", keybuf.head, keybuf.tail);
    if (keybuf.tail == keybuf.head) {
        return KEYBUF_READ_EMPTY;
    }

    int r = keybuf.buf[keybuf.tail];
    keybuf.tail = (keybuf.tail + 1) % KEYBUF_SIZE;
    return r;
}

void inthandler21(int *esp) { keybuf_write((int)key_read()); }

void set_gate_descriptor(struct gate_descriptor *gd, int offset, int selector,
                         int access_right)
{
    gd->offset_low = offset & 0xffff;
    gd->selector = selector;
    gd->hoge = (access_right >> 8) & 0xff;
    gd->access_right = access_right & 0xff;
    gd->offset_high = (offset >> 16) & 0xffff;
    return;
}

void init_keybuf(void)
{
    for (int i = 0; i < KEYBUF_SIZE; ++i) {
        keybuf.buf[i] = 0;
        keybuf.head = 0;
        keybuf.tail = 0;
    }
}

void init_idt(void)
{
    init_keybuf();
    set_gate_descriptor(idt + 0x21, (int)asm_inthandler21, 2 << 3,
                        AR_INTGATE32);
}

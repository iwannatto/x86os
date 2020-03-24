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

#define RINGBUF_SIZE 256

struct ringbuf {
    unsigned char buf[RINGBUF_SIZE];
    int head, tail;
};

static struct ringbuf keybuf;
static struct ringbuf mousebuf;

#define RINGBUF_WRITE_SUCCESS 0
#define RINGBUF_WRITE_FULL -1

int ringbuf_write(struct ringbuf *rbuf, int x)
{
    int h = (rbuf->head + 1) % RINGBUF_SIZE;
    if (h == rbuf->tail) {
        return RINGBUF_WRITE_FULL;
    }

    rbuf->buf[rbuf->head] = x;
    rbuf->head = h;
    return RINGBUF_WRITE_SUCCESS;
}

int ringbuf_read(struct ringbuf *rbuf)
{
    if (rbuf->tail == rbuf->head) {
        return KEYBUF_READ_EMPTY;
    }

    int r = rbuf->buf[rbuf->tail];
    rbuf->tail = (rbuf->tail + 1) % RINGBUF_SIZE;
    return r;
}

int keybuf_read(void) { return ringbuf_read(&keybuf); }
int mousebuf_read(void) { return ringbuf_read(&mousebuf); }

void inthandler21(int *esp) { ringbuf_write(&keybuf, (int)key_read()); }

void inthandler2c(int *esp) { ringbuf_write(&mousebuf, (int)mouse_read()); }

void init_ringbuf(struct ringbuf *rbuf)
{
    for (int i = 0; i < RINGBUF_SIZE; ++i) {
        rbuf->buf[i] = 0;
        rbuf->head = 0;
        rbuf->tail = 0;
    }
}

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

void init_idt(void)
{
    init_ringbuf(&keybuf);
    init_ringbuf(&mousebuf);
    set_gate_descriptor(idt + 0x21, (int)asm_inthandler21, 2 << 3,
                        AR_INTGATE32);
    set_gate_descriptor(idt + 0x2c, (int)asm_inthandler2c, 2 << 3,
                        AR_INTGATE32);
}

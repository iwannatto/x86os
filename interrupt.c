#include "interrupt.h"
#include "asm.h"

#define AR_INTGATE32 0x008e

struct gate_descriptor {
    short offset_low, selector;
    char hoge, access_right;
    short offset_high;
};

/* in entry.s */
extern struct gate_descriptor idt[256];

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
    set_gate_descriptor(idt + 0x21, (int)asm_inthandler21, 2 << 3,
                        AR_INTGATE32);
    set_gate_descriptor(idt + 0x2c, (int)asm_inthandler2c, 2 << 3,
                        AR_INTGATE32);
}

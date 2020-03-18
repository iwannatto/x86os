#include "interrupt.h"

#include "asm.h"
#include "util.h"

#define AR_INTGATE32 0x008e

struct gate_descriptor {
    short offset_low, selector;
    char hoge, access_right;
    short offset_high;
};

extern struct gate_descriptor idt[256];

void inthandler21(int *esp)
{
    printf("key\n", 0);
    while (1) {
        asm volatile("hlt");
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
    set_gate_descriptor(idt + 0x21, (int)asm_inthandler21, 2 << 3,
                        AR_INTGATE32);
}

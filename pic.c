#include "pic.h"

#include "asm.h"

#define PIC0_ICW1 0x0020
#define PIC0_OCW2 0x0020
#define PIC0_IMR 0x0021
#define PIC0_ICW2 0x0021
#define PIC0_ICW3 0x0021
#define PIC0_ICW4 0x0021

#define PIC1_ICW1 0x00a0
#define PIC1_OCW2 0x00a0
#define PIC1_IMR 0x00a1
#define PIC1_ICW2 0x00a1
#define PIC1_ICW3 0x00a1
#define PIC1_ICW4 0x00a1

/* before calling this, all interrupts to PIC must be masked */
void init_pic(void)
{
    out8(PIC0_ICW1, 0x11);   /* initialize */
    out8(PIC0_ICW2, 0x20);   /* receive IRQ0~7 as INT0x20~0x27 */
    out8(PIC0_ICW3, 1 << 2); /* IRQ2 is connected to slave PIC */
    out8(PIC0_ICW4, 0x01);   /* x86 */

    out8(PIC1_ICW1, 0x11); /* initialize */
    out8(PIC1_ICW2, 0x28); /* receive IRQ8~15 as INT0x28~2f */
    out8(PIC1_ICW3, 2);    /* IRQ number master PIC uses to connect to is 2 */
    out8(PIC1_ICW4, 0x01); /* x86 */

    out8(PIC0_IMR, 0b11111001); /* allow IRQ-2=PIC1 and IRQ-1=keyboard */
    out8(PIC1_IMR, 0b11101111); /* allow IRQ-12=mouse */
}

void reenable_keyint(void) { out8(PIC0_OCW2, 0x61); /* re-enable IRQ-1 */ }

void reenable_mouseint(void)
{
    out8(PIC1_OCW2, 0x64); /* re-enable IRQ-12 */
    out8(PIC0_OCW2, 0x62); /* re-enable IRQ-2 */
}

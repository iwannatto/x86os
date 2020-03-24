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

#define KBC_DATA 0x0060
#define KBC_STATUS 0x0064
#define KBC_COMMAND 0x0064
#define READY_TO_SEND(status) ((status & (1 << 1)) == 0)
#define CONFIGURE_MODE 0x60
#define ACCEPT_MOUSE_MODE 0x47
#define CONFIGURE_MOUSE 0xd4
#define ENABLE_MOUSE 0xf4

void wait_until_KBC_ready(void)
{
    while (!READY_TO_SEND(in8(KBC_STATUS))) {
        ;
    }
}

void init_KBC(void)
{
    wait_until_KBC_ready();
    out8(KBC_COMMAND, CONFIGURE_MODE);
    wait_until_KBC_ready();
    out8(KBC_DATA, ACCEPT_MOUSE_MODE);
    wait_until_KBC_ready();
    out8(KBC_COMMAND, CONFIGURE_MOUSE);
    wait_until_KBC_ready();
    out8(KBC_DATA, ENABLE_MOUSE);
}

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

char key_read(void)
{
    out8(PIC0_OCW2, 0x61); /* re-enable IRQ-1 */
    return in8(KBC_DATA);
}

char mouse_read(void)
{
    out8(PIC1_OCW2, 0x64); /* re-enable IRQ-12 */
    out8(PIC0_OCW2, 0x62); /* re-enable IRQ-2 */
    return in8(KBC_DATA);
}

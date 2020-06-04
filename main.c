#include "asm.h"
#include "graphic.h"
#include "interrupt.h"
#include "kbc.h"
#include "keyboard.h"
#include "layer.h"
#include "mem.h"
#include "mouse.h"
#include "pic.h"
#include "util.h"

void main(void)
{
    init_idt();
    init_kbc();
    init_pic();
    asm volatile("sti");

    init_palette();
    init_background();
    init_keyboard();
    init_mouse();

    unsigned int memtotal =
        0x00400000 + memtest((void *)0x00400000, (void *)0xbfffffff);
    init_memman();
    freeb((void *)0x00001000, 0x0009e000);
    freeb((void *)0x00400000, memtotal - 0x00400000);
    printf("mem: %dMB free: %dKB\n", (int)memtotal / 1024 / 1024,
           (int)size_of_free() / 1024);

    init_layers();
    struct layer *mouse_layer = new_layer();
    unsigned char mousebuf_entity[16 * 16];
    unsigned char *mousebuf = mousebuf_entity;
    init_mousebuf(mousebuf);
    layer_setbuf(mouse_layer, mousebuf, 16, 16);
    layer_setpos(mouse_layer, 10, 10);
    layer_setheight(mouse_layer, 0);
    // (10, 10)にマウスの絵をレイヤーを使って表示させることはできた
    // マウス割り込みもレイヤー動かしにする
    // 背景とかもレイヤーに対応する
    // printfは……どうしようね

    while (1) {
        draw_keyinput();
        draw_mouseinput();
        asm volatile("cli");
        if (keyinput_empty() && mouseinput_empty()) {
            asm volatile("sti; hlt");
            continue;
        }
        asm volatile("sti");
    }
}

#include "graphic.h"
#include "asm.h"
#include "hankaku.h"
#include "mem.h"

#define PALETTE_INDEX 0x03c8
#define PALETTE_RGB 0x03c9

/* 320*200 */
unsigned char (*vram)[320] = (unsigned char (*)[320])0xa0000;

void init_palette(void)
{
    unsigned char palette_rgb[16 * 3] = {
        0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0xff, 0xff, 0x00,
        0x00, 0x00, 0xff, 0xff, 0x00, 0xff, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xc6, 0xc6, 0xc6, 0x84, 0x00, 0x00, 0x00, 0x84, 0x00, 0x84, 0x84, 0x00,
        0x00, 0x00, 0x84, 0x84, 0x00, 0x84, 0x00, 0x84, 0x84, 0x84, 0x84, 0x84,
    };

    out8(PALETTE_INDEX, 0);
    for (int i = 0; i < 16; ++i) {
        out8(PALETTE_RGB, palette_rgb[3 * i]);
        out8(PALETTE_RGB, palette_rgb[3 * i + 1]);
        out8(PALETTE_RGB, palette_rgb[3 * i + 2]);
    }
}

void drawsq(int x0, int y0, int x1, int y1, unsigned char color)
{
    for (int y = y0; y <= y1; ++y) {
        for (int x = x0; x <= x1; ++x) {
            vram[y][x] = color;
        }
    }
}

void drawbuf(int x0, int y0, int xsize, int ysize, char *buf)
{
    for (int y = 0; y < ysize; ++y) {
        for (int x = 0; x < xsize; ++x) {
            vram[y0 + y][x0 + x] = buf[y * xsize + x];
        }
    }
}

void drawchar(int x0, int y0, unsigned char color, char c)
{
    char *font = hankaku + c * 16;
    for (int y = 0; y < 16; ++y) {
        for (int x = 0; x < 8; ++x) {
            if (font[y] & (0x80 >> x)) {
                vram[y0 + y][x0 + x] = color;
            }
        }
    }
}

void init_background(void) { drawsq(0, 0, 320 - 1, 200 - 1, DARK_CYAN); }

void draw_mouse_cursor(int x0, int y0)
{
    int bgcolor = DARK_CYAN;

    char cursor[16][16] = {
        "**************..", "*OOOOOOOOOOO*...", "*OOOOOOOOOO*....",
        "*OOOOOOOOO*.....", "*OOOOOOOO*......", "*OOOOOOO*.......",
        "*OOOOOOO*.......", "*OOOOOOOO*......", "*OOOO**OOO*.....",
        "*OOO*..*OOO*....", "*OO*....*OOO*...", "*O*......*OOO*..",
        "**........*OOO*.", "*..........*OOO*", "............*OO*",
        "............****",
    };

    for (int y = 0; y < 16; ++y) {
        for (int x = 0; x < 16; ++x) {
            char c;
            switch (cursor[y][x]) {
            case '*':
                c = BLACK;
                break;
            case 'O':
                c = WHITE;
                break;
            case '.':
                c = bgcolor;
                break;
            default:
                break;
            }
            vram[y0 + y][x0 + x] = c;
        }
    }

    return;
}

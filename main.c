#include <stdarg.h>

#define BLACK	        0
#define LIGHT_RED	    1
#define LIGHT_GREEN	    2
#define LIGHT_YELLOW	3
#define LIGHT_BLUE	    4
#define LIGHT_PURPLE	5
#define LIGHT_CYAN	    6
#define WHITE	        7
#define LIGHT_GRAY      8
#define DARK_RED	    9
#define DARK_GREEN  	10
#define DARK_YELLOW 	11
#define DARK_BLUE   	12
#define DARK_PURPLE 	13
#define DARK_CYAN   	14
#define DARK_GRAY   	15

#define PIC0_ICW1	0x0020
#define PIC0_OCW2	0x0020
#define PIC0_IMR	0x0021
#define PIC0_ICW2	0x0021
#define PIC0_ICW3	0x0021
#define PIC0_ICW4	0x0021

#define PIC1_ICW1	0x00a0
#define PIC1_OCW2	0x00a0
#define PIC1_IMR	0x00a1
#define PIC1_ICW2	0x00a1
#define PIC1_ICW3	0x00a1
#define PIC1_ICW4	0x00a1

#define AR_INTGATE32    0x008e

struct gate_descriptor {
    short offset_low, selector;
    char hoge, access_right;
    short offset_high;
};

extern struct gate_descriptor idt[256];

extern void asm_inthandler21(void);

int load_eflags(void);
void store_eflags(int eflags);
void out8(short port, char data);

extern char hankaku[4096];

unsigned char (*vram)[320] = (unsigned char (*)[320])0xa0000;

int sprintf(char * restrict s, const char * restrict format, ...) {
    va_list ap;
    va_start(ap, format);
    int n = 0;
    while (*format != '\0') {
        if (*format == '%' && (*(format+1) == 'd')) {
            format += 2;
            
            int i = va_arg(ap, int);
            if (i == 0) {
                *s = '0';
                ++s;
                ++n;
                continue;
            }

            /* don't think about INT_MIN */
            if (i < 0) {
                *s = '-';
                ++s;
                ++n;

                i = -i;
            }

            int pow10 = 1;
            while ((i / (pow10*10)) > 0) {
                pow10 *= 10;
            }

            while (pow10 > 0) {
                *s = '0' + (i / pow10);
                ++s;
                ++n;
                i -= (i / pow10) * pow10;
                pow10 /= 10;
            }
        } else {
            *s = *format;
            ++s;
            ++format;
            ++n;
        }
    }
    *s = '\0';
    ++n;
    return n;
}

void init_palette(void) {
    unsigned char palette_rgb[16*3] = {
        0x00, 0x00, 0x00,
        0xff, 0x00, 0x00,
        0x00, 0xff, 0x00,
        0xff, 0xff, 0x00,
        0x00, 0x00, 0xff,
        0xff, 0x00, 0xff,
        0x00, 0xff, 0xff,
        0xff, 0xff, 0xff,
        0xc6, 0xc6, 0xc6,
        0x84, 0x00, 0x00,
        0x00, 0x84, 0x00,
        0x84, 0x84, 0x00,
        0x00, 0x00, 0x84,
        0x84, 0x00, 0x84,
        0x00, 0x84, 0x84,
        0x84, 0x84, 0x84,
    };

    int eflags = load_eflags();

    asm volatile("cli");

    out8(0x03c8, 0);
    for (int i = 0; i < 16; ++i) {
        out8(0x03c9, palette_rgb[3*i]);
        out8(0x03c9, palette_rgb[3*i + 1]);
        out8(0x03c9, palette_rgb[3*i + 2]);
    }

    store_eflags(eflags);
}

void boxfill(unsigned char color, int x0, int y0, int x1, int y1) {
    for (int y = y0; y <= y1; ++y) {
        for (int x = x0; x <= x1; ++x) {
            vram[y][x] = color;
        }
    }
    return;
}

void putstr(int x0, int y0, unsigned char color, char *str) {
    for (int i = 0; str[i] != '\0'; ++i) {
        char *font = hankaku + str[i]*16;
        for (int y = 0; y < 16; ++y) {
            for (int x = 0; x < 8; ++x) {
                if (font[y] & (0x80 >> x)) {
                    vram[y0+y][x0 + 8*i + x] = color;
                }
            }
        }
    }    
    return;
}

void init_mouse_cursor(char *mouse, char bgcolor) {
    char cursor[16][16] = {
        "**************..",
        "*OOOOOOOOOOO*...",
        "*OOOOOOOOOO*....",
        "*OOOOOOOOO*.....",
        "*OOOOOOOO*......",
        "*OOOOOOO*.......",
        "*OOOOOOO*.......",
        "*OOOOOOOO*......",
        "*OOOO**OOO*.....",
        "*OOO*..*OOO*....",
        "*OO*....*OOO*...",
        "*O*......*OOO*..",
        "**........*OOO*.",
        "*..........*OOO*",
        "............*OO*",
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
            mouse[y*16 + x] = c;
        }
    }

    return;
}

void putblock(int x0, int y0, int xsize, int ysize, char *buf) {
    for (int y = 0; y < ysize; ++y) {
        for (int x = 0; x < xsize; ++x) {
            vram[y0+y][x0+x] = buf[y*xsize + x];
        }
    }
    return;
}

void set_gate_descriptor(struct gate_descriptor *gd, int offset,
                         int selector, int access_right)
{
    gd->offset_low = offset & 0xffff;
    gd->selector = selector;
    gd->hoge = (access_right >> 8) & 0xff;
    gd->access_right = access_right & 0xff;
    gd->offset_high = (offset >> 16) & 0xffff;
    return;
}

void inthandler21(int *esp) {
    putstr(110, 110, WHITE, "key");
    while (1) {
        asm volatile("hlt");
    }
}

void init_idt(void) {
    set_gate_descriptor(idt+0x21, (int)asm_inthandler21, 2<<3, AR_INTGATE32);
}

void init_pic(void) {
    /* all interrupts to PIC should have been already masked */

    out8(PIC0_ICW1, 0x11);  /* initialize */
    out8(PIC0_ICW2, 0x20);	/* receive IRQ0~7 as INT0x20~0x27 */
    out8(PIC0_ICW3, 1<<2);  /* IRQ2 is connected to slave PIC */
    out8(PIC0_ICW4, 0x01);	/* x86 */
    
    out8(PIC1_ICW1, 0x11);	/* initialize */
    out8(PIC1_ICW2, 0x28);	/* receive IRQ8~15 as INT0x28~2f */
    out8(PIC1_ICW3, 2);     /* IRQ number the master PIC uses to connect to is 2 */
    out8(PIC1_ICW4, 0x01);	/* x86 */
    
    out8(PIC0_IMR, 0xfb);	/* mask 11111011 (only IRQ2=PIC1 is allowed) */
    out8(PIC1_IMR, 0xff);	/* mask 11111111 */
}

void main(void) {
    init_palette();
    init_pic();
    init_idt();
    out8(PIC0_IMR, 0xf9);
    asm volatile("sti");

    boxfill(DARK_CYAN, 0, 0, 320-1, 200-1);
    char s[100];
    sprintf(s, "hello, world %d", 3149);
    putstr(0, 0, WHITE, s);
    sprintf(s, "%d, %d", (int)asm_inthandler21, (int)idt);
    putstr(0, 16, WHITE, s);

    char mcursor[16*16];
    init_mouse_cursor(mcursor, DARK_CYAN);
    putblock(100, 100, 16, 16, mcursor);

    while (1) {
        asm volatile("hlt");
    }
}
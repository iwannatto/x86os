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

int load_eflags(void);
void store_eflags(int eflags);
void out8(short port, char data);

extern char hankaku[4096];

unsigned char (*vram)[320] = (unsigned char (*)[320])0xa0000;

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

void main(void) {
    init_palette();

    boxfill(DARK_CYAN, 0, 0, 320-1, 200-1);
    putstr(10, 10, WHITE, "hello, world");

    while (1) {
        asm volatile("hlt");
    }
}
#include "layer.h"
#include "graphic.h"
#include "mem.h"

#define NLAYERS 256
#define TOP_NOLAYER -1
#define FLAGS_UNUSED 0
#define FLAGS_INUSE 1
#define LAYER_NULL ((struct layer *)0)

// kaburu in graphic.c
#define XSIZE 320
#define YSIZE 200

/* 320*200 */
static unsigned char (*vram)[320] = (unsigned char (*)[320])0xa0000;

struct layers {
    int top;
    struct layer *p[NLAYERS];
    struct layer entity[NLAYERS];
};

static struct layers *ls;

void init_layers()
{
    ls = malloc4k(sizeof(struct layers));
    if (ls == NULL) {
        /* TODO: abort */
        return;
    }
    ls->top = TOP_NOLAYER;
    for (int i = 0; i < NLAYERS; i++) {
        ls->entity[i].flags = FLAGS_UNUSED;
    }
}

struct layer *new_layer(void)
{
    for (int i = 0; i < NLAYERS; i++) {
        if (ls->entity[i].flags == FLAGS_UNUSED) {
            struct layer *p = &ls->entity[i];
            p->flags = FLAGS_INUSE;
            p->height = -1;
            return p;
        }
    }
    return LAYER_NULL;
}

void layer_setbuf(struct layer *l, unsigned char *buf, int xsize, int ysize)
{
    l->buf = buf;
    l->xsize = xsize;
    l->ysize = ysize;
}

void reflesh(void);

void layer_setpos(struct layer *l, int x0, int y0)
{
    l->x0 = x0;
    l->y0 = y0;
    if (l->height >= 0) {
        reflesh();
    }
}

// #include "util.h"

void layer_setheight(struct layer *l, int newh)
{
    if (ls->top == TOP_NOLAYER) {
        ls->top = 0;
        ls->p[0] = l;
        l->height = 0;
        reflesh();
        return;
        // printf("hoe\n", 0);
    }

    int oldh = l->height;
    if (oldh == newh) {
        return;
    }

    newh = (newh > ls->top + 1) ? ls->top + 1 : newh;
    newh = (newh < -1) ? -1 : newh;
    l->height = newh;

    if (newh == -1 || (newh > oldh && oldh != -1)) {
        /* down */
        int lower = oldh;
        int higher = (newh == -1) ? ls->top : newh;
        if (newh == -1) {
            ls->top--;
        }
        for (int h = lower; h < higher; h++) {
            ls->p[h] = ls->p[h + 1];
            ls->p[h]->height = h;
        }
        if (newh > oldh && oldh != -1) {
            ls->p[newh] = l;
        }
    } else if (oldh == -1 || newh < oldh) {
        /* up */
        int lower = newh;
        int higher = (oldh == -1) ? ls->top + 1 : oldh;
        if (oldh == -1) {
            ls->top++;
        }
        for (int h = higher; h > lower; h++) {
            ls->p[h] = ls->p[h - 1];
            ls->p[h]->height = h;
        }
        if (newh < oldh) {
            ls->p[newh] = l;
        }
    }
    reflesh();
}

void reflesh(void)
{
    if (ls->top == TOP_NOLAYER) {
        return;
    }
    for (int h = 0; h <= ls->top; h++) {
        struct layer *l = ls->p[h];
        for (int y = 0; y < l->ysize; y++) {
            for (int x = 0; x < l->xsize; x++) {
                unsigned char c = l->buf[y * l->xsize + x];
                if (c != INV) {
                    vram[l->y0 + y][l->x0 + x] = c;
                }
            }
        }
    }
}

void free_layer(struct layer *l)
{
    if (l->height >= 0) {
        layer_setheight(l, -1);
    }
    l->flags = 0;
}

void init_mousebuf(unsigned char *buf)
{
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
            unsigned char c;
            switch (cursor[y][x]) {
            case '*':
                c = BLACK;
                break;
            case 'O':
                c = WHITE;
                break;
            case '.':
                c = INV;
                break;
            default:
                break;
            }
            buf[y * 16 + x] = c;
        }
    }
}

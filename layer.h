#pragma once

#define INV 99

struct layer {
    unsigned char *buf;
    int xsize, ysize, x0, y0, height, flags;
};

void init_layers();
struct layer *new_layer(void);
void layer_setbuf(struct layer *l, unsigned char *buf, int xsize, int ysize);
void layer_setpos(struct layer *l, int x0, int y0);
void layer_setheight(struct layer *l, int newh);

void init_mousebuf(unsigned char *buf);

#pragma once

#define BLACK 0
#define LIGHT_RED 1
#define LIGHT_GREEN 2
#define LIGHT_YELLOW 3
#define LIGHT_BLUE 4
#define LIGHT_PURPLE 5
#define LIGHT_CYAN 6
#define WHITE 7
#define LIGHT_GRAY 8
#define DARK_RED 9
#define DARK_GREEN 10
#define DARK_YELLOW 11
#define DARK_BLUE 12
#define DARK_PURPLE 13
#define DARK_CYAN 14
#define DARK_GRAY 15

void init_palette(void);
void init_background(void);
void drawsq(int x0, int y0, int x1, int y1, unsigned char color);
void drawchar(int x0, int y0, unsigned char color, char c);
void draw_mouse_cursor(int x0, int y0);

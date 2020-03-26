#pragma once

#define KEYBUF_READ_EMPTY -1
#define MOUSEBUF_READ_EMPTY -1

void init_idt(void);
int keybuf_read(int *status);
unsigned char mousebuf_read(int *status);

#pragma once

#define KEYBUF_READ_EMPTY -1

void init_idt(void);
int keybuf_read(void);

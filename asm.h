#pragma once

int load_eflags(void);
void store_eflags(int eflags);
void out8(short port, char data);

void asm_inthandler21(void);

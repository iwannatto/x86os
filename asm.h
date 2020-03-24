#pragma once

int load_eflags(void);
void store_eflags(int eflags);
char in8(short port);
void out8(short port, char data);

void asm_inthandler21(void);
void asm_inthandler2c(void);

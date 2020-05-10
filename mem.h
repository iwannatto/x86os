#pragma once

#define NULL 0

unsigned int memtest(void *start, void *end);
void init_memman(void);
unsigned int size_of_free(void);
void *malloc4k(unsigned int size);
int freeb(void *addr, unsigned int size);

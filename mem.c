#include "mem.h"

#define FREES 4090
#define FREE_SUCCESS 0
#define FREE_FAIL 1

struct freeinfo {
    void *addr;
    unsigned int size;
};

struct memman {
    int frees, maxfrees, lostsize, losts;
    struct freeinfo free[FREES];
};

extern struct memman mman;

unsigned int memtest(void *start, void *end)
{
    /* disalbe cache */
    unsigned int cr0;
    asm volatile("movl %%cr0, %0" : "=r"(cr0) : :);
    cr0 |= 0x60000000;
    asm volatile("movl %0, %%cr0" : : "r"(cr0) :);

    unsigned int s = 0;
    for (; start <= end; start += 0x1000) {
        volatile unsigned int *p = (volatile unsigned int *)start;
        unsigned int pat = 0xffffffff;
        unsigned int old = *p;
        *p = pat;
        if (*p != pat) {
            break;
        }
        *p = old;
        s += 0x1000;
    }

    /* enable cache */
    asm volatile("movl %%cr0, %0" : "=r"(cr0) : :);
    cr0 &= ~0x60000000;
    asm volatile("movl %0, %%cr0" : : "r"(cr0) :);

    return s;
}

void init_memman(void)
{
    mman.frees = 0;
    mman.maxfrees = 0;
    mman.lostsize = 0;
    mman.losts = 0;
}

unsigned int size_of_free(void)
{
    unsigned int s = 0;
    for (int i = 0; i < mman.frees; ++i) {
        s += mman.free[i].size;
    }
    return s;
}

void *malloc(unsigned int size)
{
    for (int i = 0; i < mman.frees; i++) {
        struct freeinfo *f = &(mman.free[i]);
        if (f->size >= size) {
            void *addr = f->addr;
            f->addr += size;
            f->size -= size;
            if (f->size == 0) {
                mman.frees--;
                for (; mman.frees; i++) {
                    mman.free[i] = mman.free[i + 1];
                }
            }
            return addr;
        }
    }
    return NULL;
}

int free(void *addr, unsigned int size)
{
    int i;
    for (i = 0; i < mman.frees; i++) {
        if (mman.free[i].addr > addr) {
            break;
        }
    }

    int contiguous_prev = 0;
    if (i > 0 && (mman.free[i - 1].addr + mman.free[i - 1].size == addr)) {
        contiguous_prev = 1;
    }

    int contiguous_next = 0;
    if (i < mman.frees && (addr + size == mman.free[i].addr)) {
        contiguous_next = 1;
    }

    if (contiguous_prev && contiguous_next) {
        mman.free[i - 1].size += size + mman.free[i].size;

        mman.frees--;
        for (; i < mman.frees; i++) {
            mman.free[i] = mman.free[i + 1];
        }
        return FREE_SUCCESS;
    }

    if (contiguous_prev) {
        mman.free[i - 1].size += size;
        return FREE_SUCCESS;
    }

    if (contiguous_next) {
        mman.free[i].addr = addr;
        mman.free[i].size += size;
        return FREE_SUCCESS;
    }

    if (mman.frees < FREES) {
        for (int j = mman.frees; j > i; j--) {
            mman.free[j] = mman.free[j - 1];
        }
        mman.frees++;
        mman.maxfrees =
            (mman.frees > mman.maxfrees) ? mman.frees : mman.maxfrees;
        mman.free[i].addr = addr;
        mman.free[i].size = size;
        return FREE_SUCCESS;
    }

    mman.losts++;
    mman.lostsize += size;
    return FREE_FAIL;
}

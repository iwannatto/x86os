#include "mem.h"

#define MAXNFREE 4090
#define FREE_SUCCESS 0
#define FREE_FAIL 1

struct region {
    void *addr;
    unsigned int size;
};

struct memman {
    int nfree, maxnfree, lostsize, nlost;
    struct region free[MAXNFREE];
};

extern struct memman mman;

unsigned int memtest(void *start, void *end)
{
    /* disalbe cache */
    unsigned int cr0;
    const unsigned int CR0_CD_NW = 0x60000000;
    asm volatile("movl %%cr0, %0" : "=r"(cr0) : :);
    cr0 |= CR0_CD_NW;
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
    cr0 &= ~CR0_CD_NW;
    asm volatile("movl %0, %%cr0" : : "r"(cr0) :);

    return s;
}

void init_memman(void)
{
    mman.nfree = 0;
    mman.maxnfree = 0;
    mman.lostsize = 0;
    mman.nlost = 0;
}

unsigned int size_of_free(void)
{
    unsigned int s = 0;
    for (int i = 0; i < mman.nfree; ++i) {
        s += mman.free[i].size;
    }
    return s;
}

void *mallocb(unsigned int size)
{
    for (int i = 0; i < mman.nfree; i++) {
        struct region *f = &(mman.free[i]);
        if (f->size >= size) {
            void *addr = f->addr;
            f->addr += size;
            f->size -= size;
            if (f->size == 0) {
                mman.nfree--;
                for (; mman.nfree; i++) {
                    mman.free[i] = mman.free[i + 1];
                }
            }
            return addr;
        }
    }
    return NULL;
}

void *malloc4k(unsigned int size)
{
    return mallocb((size + 0xfff) & 0xfffff000);
}

int freeb(void *addr, unsigned int size)
{
    int i;
    for (i = 0; i < mman.nfree; i++) {
        if (mman.free[i].addr > addr) {
            break;
        }
    }

    int contiguous_prev = 0;
    if (i > 0 && (mman.free[i - 1].addr + mman.free[i - 1].size == addr)) {
        contiguous_prev = 1;
    }

    int contiguous_next = 0;
    if (i < mman.nfree && (addr + size == mman.free[i].addr)) {
        contiguous_next = 1;
    }

    if (contiguous_prev && contiguous_next) {
        mman.free[i - 1].size += size + mman.free[i].size;

        mman.nfree--;
        for (; i < mman.nfree; i++) {
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

    if (mman.nfree < MAXNFREE) {
        for (int j = mman.nfree; j > i; j--) {
            mman.free[j] = mman.free[j - 1];
        }
        mman.nfree++;
        mman.maxnfree =
            (mman.nfree > mman.maxnfree) ? mman.nfree : mman.maxnfree;
        mman.free[i].addr = addr;
        mman.free[i].size = size;
        return FREE_SUCCESS;
    }

    mman.nlost++;
    mman.lostsize += size;
    return FREE_FAIL;
}

int free4k(void *addr, unsigned int size)
{
    return freeb(addr, (size + 0xfff) & 0xfffff000);
}

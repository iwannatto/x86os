#include "ringbuf.h"

void init_ringbuf(struct ringbuf *rbuf)
{
    for (int i = 0; i < RINGBUF_SIZE; ++i) {
        rbuf->buf[i] = 0;
        rbuf->head = 0;
        rbuf->tail = 0;
    }
}

int ringbuf_write(struct ringbuf *rbuf, char x)
{
    int h = (rbuf->head + 1) % RINGBUF_SIZE;
    if (h == rbuf->tail) {
        return RINGBUF_WRITE_FULL;
    }

    rbuf->buf[rbuf->head] = x;
    rbuf->head = h;
    return RINGBUF_WRITE_SUCCESS;
}

char ringbuf_read(struct ringbuf *rbuf, int *status)
{
    if (ringbuf_empty(*rbuf)) {
        *status = RINGBUF_READ_EMPTY;
        return 0;
    }

    char x = rbuf->buf[rbuf->tail];
    rbuf->tail = (rbuf->tail + 1) % RINGBUF_SIZE;
    *status = RINGBUF_READ_SUCCESS;
    return x;
}

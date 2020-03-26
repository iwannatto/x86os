#pragma once

#define RINGBUF_SIZE 256

#define RINGBUF_WRITE_SUCCESS 0
#define RINGBUF_WRITE_FULL 1
#define RINGBUF_READ_SUCCESS 0
#define RINGBUF_READ_EMPTY 1

struct ringbuf {
    char buf[RINGBUF_SIZE];
    int head, tail;
};

void init_ringbuf(struct ringbuf *rbuf);

#define ringbuf_empty(rbuf) ((rbuf).tail == ((rbuf).head))

int ringbuf_write(struct ringbuf *rbuf, char x);
char ringbuf_read(struct ringbuf *rbuf, int *status);

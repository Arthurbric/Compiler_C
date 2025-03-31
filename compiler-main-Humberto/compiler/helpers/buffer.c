#include "buffer.h"
#include <stdlib.h>
#include <string.h>

struct buffer* buffer_create() {
    struct buffer* buf = malloc(sizeof(struct buffer));
    buf->data = malloc(32);
    buf->len = 0;
    buf->cap = 32;
    return buf;
}

void buffer_write(struct buffer* buf, char c) {
    if (buf->len >= buf->cap) {
        buf->cap *= 2;
        buf->data = realloc(buf->data, buf->cap);
    }
    buf->data[buf->len++] = c;
}

const char* buffer_ptr(struct buffer* buf) {
    return buf->data;
}

void buffer_free(struct buffer* buf) {
    free(buf->data);
    free(buf);
}
#ifndef BUFFER_H
#define BUFFER_H

struct buffer {
    char* data;
    int len;
    int cap;
};

struct buffer* buffer_create();
void buffer_write(struct buffer* buf, char c);
const char* buffer_ptr(struct buffer* buf);
void buffer_free(struct buffer* buf);

#endif
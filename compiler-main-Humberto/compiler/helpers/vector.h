#ifndef VECTOR_H
#define VECTOR_H

typedef struct {
    void** data;
    int size;
    int capacity;
    int peek_index;  // Adicione esta linha
} Vector;

Vector* vector_create();
void vector_free(Vector* vec);
void vector_push(Vector* vec, void* item);
void* vector_pop(Vector* vec);
void* vector_peek(Vector* vec);
void* vector_back_or_null(Vector* vec);
void vector_set_peek_pointer(Vector* vec, int index);
void* vector_peek_at(Vector* vec, int index);

#endif
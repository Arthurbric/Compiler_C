#include "vector.h"
#include <stdlib.h>
#include <string.h>

/**
 * Cria um novo vetor dinâmico
 */
Vector* vector_create() {
    Vector* vec = malloc(sizeof(Vector));
    if (!vec) return NULL;
    
    vec->size = 0;
    vec->capacity = 16;
    vec->peek_index = 0;  // Inicializa o índice de peek
    vec->data = malloc(sizeof(void*) * vec->capacity);
    
    if (!vec->data) {
        free(vec);
        return NULL;
    }
    return vec;
}

/**
 * Libera a memória do vetor
 */
void vector_free(Vector* vec) {
    if (vec) {
        free(vec->data);
        free(vec);
    }
}

/**
 * Adiciona um item ao final do vetor
 */
void vector_push(Vector* vec, void* item) {
    if (!vec) return;
    
    // Redimensiona se necessário
    if (vec->size >= vec->capacity) {
        vec->capacity *= 2;
        void** new_data = realloc(vec->data, sizeof(void*) * vec->capacity);
        if (!new_data) return;
        vec->data = new_data;
    }
    
    vec->data[vec->size++] = item;
}

/**
 * Remove e retorna o último item
 */
void* vector_pop(Vector* vec) {
    if (!vec || vec->size == 0) return NULL;
    return vec->data[--vec->size];
}

/**
 * Retorna o último item sem remover
 */
void* vector_peek(Vector* vec) {
    if (!vec || vec->size == 0) return NULL;
    return vec->data[vec->size - 1];
}

/**
 * Retorna o último item ou NULL (alias para vector_peek)
 */
void* vector_back_or_null(Vector* vec) {
    return vector_peek(vec);
}

/**
 * Define a posição do ponteiro de peek
 */
void vector_set_peek_pointer(Vector* vec, int index) {
    if (vec && index >= 0 && index < vec->size) {
        vec->peek_index = index;
    }
}

/**
 * Retorna o próximo item usando peek_index e avança o índice
 */
void* vector_peek_next(Vector* vec) {
    if (!vec || vec->peek_index >= vec->size) {
        return NULL;
    }
    return vec->data[vec->peek_index++];
}

/**
 * Retorna o item em uma posição específica
 */
void* vector_peek_at(Vector* vec, int index) {
    if (!vec || index < 0 || index >= vec->size) {
        return NULL;
    }
    return vec->data[index];
}

/**
 * Retorna o tamanho atual do vetor
 */
int vector_size(Vector* vec) {
    return vec ? vec->size : 0;
}
#ifndef COMPILER_H
#define COMPILER_H

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>
#include "helpers/vector.h"
#include "helpers/buffer.h"

// Estados do compilador
enum {
    COMPILER_FILE_COMPILED_OK,
    COMPILER_FAILED_WITH_ERRORS
};

// Estados da análise léxica
enum {
    LEXICAL_ANALYSIS_ALL_OK,
    LEXICAL_ANALYSIS_INPUT_ERROR
};


// Tipos de tokens
enum {
    TOKEN_TYPE_KEYWORD,
    TOKEN_TYPE_IDENTIFIER,
    TOKEN_TYPE_OPERATOR,
    TOKEN_TYPE_SYMBOL,
    TOKEN_TYPE_NUMBER,
    TOKEN_TYPE_STRING,
    TOKEN_TYPE_COMMENT,
    TOKEN_TYPE_NEWLINE
};

// Posição no código
struct pos {
    int line;
    int col;
    const char* filename;
};

// Token
struct token {
    int type;
    int flags;
    struct pos pos;
    union {
        char cval;
        const char* sval;
        unsigned int inum;
        unsigned long lnum;
        unsigned long long llnum;
        void* any;
    };
    bool whitespace;
    const char* between_brackets;
};

// Processo de compilação
struct compile_process {
    int flags;
    struct pos pos;
    struct {
        FILE* fp;
        const char* abs_path;
    } cfile;
    FILE* ofile;
};

// Funções do lexer
struct lex_process;
typedef char (*LEX_PROCESS_NEXT_CHAR)(struct lex_process* process);
typedef char (*LEX_PROCESS_PEEK_CHAR)(struct lex_process* process);
typedef void (*LEX_PROCESS_PUSH_CHAR)(struct lex_process* process, char c);

struct lex_process_functions {
    LEX_PROCESS_NEXT_CHAR next_char;
    LEX_PROCESS_PEEK_CHAR peek_char;
    LEX_PROCESS_PUSH_CHAR push_char;
};

struct lex_process {
    struct pos pos;
    Vector* token_vec;  // Alterado de struct vector* para Vector*
    struct compile_process* compiler;
    struct lex_process_functions* functions;
    void* private;
};

// Declarações de funções
struct compile_process* compile_process_create(const char* filename, const char* filename_out, int flags);
char compile_process_next_char(struct lex_process* lex_process);
char compile_process_peek_char(struct lex_process* lex_process);
void compile_process_push_char(struct lex_process* lex_process, char c);

struct lex_process* lex_process_create(struct compile_process* compiler, 
                                     struct lex_process_functions* functions, 
                                     void* private);
void lex_process_free(struct lex_process* process);
void* lex_process_private(struct lex_process* process);
Vector* lex_process_tokens(struct lex_process* process);
struct token* token_create(struct token* _token);
struct token* token_make_number();
struct token* token_make_number_for_value(unsigned long long num);
int lex(struct lex_process* process);

void compiler_error(struct compile_process* compiler, const char* msg, ...);

#define NUMERIC_CASE \
    case '0': case '1': case '2': case '3': case '4': \
    case '5': case '6': case '7': case '8': case '9'


#endif

// Buffer functions
struct buffer* buffer_create();
void buffer_write(struct buffer* buf, char c);
const char* buffer_ptr(struct buffer* buf);

// Vector functions
void vector_free(Vector* vec);

// Error handling
void compiler_error(struct compile_process* compiler, const char* msg, ...);
#ifndef COMPILER_H
#define COMPILER_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "compiler_cases.h"

enum {
    COMPILER_FILE_COMPILED_OK,
    COMPILER_FAILED_WITH_ERRORS
};

enum {
    LEXICAL_ANALYSIS_ALL_OK,
    LEXICAL_ANALYSIS_INPUT_ERROR
};

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

struct pos {
    int line;
    int col;
    const char *filename;
};

struct compile_process {
    // Como o arquivo deve ser compilado
    int flags;

    struct pos pos;

    struct compile_process_input_file {
        FILE *fp;
        const char *abs_path;
    } cfile;

    FILE *ofile;
};

struct compile_process_options {
    int flags; // Adicione flags conforme necessário
};

struct token {
    int type;
    int flags;

    struct pos pos; // Identificar onde o token está no arquivo

    union {
        char cval;
        const char *sval;
        unsigned int inum;
        unsigned long lnum;
        unsigned long long llnum;
        void *any;
    };

    // Verdadeiro se tiver um espaço entre um token e o próximo token
    bool whitespace;

    // (1+2+3) ==> 1+2+3
    const char *between_brackets;
};

struct lex_process;

typedef char (*LEX_PROCESS_NEXT_CHAR) (struct lex_process *process);
typedef char (*LEX_PROCESS_PEEK_CHAR) (struct lex_process *process);
typedef void (*LEX_PROCESS_PUSH_CHAR) (struct lex_process *process, char c);

struct lex_process_functions {
    LEX_PROCESS_NEXT_CHAR next_char;
    LEX_PROCESS_PEEK_CHAR peek_char;
    LEX_PROCESS_PUSH_CHAR push_char;
};

struct lex_process {
    struct pos pos;
    struct vector *token_vec;
    struct compile_process *compiler;

    int current_expression_count; // Qtd de parênteses que existem no momento

    struct buffer *parentheses_buffer;
    struct lex_process_functions *function;

    void *private; // Dados que o programador entende
};


/* Declarações de funções externas */

// compiler.c
int compile_file(const char *filename, const char *out_filename, int flags);

// cprocess.c
char compile_process_next_char(struct lex_process *lex_process);
char compile_process_peek_char(struct lex_process *lex_process);
void compile_process_push_char(struct lex_process *lex_process, char c);
struct compile_process* compile_process_create(const char* filename, const char* filename_out, int flags); 

// lex_process.c
struct lex_process *lex_process_create(struct compile_process *compiler, struct lex_process_functions *functions, void *private);
void lex_process_free(struct lex_process *process);
void *lex_process_private(struct lex_process *process);
struct vector *lex_process_tokens(struct lex_process *process);

// lexer.c
int lex(struct lex_process *process);
// token_utils.c
const char* token_type_to_str(int type);
void token_print(struct token* token);


#endif
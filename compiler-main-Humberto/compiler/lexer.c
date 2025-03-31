#include "compiler.h"
#include <stdlib.h>
#include "helpers/buffer.h"

static struct lex_process* lex_process;

static char peekc() {
    return lex_process->functions->peek_char(lex_process);
}

static char nextc() {
    char c = lex_process->functions->next_char(lex_process);
    if (c == '\n') {
        lex_process->pos.line++;
        lex_process->pos.col = 1;
    } else {
        lex_process->pos.col++;
    }
    return c;
}

static void pushc(char c) {
    lex_process->functions->push_char(lex_process, c);
}

static struct pos lex_file_position() {
    return lex_process->pos;
}

struct token* token_create(struct token* _token) {
    static struct token token;
    memcpy(&token, _token, sizeof(struct token));
    token.pos = lex_process->pos;
    return &token;
}

static struct token* lexer_last_token() {
    return vector_back_or_null(lex_process->token_vec);
}

static const char* read_number_str() {
    struct buffer* buffer = buffer_create();
    char c = peekc();
    
    while (c >= '0' && c <= '9') {
        buffer_write(buffer, c);
        nextc();
        c = peekc();
    }
    buffer_write(buffer, 0x00);
    return buffer_ptr(buffer);
}

static unsigned long long read_number() {
    const char* num_str = read_number_str();
    return atoll(num_str);
}

struct token* token_make_number() {
    return token_make_number_for_value(read_number());
}

struct token* read_next_token() {
    char c = peekc();
    switch (c) {
        case EOF:
            return NULL;
            
        NUMERIC_CASE:
            return token_make_number();
            
        case ' ':
        case '\t':
            nextc();
            return read_next_token();
            
        case '\n':
            nextc();
            return token_create(&(struct token){
                .type = TOKEN_TYPE_NEWLINE
            });
            
        default:
            compiler_error(lex_process->compiler, "Token inválido: '%c'\n", c);
            return NULL;
    }
}

struct token* token_make_number_for_value(unsigned long long num) {
    struct token token = {
        .type = TOKEN_TYPE_NUMBER,
        .llnum = num
    };
    return token_create(&token);
}

// Corrija a função lex
int lex(struct lex_process* process) {
    lex_process = process;
    process->pos.filename = process->compiler->cfile.abs_path;

    struct token* token = read_next_token();
    while (token) {
        vector_push((Vector*)process->token_vec, token);  // Cast explícito
        token = read_next_token();
    }
    return LEXICAL_ANALYSIS_ALL_OK;
}
void print_token(struct token* token) {
    const char* type_str = "";
    switch(token->type) {
        case TOKEN_TYPE_KEYWORD:    type_str = "KEYWORD"; break;
        case TOKEN_TYPE_IDENTIFIER: type_str = "IDENTIFIER"; break;
        case TOKEN_TYPE_NUMBER:    type_str = "NUMBER"; break;
        // ... complete para outros tipos
    }

    printf("[%s] %s (Linha %d, Col %d)\n", 
           type_str,
           token_type_to_str(token), // Implemente esta função também
           token->pos.line, 
           token->pos.col);
}
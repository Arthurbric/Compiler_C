#include "compiler.h"
#include <string.h>
#include <ctype.h>
#include "helpers/vector.h"
#include "helpers/buffer.h"

const char* token_type_to_str(int type) {
    switch(type) {
        case TOKEN_TYPE_KEYWORD:    return "KEYWORD";
        case TOKEN_TYPE_IDENTIFIER: return "IDENTIFIER";
        case TOKEN_TYPE_OPERATOR:   return "OPERATOR";
        case TOKEN_TYPE_SYMBOL:     return "SYMBOL";
        case TOKEN_TYPE_NUMBER:     return "NUMBER";
        case TOKEN_TYPE_STRING:     return "STRING";
        case TOKEN_TYPE_COMMENT:    return "COMMENT";
        case TOKEN_TYPE_NEWLINE:    return "NEWLINE";
        default:                    return "UNKNOWN";
    }
}

void token_print(struct token* token) {
    if (!token) {
        printf("Token: NULL\n");
        return;
    }
    
    printf("Token [%s]: ", token_type_to_str(token->type));
    
    switch(token->type) {
        case TOKEN_TYPE_NUMBER:
            printf("%llu", token->llnum);
            break;
        case TOKEN_TYPE_STRING:
        case TOKEN_TYPE_SYMBOL:
            printf("\"%s\"", token->sval);
            break;
        case TOKEN_TYPE_OPERATOR:
            printf("'%c'", token->cval);
            break;
        default:
            printf("(valor não impresso)");
    }
    
    printf(" (linha %d, col %d)\n", token->pos.line, token->pos.col);
}

#define LEX_GETC_IF(buffer, c, exp) \
    for (c = peekc(); exp; c = peekc()) { \
        buffer_write(buffer, c); \
        nextc(); \
    }

struct token *read_next_token();
static struct lex_process *lex_process;
static struct token tmp_token;

static char peekc() {
    return lex_process->function->peek_char(lex_process);
}

static char nextc() {
    char c = lex_process->function->next_char(lex_process);
    lex_process->pos.col += 1;
    if (c == '\n') {
        lex_process->pos.line += 1;
        lex_process->pos.col = 1;
    }
    return c;
}

// Adicionar um caractere no arquivo
static void pushc(char c) {
    lex_process->function->push_char(lex_process, c);
}

static struct pos lex_file_position() {
    return lex_process->pos;
}

struct token *token_create(struct token *_token) {
    memcpy(&tmp_token, _token, sizeof(struct token));
    tmp_token.pos = lex_file_position();

    return &tmp_token;
}

static struct token *lexer_last_token() {
    return vector_back_or_null(lex_process->token_vec);
}

static struct token *handle_whitespace() {
    struct token *last_token = lexer_last_token();
    if (last_token)
        last_token->whitespace = true;

    nextc();
    return read_next_token();
}

const char *read_number_str() {
    const char *num = NULL;
    struct buffer *buffer = buffer_create();
    char c = peekc();
    LEX_GETC_IF(buffer, c, (c >= '0' && c <= '9'));

    buffer_write(buffer, 0x00); // Finaliza a string

    printf("Token (Number): %s\n", buffer->data);

    return buffer_ptr(buffer);
}

const char *read_symbol_str() {
    const char *str = NULL;
    struct buffer *buffer = buffer_create();
    char c = peekc();
    buffer_write(buffer, c);
    nextc();

    buffer_write(buffer, 0x00);

    printf("Token (Symbol): %s\n", buffer->data);

    return buffer_ptr(buffer);
}

const char *read_string_str() {
    struct buffer *buffer = buffer_create();
    char c = nextc();
    LEX_GETC_IF(buffer, c, (isalpha(c)));

    buffer_write(buffer, 0x00); // Finaliza a string

    printf("Token (String): %s\n", buffer->data);

    return buffer_ptr(buffer);
}

unsigned long long read_number() {
    const char *s = read_number_str();
    return atoll(s);
}

const char *read_symbol() {
    const char *s = read_symbol_str();
    return s;
}

const char *read_string() {
    const char *s = read_string_str();
    return s;
}

struct token *token_make_number_for_value(unsigned long number) {
    return token_create(&(struct token) {
        .type=TOKEN_TYPE_SYMBOL, .llnum=number
    });
}

struct token *token_make_symbol_for_value(const char *str) {
    return token_create(&(struct token) {
        .type=TOKEN_TYPE_SYMBOL, .sval=str
    });
}

struct token *token_make_string_for_value(const char *str) {
    return token_create(&(struct token) {
        .type=TOKEN_TYPE_STRING, .sval=str
    });
}



struct token *token_make_number() {
    return token_make_number_for_value(read_number());
}

struct token *token_make_symbol() {
    return token_make_symbol_for_value(read_symbol());
}

struct token *token_make_string() {
    return token_make_string_for_value(read_string());
}

struct token *read_next_token() {
    struct token *token = NULL;
    char c = peekc();
    switch (c) 
    {
    case EOF:
        break;
    
    NUMERIC_CASE:
        token = token_make_number();
    break;

    SYMBOL_CASE:
        token = token_make_symbol();
    break;

    STRING_CASE:
        token = token_make_string();
    break;

    case ' ':
    case '\t':
        token = handle_whitespace();
    break;

    default:
        //compiler_error(lex_process->compiler, "Token inválido!\n");
        break;
    }

    return token;
}

int lex(struct lex_process *process) {
    process->current_expression_count = 0;
    process->parentheses_buffer = NULL;
    lex_process = process;
    process->pos.filename = process->compiler->cfile.abs_path;

    struct token *token = read_next_token();

    while (token) {
        vector_push(process->token_vec, token);
        token_print(token);  // Debug: imprime cada token
        token = read_next_token();
    }

    return LEXICAL_ANALYSIS_ALL_OK;
}


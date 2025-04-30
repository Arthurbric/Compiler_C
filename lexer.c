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
            printf("\"%s\"", token->sval);
            break;
        case TOKEN_TYPE_SYMBOL:
            printf("\"%c\"", token->cval);
            break;
        case TOKEN_TYPE_OPERATOR:
            printf("'%s'", token->sval);
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

struct token *token_make_newline() {
    nextc();
    return token_create(&(struct token){.type=TOKEN_TYPE_NEWLINE});
}

static struct token *handle_whitespace() {
    struct token *last_token = lexer_last_token();
    if (last_token)
        last_token->whitespace = true;

    nextc();
    return read_next_token();
}

struct token *token_make_one_line_comment() {
    struct buffer *buffer = buffer_create();
    char c = 0;
    LEX_GETC_IF(buffer, c, c != '\n' && c != EOF);

    return token_create(&(struct token){.type=TOKEN_TYPE_COMMENT,.sval=buffer_ptr(buffer)});
}

struct token *token_make_multiline_comment() {
    struct buffer *buffer = buffer_create();
    char c = 0;
    while (1) {
        LEX_GETC_IF(buffer, c, c != '*' && c != EOF);

        if (c == EOF) {
            compiler_error(lex_process->compiler, "O comentário não foi fechado\n");
        } else if (c == '*') {
            nextc();

            if (peekc() == '/') {
                nextc();
                break;
            }
        }
    }

    return token_create(&(struct token){.type=TOKEN_TYPE_COMMENT,.sval=buffer_ptr(buffer)});
}

static struct token *make_identifier_or_keyword() {
    struct buffer *buffer = buffer_create();
    char c;
    LEX_GETC_IF(buffer, c, (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_')
    // Não entendi ao certo porque tem dois "|| (c >= 'A' && c <= 'Z') ||"

    buffer_write(buffer, 0x00);
    printf("Token: %s\n", buffer->data);

    if (is_keyword(buffer_ptr(buffer)))
        return token_create(&(struct token){.type=TOKEN_TYPE_KEYWORD,.sval=buffer_ptr(buffer)});
    return token_create(&(struct token){.type=TOKEN_TYPE_IDENTIFIER,.sval=buffer_ptr(buffer)});
}

struct token *read_special_token() {
    char c = peekc();
    if (isalpha(c) || c == '_')
        return make_identifier_or_keyword();

    return NULL;
}

static void lex_new_expression() {
    lex_process->current_expression_count++;

    if (lex_process->current_expression_count == 1)
        lex_process->parentheses_buffer = buffer_create();
}

static void lex_fisish_expression() {
    lex_process->current_expression_count--;

    if (lex_process->current_expression_count < 0)
        compiler_error(lex_process->compiler, "Você fechou uma expressão nunca iniciada!\n");
}

bool lex_is_in_expression() {
    return lex_process->current_expression_count > 0;
}

static struct token *token_make_symbol() {
    char c = nextc();
    if (c == ')')
        lex_fisish_expression();

    struct token *token = token_create(&(struct token){.type=TOKEN_TYPE_SYMBOL,.cval=c});
    return token;
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

const char *read_string_str(char i, char j) {
    struct buffer *buffer = buffer_create();

    char c = nextc();
    LEX_GETC_IF(buffer, c, (c != j));
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

const char *read_string(char i, char j) {
    const char *s = read_string_str(i, j);
    return s;
}

struct token *token_make_number_for_value(unsigned long number) {
    return token_create(&(struct token) {
        .type=TOKEN_TYPE_NUMBER, .llnum=number
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

// struct token *token_make_symbol() {
//     return token_make_symbol_for_value(read_symbol());
// }

struct token *token_make_string(char i, char j) {
    const char *str = read_string(i, j);
    nextc(); // Isso aqui quase que deixa sem dormir

    return token_make_string_for_value(str);
}

static char *read_op() {
    struct buffer *buffer = buffer_create();
    char c = peekc();

    buffer_write(buffer, c);
    nextc();

    char next = peekc();
    if ((c == '=' && next == '=') || 
        (c == '!' && next == '=') ||  
        (c == '<' && next == '=') ||  
        (c == '>' && next == '=') ||  
        (c == '&' && next == '&') ||  
        (c == '|' && next == '|')) {  
        buffer_write(buffer, next);
        nextc();
    }

    buffer_write(buffer, 0x00);
    return buffer_ptr(buffer);
}

static struct token *token_make_operator_or_string() {
    char op = peekc();

    if (op == '<') {
        struct token *last_token = lexer_last_token();
        if (token_is_keyword(last_token, "include"))
            return token_make_string('<', '>');
    }

    struct token *token = token_create(&(struct token){.type=TOKEN_TYPE_OPERATOR,.sval=read_op()});
    if (op == '(') {
        lex_new_expression();
    }

    return token;
}

struct token *handle_comment() {
    char c = peekc();
    if (c == '/') {
        nextc();
        if (peekc() == '/') {
            nextc();
            return token_make_one_line_comment();
        } else if (peekc() == '*') {
            nextc();
            return token_make_multiline_comment();
        }

        pushc('/');
        return token_make_operator_or_string();
    }

    return NULL;
}

struct token *read_next_token() {
    struct token *token = NULL;
    char c = peekc();

    token = handle_comment();
    if (token) return token;

    switch (c) {
    case EOF:
        break;
    NUMERIC_CASE:
        token = token_make_number();
    break;
    OPERATOR_CASE:
        token = token_make_operator_or_string();
        break;
    SYMBOL_CASE:
        token = token_make_symbol();
    break;
    case '"':
        token = token_make_string('"', '"');
        break;
    case '\t':
    case ' ':
        token = handle_whitespace();
        break;
    case '\n':
        token = token_make_newline(); // Sem declaração
        break;

    default:
        token = read_special_token();
        if (!token)
            compiler_error(lex_process->compiler, "Token inválido!\n");
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


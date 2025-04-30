#ifndef COMPILER_H
#define COMPILER_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define NUMERIC_CASE \
    case '0':       \
    case '1':       \
    case '2':       \
    case '3':       \
    case '4':       \
    case '5':       \
    case '6':       \
    case '7':       \
    case '8':       \
    case '9'

#define SYMBOL_CASE \
    case '{':       \
    case '}':       \
    case ':':       \
    case ';':       \
    case '#':       \
    case ')':       \
    case ']':       \
    case '\\'

#define OPERATOR_CASE \
    case '+':        \
    case '-':        \
    case '*':        \
    case '>':        \
    case '<':        \
    case '^':        \
    case '%':        \
    case '!':        \
    case '=':        \
    case '~':        \
    case '|':        \
    case '&':        \
    case '(':        \
    case '[':        \
    case ',':        \
    case '?'

#define token_is_keyword(last_token, str) (strcmp(last_token->sval, str) == 0)

#define is_keyword(keyword) ( \
    strcmp(keyword, "unsigned") == 0 || \
    strcmp(keyword, "signed") == 0 || \
    strcmp(keyword, "char") == 0 || \
    strcmp(keyword, "short") == 0 || \
    strcmp(keyword, "int") == 0 || \
    strcmp(keyword, "long") == 0 || \
    strcmp(keyword, "float") == 0 || \
    strcmp(keyword, "double") == 0 || \
    strcmp(keyword, "void") == 0 || \
    strcmp(keyword, "struct") == 0 || \
    strcmp(keyword, "union") == 0 || \
    strcmp(keyword, "static") == 0 || \
    strcmp(keyword, "__ignore_typecheck") == 0 || \
    strcmp(keyword, "return") == 0 || \
    strcmp(keyword, "include") == 0 || \
    strcmp(keyword, "sizeof") == 0 || \
    strcmp(keyword, "if") == 0 || \
    strcmp(keyword, "else") == 0 || \
    strcmp(keyword, "while") == 0 || \
    strcmp(keyword, "for") == 0 || \
    strcmp(keyword, "do") == 0 || \
    strcmp(keyword, "break") == 0 || \
    strcmp(keyword, "continue") == 0 || \
    strcmp(keyword, "switch") == 0 || \
    strcmp(keyword, "case") == 0 || \
    strcmp(keyword, "default") == 0 || \
    strcmp(keyword, "goto") == 0 || \
    strcmp(keyword, "typedef") == 0 || \
    strcmp(keyword, "const") == 0 || \
    strcmp(keyword, "extern") == 0 || \
    strcmp(keyword, "restrict") == 0 \
)

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

enum {
    NODE_TYPE_EXPRESSION,
    NODE_TYPE_EXPRESSION_PARENTHESES,
    NODE_TYPE_NUMBER,
    NODE_TYPE_IDENTIFIER,
    NODE_TYPE_STRING,
    NODE_TYPE_VARIABLE,
    NODE_TYPE_VARIABLE_LIST,
    NODE_TYPE_FUNCTION,
    NODE_TYPE_BODY,
    NODE_TYPE_STATEMENT_RETURN,
    NODE_TYPE_STATEMENT_IF,
    NODE_TYPE_STATEMENT_ELSE,
    NODE_TYPE_STATEMENT_WHILE,
    NODE_TYPE_STATEMENT_DO_WHILE,
    NODE_TYPE_STATEMENT_FOR,
    NODE_TYPE_STATEMENT_BREAK,
    NODE_TYPE_STATEMENT_CONTINUE,
    NODE_TYPE_STATEMENT_SWITCH,
    NODE_TYPE_STATEMENT_CASE,
    NODE_TYPE_STATEMENT_DEFAULT,
    NODE_TYPE_STATEMENT_GOTO,
    NODE_TYPE_UNARY,
    NODE_TYPE_TENARY,
    NODE_TYPE_LABEL,
    NODE_TYPE_STRUCT,
    NODE_TYPE_UNION,
    NODE_TYPE_BRACKET,
    NODE_TYPE_CAST,
    NODE_TYPE_BLANK

};

enum {
    PARSE_ALL_OK,
    PARSE_GENERAL_ERROR
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

    struct vector *token_vec; // Vetor de tokens da análise léxica
    struct vector *node_vec; // Vetor de nodes da análise sintática
    struct vector *node_tree_vec; // Raiz da arvore de análise

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

// Cada nó uma parte do inputfile
struct node {
    int type;
    int flags;
    struct pos pos;

    struct node_binded {
        // Ponteiro para o body node
        struct node *owner;

        // Ponteiro para a função que o nó está
        struct node *function;
    } binded;

    union {
        char cval;
        const char *sval;
        unsigned int inum;
        unsigned long lnum;
        unsigned long long llnum;
        void *any;
    };
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
void compiler_error(struct compile_process *compiler, const char *msg, ...);
void compiler_warning(struct compile_process *compiler, const char *msg, ...);


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

// parser.c
int parse(struct compile_process *process);


#endif
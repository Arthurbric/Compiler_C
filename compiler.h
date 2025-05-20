#ifndef COMPILER_H
#define COMPILER_H

#include <stdio.h> 
#include <stdlib.h>
#include <string.h>


/* BEGIN - LAB 2 ---------------------------------*/
#include <stdbool.h>

struct pos {
    int line;
    int col;
    const char* filename;
};

#define S_EQ(str, str2) \
        (str && str2 && (strcmp(str, str2) == 0))


#define NUMERIC_CASE \
    case '0':   \
    case '1':   \
    case '2':   \
    case '3':   \
    case '4':   \
    case '5':   \
    case '6':   \
    case '7':   \
    case '8':   \
    case '9'

enum {
    LEXICAL_ANALYSIS_ALL_OK ,
    LEXICAL_ANALYSIS_INPUT_ERROR
};

#define OPERATOR_CASE \
    case '+':   \
    case '-':   \
    case '*':   \
    case '>':   \
    case '<':   \
    case '^':   \
    case '%':   \
    case '!':   \
    case '=':   \
    case '~':   \
    case '|':   \
    case '&':   \
    case '(':   \
    case '[':   \
    case ',':   \
    case '.':   \
    case '?'   

#define SYMBOL_CASE \
    case '{':   \
    case '}':   \
    case ':':   \
    case ';':   \
    case '#':   \
    case '\\':  \
    case ')':   \
    case ']'

enum {
    TOKEN_TYPE_KEYWORD ,
    TOKEN_TYPE_IDENTIFIER ,
    TOKEN_TYPE_OPERATOR ,
    TOKEN_TYPE_SYMBOL ,
    TOKEN_TYPE_NUMBER ,
    TOKEN_TYPE_STRING ,
    TOKEN_TYPE_COMMENT ,
    TOKEN_TYPE_NEWLINE
};

struct token{ 
    int type;
    int flags;

    struct pos pos; //Identificar onde o token esta no arquivo.

    union {
        char cval;
        const char *sval;
        unsigned int inum;
        unsigned long lnum;
        unsigned long long llnum;
        void* any;

    };
    // Sera 'true' se tiver um espaço entre um token e o próximo token.
    bool whitespace;

    // Retira a string que estiver dentro de parênteses. Ex: (1+2+3) resulta em 1+2+3.
    const char* between_brackets;
};

struct lex_process;

// Definicao de ponteiros para funcoes.
typedef char (*LEX_PROCESS_NEXT_CHAR) (struct lex_process* process);            
typedef char (*LEX_PROCESS_PEEK_CHAR) (struct lex_process* process);            
typedef void (*LEX_PROCESS_PUSH_CHAR) (struct lex_process* process, char c);

struct lex_process_functions {
    LEX_PROCESS_NEXT_CHAR next_char;
    LEX_PROCESS_PEEK_CHAR peek_char;
    LEX_PROCESS_PUSH_CHAR push_char;
};

struct lex_process {
    struct pos pos;
    struct vector* token_vec;
    struct compile_process* compiler;

    
    int current_expression_count; //Qts parenteses existem no momento.

    struct buffer* parentheses_buffer;
    struct lex_process_functions* function;

    void* private; //Dados privados que o lexer nao entende mas o programador entende.
};

// Funcoes do arquivo cprocess.c
char compile_process_next_char(struct lex_process* lex_process);
char compile_process_peek_char(struct lex_process* lex_process);
void compile_process_push_char(struct lex_process* lex_process, char c);

// Funcoes do arquivo lex_process.c
struct lex_process* lex_process_create(struct compile_process* compiler, struct lex_process_functions* functions, void *private);
void lex_process_free(struct lex_process* process);
void* lex_process_private(struct lex_process* process);
struct vector* lex_process_tokens(struct lex_process* process);

// Funcoes do arquivo lexer.c
int lex(struct lex_process* process);

// Funcoes do arquivo compiler.c
void compiler_error(struct compile_process* compiler, const char* msg, ...);
void compiler_warning(struct compile_process* compiler, const char* msg, ...);


/* END - LAB 2 ---------------------------------*/

enum{
    COMPILER_FILE_COMPILED_OK,
    COMPILER_FAILED_WITH_ERRORS
};

struct compile_process {
    // Como o arquivo deve ser compilado
    int flags;

    /* LAB2: Adicionar*/
    struct pos pos;

    struct compile_process_input_file{
        FILE* fp;
        const char* abs_path;
    } cfile;

    
    struct vector* token_vec;       /* LAB3: Vetor de tokens da análise léxica*/
    struct vector* node_vec;        /* LAB3: Vetor de nodes da análise sintatica*/
    struct vector* node_tree_vec;   /* LAB3: Raiz da arvore de analise*/

    FILE* ofile;
};

/* BEGIN - LAB 3 ---------------------------------*/
enum {
    NODE_TYPE_EXPRESSION,
    NODE_TYPE_EXPRESSION_PARENTHESES,
    NODE_TYPE_NUMBER,
    NODE_TYPE_IDENTIFIER ,
    NODE_TYPE_STRING ,
    NODE_TYPE_VARIABLE ,
    NODE_TYPE_VARIABLE_LIST ,
    NODE_TYPE_FUNCTION ,
    NODE_TYPE_BODY ,
    NODE_TYPE_STATEMENT_RETURN ,
    NODE_TYPE_STATEMENT_IF ,
    NODE_TYPE_STATEMENT_ELSE ,
    NODE_TYPE_STATEMENT_WHILE ,
    NODE_TYPE_STATEMENT_DO_WHILE ,
    NODE_TYPE_STATEMENT_FOR ,
    NODE_TYPE_STATEMENT_BREAK ,
    NODE_TYPE_STATEMENT_CONTINUE ,
    NODE_TYPE_STATEMENT_SWITCH ,
    NODE_TYPE_STATEMENT_CASE ,
    NODE_TYPE_STATEMENT_DEFAULT ,
    NODE_TYPE_STATEMENT_GOTO ,
    NODE_TYPE_UNARY ,
    NODE_TYPE_TENARY ,
    NODE_TYPE_LABEL ,
    NODE_TYPE_STRUCT ,
    NODE_TYPE_UNION ,
    NODE_TYPE_BRACKET ,
    NODE_TYPE_CAST ,
    NODE_TYPE_BLANK ,

};

enum {
    PARSE_ALL_OK,
    PARSE_GENERAL_ERROR
};

// Cada nó uma parte do inputfile. 
struct node {
    int type;
    int flags;
    struct pos pos;

    struct node_binded {
        // Ponteiro para o body node.
        struct node* owner;

        // Ponteiro para a funcao que o no esta.
        struct node* funtion;
    } binded;

    // Estrutura similar ao token
    union {
        char cval;
        const char *sval;
        unsigned int inum;
        unsigned long lnum;
        unsigned long long llnum;
        void* any;
    };
};

/* END - LAB 3 ---------------------------------*/

int compile_file(const char* filename, const char* out_finename, int flags);
struct compile_process* compile_process_create(const char* filename, const char* filename_out, int flags);


bool token_is_keyword(struct token* token, const char* value);

/* Build token for the input string*/
struct lex_process* tokens_build_for_string(struct compile_process* compiler, const char* str);

int parse(struct compile_process* process);             /*LAB3: Adicionar*/
#endif





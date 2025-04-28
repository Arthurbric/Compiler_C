#include "compiler.h"
#include <stdio.h>
#include <string.h>

void test_all_token_types() {
    printf("=== Teste Completo de Todos os Tokens ===\n\n");
    
    // 1. Teste de tipos básicos
    printf("1. Verificação de tipos:\n");
    for(int i = 0; i <= TOKEN_TYPE_NEWLINE; i++) {
        printf("Tipo %d: %s\n", i, token_type_to_str(i));
    }
    printf("\n");
    
    // 2. Teste de criação de tokens
    printf("2. Criação de tokens:\n");
    
    // Token numérico
    struct token num_token = {
        .type = TOKEN_TYPE_NUMBER,
        .llnum = 42,
        .pos = {1, 1, "test.c"}
    };
    printf("- "); token_print(&num_token);
    
    // Token de string
    struct token str_token = {
        .type = TOKEN_TYPE_STRING,
        .sval = "exemplo",
        .pos = {1, 10, "test.c"}
    };
    printf("- "); token_print(&str_token);
    
    // Token de operador
    struct token op_token = {
        .type = TOKEN_TYPE_OPERATOR,
        .cval = '+',
        .pos = {2, 5, "test.c"}
    };
    printf("- "); token_print(&op_token);
    
    // Token de símbolo
    struct token sym_token = {
        .type = TOKEN_TYPE_SYMBOL,
        .sval = "{",
        .pos = {3, 1, "test.c"}
    };
    printf("- "); token_print(&sym_token);
    
    // Token de identificador
    struct token id_token = {
        .type = TOKEN_TYPE_IDENTIFIER,
        .sval = "variavel",
        .pos = {4, 1, "test.c"}
    };
    printf("- "); token_print(&id_token);
    
    // Token de keyword
    struct token kw_token = {
        .type = TOKEN_TYPE_KEYWORD,
        .sval = "if",
        .pos = {5, 1, "test.c"}
    };
    printf("- "); token_print(&kw_token);
    
    // Token de comentário
    struct token com_token = {
        .type = TOKEN_TYPE_COMMENT,
        .sval = "// comentário",
        .pos = {6, 1, "test.c"}
    };
    printf("- "); token_print(&com_token);
    
    // Token de newline
    struct token nl_token = {
        .type = TOKEN_TYPE_NEWLINE,
        .pos = {7, 1, "test.c"}
    };
    printf("- "); token_print(&nl_token);
    
    printf("\n");
}

void test_token_functions() {
    printf("3. Teste de funções auxiliares:\n");
    
    struct token t = {
        .type = TOKEN_TYPE_NUMBER,
        .llnum = 100,
        .pos = {10, 5, "test.c"}
    };
    
    // Teste simplificado sem token_is_value
    printf("- Token do tipo: %s\n", token_type_to_str(t.type));
    
    // Mudando o tipo para testar
    t.type = TOKEN_TYPE_OPERATOR;
    printf("- Token do tipo: %s\n", token_type_to_str(t.type));
    
    printf("\n");
}

int main() {
    test_all_token_types();
    test_token_functions();
    return 0;
}
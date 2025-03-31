#include "compiler.h"
#include <stdio.h>

int main() {
    const char* input_file = "test.c";
    const char* output_file = NULL; // Não precisamos de saída ainda

    // 1. Inicializa o processo de compilação
    struct compile_process* compiler = compile_process_create(input_file, output_file, 0);
    if (!compiler) {
        fprintf(stderr, "Falha ao criar processo de compilação\n");
        return 1;
    }

    // 2. Configura as funções do lexer
    struct lex_process_functions lex_functions = {
        .next_char = compile_process_next_char,
        .peek_char = compile_process_peek_char,
        .push_char = compile_process_push_char
    };

    // 3. Cria o processo léxico
    struct lex_process* lexer = lex_process_create(compiler, &lex_functions, NULL);
    if (!lexer) {
        fprintf(stderr, "Falha ao criar lexer\n");
        return 1;
    }

    // 4. Executa a análise léxica
    printf("Iniciando análise léxica...\n");
    if (lex(lexer) != LEXICAL_ANALYSIS_ALL_OK) {
        fprintf(stderr, "Falha na análise léxica\n");
        return 1;
    }

    // 5. Imprime os tokens gerados (adicione esta função ao lexer.c)
    printf("Tokens gerados:\n");
    Vector* tokens = lex_process_tokens(lexer);
    for (int i = 0; i < vector_size(tokens); i++) {
        struct token* token = vector_peek_at(tokens, i);
        print_token(token); // Você precisará implementar esta função
    }

    // 6. Limpeza
    lex_process_free(lexer);
    printf("Análise concluída com sucesso!\n");
    return 0;
}
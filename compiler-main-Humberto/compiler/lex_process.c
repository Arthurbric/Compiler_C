#include "compiler.h"
#include "helpers/vector.h"

/**
 * Cria um novo processo léxico
 * @param compiler Ponteiro para o processo de compilação
 * @param functions Funções de manipulação de caracteres
 * @param private Dados específicos da implementação
 * @return Ponteiro para o processo léxico criado ou NULL em caso de erro
 */
struct lex_process* lex_process_create(
    struct compile_process* compiler,
    struct lex_process_functions* functions,
    void* private)
{
    struct lex_process* process = calloc(1, sizeof(struct lex_process));
    if (!process) {
        return NULL;
    }

    process->functions = functions;
    process->token_vec = vector_create();
    if (!process->token_vec) {
        free(process);
        return NULL;
    }
    
    process->compiler = compiler;
    process->private = private;
    process->pos.line = 1;
    process->pos.col = 1;
    process->pos.filename = compiler->cfile.abs_path;

    return process;
}

/**
 * Libera a memória alocada para o processo léxico
 * @param process Ponteiro para o processo léxico
 */
void lex_process_free(struct lex_process* process)
{
    if (!process) {
        return;
    }

    if (process->token_vec) {
        vector_free(process->token_vec);
    }

    free(process);
}

/**
 * Obtém os dados privados do processo léxico
 * @param process Ponteiro para o processo léxico
 * @return Ponteiro para os dados privados
 */
void* lex_process_private(struct lex_process* process)
{
    return process->private;
}

/**
 * Obtém o vetor de tokens gerados
 * @param process Ponteiro para o processo léxico
 * @return Ponteiro para o vetor de tokens
 */
Vector* lex_process_tokens(struct lex_process* process)
{
    return process->token_vec;
}
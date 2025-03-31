#include "compiler.h"
#include <stdio.h>
#include <stdlib.h>

// Cria um processo de compilação (versão corrigida)
struct compile_process* compile_process_create(const char* filename, const char* filename_out, int flags) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        return NULL;
    }

    FILE *out_file = NULL;
    if (filename_out) {
        out_file = fopen(filename_out, "w");
        if (!out_file) {
            fclose(file);  // Fecha o arquivo de entrada se o de saída falhar
            return NULL;
        }
    }

    struct compile_process *process = calloc(1, sizeof(struct compile_process));
    process->flags = flags;
    process->cfile.fp = file;
    process->cfile.abs_path = filename;  // Armazena o caminho absoluto
    process->ofile = out_file;
    
    // Inicializa a posição (LAB 2)
    process->pos.line = 1;
    process->pos.col = 1;
    process->pos.filename = filename;

    return process;
}

// --- Funções para o Lexer (LAB 2) ---

// Lê o próximo caractere e atualiza a posição
char compile_process_next_char(struct lex_process *lex_process) {
    struct compile_process *compiler = lex_process->compiler;
    char c = getc(compiler->cfile.fp);
    
    if (c == '\n') {
        compiler->pos.line++;
        compiler->pos.col = 1;
    } else {
        compiler->pos.col++;
    }
    
    return c;
}

// Espia o próximo caractere sem consumi-lo
char compile_process_peek_char(struct lex_process *lex_process) {
    struct compile_process *compiler = lex_process->compiler;
    char c = getc(compiler->cfile.fp);
    ungetc(c, compiler->cfile.fp);  // Devolve o caractere ao buffer
    return c;
}

// Devolve um caractere ao buffer (para reanálise)
void compile_process_push_char(struct lex_process *lex_process, char c) {
    struct compile_process *compiler = lex_process->compiler;
    ungetc(c, compiler->cfile.fp);
    
    // Atualiza a posição (se não for quebra de linha)
    if (c != '\n') {
        compiler->pos.col--;
    }
}

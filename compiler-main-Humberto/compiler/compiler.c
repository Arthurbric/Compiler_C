#include "compiler.h"
#include <stdarg.h>

void compiler_error(struct compile_process* compiler, const char* msg, ...) {
    va_list args;
    va_start(args, msg);
    fprintf(stderr, "Erro em %s:%d:%d: ",
            compiler->cfile.abs_path,
            compiler->pos.line,
            compiler->pos.col);
    vfprintf(stderr, msg, args);
    fprintf(stderr, "\n");
    va_end(args);
}

struct lex_process_functions compiler_lex_functions = {
    .next_char = compile_process_next_char,
    .peek_char = compile_process_peek_char,
    .push_char = compile_process_push_char
};

int compile_file(const char* filename, const char* out_filename, int flags) {
    struct compile_process* process = compile_process_create(filename, out_filename, flags);
    if (!process) {
        return COMPILER_FAILED_WITH_ERRORS;
    }

    struct lex_process* lex_process = lex_process_create(process, &compiler_lex_functions, NULL);
    if (!lex_process) {
        return COMPILER_FAILED_WITH_ERRORS;
    }

    if (lex(lex_process) != LEXICAL_ANALYSIS_ALL_OK) {
        lex_process_free(lex_process);
        return COMPILER_FAILED_WITH_ERRORS;
    }

    lex_process_free(lex_process);
    return COMPILER_FILE_COMPILED_OK;
}
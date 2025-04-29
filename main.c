#include <stdio.h>
#include "helpers/vector.h"
#include "helpers/buffer.h"
#include "compiler.h"

int main(int argc, char *argv[], char *envp[]) {

    printf("Compiladores - Turma B - Grupo 2\n\n");

    int res = compile_file("./tests/test.c", "./tests/test.o", 0);
    if (res == COMPILER_FILE_COMPILED_OK) {
        printf("Compilação OK!\n");
    } else if (res == COMPILER_FAILED_WITH_ERRORS) {
        printf("Falha na Compilação!\n");
    } else {
        printf("Erro desconhecido!\n");
    }

    return 0;
}
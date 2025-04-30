#include <stdio.h>
#include "helpers/vector.h"
#include "helpers/buffer.h"
#include "compiler.h"

int main(int argc, char *argv[], char *envp[]) {
    const char vec_arquivos[3][50] = {
        "./tests/test1.c",
        "./tests/test2.c",
        "./tests/test3.c"
    };

    const char vec_arquivos_saida[3][50] = {
        "./tests/test1.o",
        "./tests/test2.o",
        "./tests/test3.o"
    };
    
    printf("Compiladores - Turma B - Grupo 2\n\n");

    for (size_t i = 0; i < 3; i++) {

        int res = compile_file(vec_arquivos[i], vec_arquivos_saida[i], 0);
        if (res == COMPILER_FILE_COMPILED_OK) {
            printf("Compilação OK!\n");
        } else if (res == COMPILER_FAILED_WITH_ERRORS) {
            printf("Falha na Compilação!\n");
        } else {
            printf("Erro desconhecido!\n");
        }

        printf("\n\n");
    }
    

    return 0;
}
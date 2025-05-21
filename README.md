# ✨ Implementação da Análise Léxica (Lab 4)

## 🏫 Turma B

## 👥 Integrantes do Grupo
- Humberto Nascimento Santos Junior
- Arthur Arash Briceno Heidari
- Pedro Garcia Vilanova
- Guilherme Cláudio Marques

---

## 📖 Descrição

Este laboratório tem como objetivo a **implementação da análise sintática** para um compilador. Foram criados **os nodos** e funções para manipulá-los, garantindo o correto funcionamento da parser.

---

## 🔑 Nodos implementados
Os seguintes nodos foram implementados:

-    `NODE_TYPE_EXPRESSION`
-    `NODE_TYPE_EXPRESSION_PARENTHESES`
-    `NODE_TYPE_NUMBER`
-    `NODE_TYPE_IDENTIFIER`
-    `NODE_TYPE_STRING`
-    `NODE_TYPE_VARIABLE`
-   `NODE_TYPE_VARIABLE_LIST`
-    `NODE_TYPE_FUNCTION`
-    `NODE_TYPE_BODY`
-    `NODE_TYPE_STATEMENT_RETURN`
-    `NODE_TYPE_STATEMENT_IF`
-    `NODE_TYPE_STATEMENT_ELSE`
-    `NODE_TYPE_STATEMENT_WHILE`
-    `NODE_TYPE_STATEMENT_DO_WHILE`
-    `NODE_TYPE_STATEMENT_FOR`
-    `NODE_TYPE_STATEMENT_BREAK`
-    `NODE_TYPE_STATEMENT_CONTINUE`
-    `NODE_TYPE_STATEMENT_SWITCH`
-   `NODE_TYPE_STATEMENT_CASE`
-    `NODE_TYPE_STATEMENT_DEFAULT`
-    `NODE_TYPE_STATEMENT_GOTO`
-    `NODE_TYPE_UNARY`
-    `NODE_TYPE_TENARY`
-   `NODE_TYPE_LABEL`
-    `NODE_TYPE_STRUCT`
-    `NODE_TYPE_UNION`
-    `NODE_TYPE_BRACKET`
-    `NODE_TYPE_CAST`
-    `NODE_TYPE_BLANK`

---

## Impressão da árvore do parser
Foi implementada uma função para exibir os nodos em forma de árvore baseado na precedência.

## 🛠️ Makefile

O **Makefile** foi configurado para simplificar os comandos de execução e limpeza:

- **`./main`**: Executa os testes nos arquivos test*.c.
- **`make`**: Compila todos os códigos-fonte.
- **`make clean`**: Remove arquivos temporários gerados durante a compilação.

---
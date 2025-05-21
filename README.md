# ✨ Implementação da Análise Léxica (Lab 4)

## 🏫 Turma B

## 👥 Integrantes do Grupo
- Humberto Nascimento Santos Junior
- Arthur Arash Briceno Heidari
- Pedro Garcia Vilanova
- Guilherme Cláudio Marques

---

## 📖 Descrição

Este laboratório tem como objetivo a **implemetação da estrutura do nodo e a definição de precedência dos operadores** para um compilador. Foram criados **os nodos** e funções para manipulá-los, garantindo o correto funcionamento do parser. Também foram criados **as histórias** e funções que usam ela, possibilitando a transferência de comandos entre funções recursivas.

---

## 🔑 Funções implementadas
As seguintos funções relativas aos nodos foram implementadas:

- `node_set_vector`
- `node_push`
- `node_peek_or_null`
- `node_peek`
- `node_pop`
- `node_peek_expressionable_or_null`
- `node_is_expressionable`
- `make_exp_node`
- `node_create`  

Outras estruturas relativas ao funcionamento dos nodos:

- `struct history`
- `struct expressionable_op_precedence_group`

---

## Impressão da árvore do parser
Foi implementada uma função para exibir os nodos em forma de árvore baseado na precedência.

## 🛠️ Makefile

O **Makefile** foi configurado para simplificar os comandos de execução e limpeza:

- **`./main`**: Executa os testes nos arquivos test*.c.
- **`make`**: Compila todos os códigos-fonte.
- **`make clean`**: Remove arquivos temporários gerados durante a compilação.

---
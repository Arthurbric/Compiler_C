#include "compiler.h"

// Formato: {operator1, operator2, operator3, ..., NULL}
struct expressionable_op_precedence_group op_precedence[TOTAL_OPERADOR_GROUPS] = {
    {.operators = {"++", "--", "()", "[]", "(", "[", ".", "->", NULL}, .associativity = ASSOCIATIVITY_LEFT_TO_RIGTH},
    {.operators = {"*", "/", "%", NULL}, .associativity = ASSOCIATIVITY_LEFT_TO_RIGTH},
    {.operators = {"+", "-", NULL}, .associativity = ASSOCIATIVITY_LEFT_TO_RIGTH},
    {.operators = {"<<", ">>", NULL}, .associativity = ASSOCIATIVITY_LEFT_TO_RIGTH},
    {.operators = {"<", "<=", ">", ">=", NULL}, .associativity = ASSOCIATIVITY_LEFT_TO_RIGTH},
    {.operators = {"==", "!=", NULL}, .associativity = ASSOCIATIVITY_LEFT_TO_RIGTH},
    {.operators = {"&", NULL}, .associativity = ASSOCIATIVITY_LEFT_TO_RIGTH},
    {.operators = {"^", NULL}, .associativity = ASSOCIATIVITY_LEFT_TO_RIGTH},
    {.operators = {"|", NULL}, .associativity = ASSOCIATIVITY_LEFT_TO_RIGTH},
    {.operators = {"&&", NULL}, .associativity = ASSOCIATIVITY_LEFT_TO_RIGTH},
    {.operators = {"||", NULL}, .associativity = ASSOCIATIVITY_LEFT_TO_RIGTH},
    {.operators = {"?", ":", NULL}, .associativity = ASSOCIATIVITY_RIGHT_TO_LEFT},                                                          // D->E
    {.operators = {"=", "+=", "-=", "*=", "/=", "%=", "<<=", ">>=", "&=", "^=", "|=", NULL}, .associativity = ASSOCIATIVITY_RIGHT_TO_LEFT}, // D->E
    {.operators = {",", NULL}, .associativity = ASSOCIATIVITY_LEFT_TO_RIGTH}};
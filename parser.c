#include "compiler.h"
#include "helpers/vector.h"
#include <assert.h>

static struct compile_process *current_process;
static struct token *parser_last_token;

// Estrutura para passar comandos atraves de funcoes recursivas.
struct history
{
    int flags;
};

void parse_expressionable(struct history *history);
int parse_expressionable_single(struct history *history);

static bool parser_left_op_has_priority(const char *op_left, const char *op_right);    // LAB4
extern struct expressionable_op_precedence_group op_precedence[TOTAL_OPERADOR_GROUPS]; // LAB4

struct history *history_begin(int flags)
{
    struct history *history = calloc(1, sizeof(struct history));
    history->flags = flags;

    return history;
}

struct history *history_down(struct history *history, int flags)
{
    struct history *new_history = calloc(1, sizeof(struct history));
    memcpy(new_history, history, sizeof(struct history));
    new_history->flags = flags;

    return new_history;
}

static void parser_ignore_nl_or_comment(struct token *token)
{

    while (token && discart_token(token))
    {
        // Pula o token que deve ser descartado no vetor.
        vector_peek(current_process->token_vec);
        // Pega o proximo token para ver se ele tambem sera descartado.
        token = vector_peek_no_increment(current_process->token_vec);
    }
}

static struct token *token_next()
{
    struct token *next_token = vector_peek_no_increment(current_process->token_vec);
    parser_ignore_nl_or_comment(next_token);
    current_process->pos = next_token->pos; // Atualiza a posicao do arquivo de compilacao.
    parser_last_token = next_token;
    return vector_peek(current_process->token_vec);
}

static struct token *token_peek_next()
{
    struct token *next_token = vector_peek_no_increment(current_process->token_vec);
    parser_ignore_nl_or_comment(next_token);

    return vector_peek_no_increment(current_process->token_vec);
}

void parse_single_token_to_node()
{
    struct token *token = token_next();
    struct node *node = NULL;

    switch (token->type)
    {
    case TOKEN_TYPE_NUMBER:
        node = node_create(&(struct node){.type = NODE_TYPE_NUMBER, .llnum = token->llnum});
        break;
    case TOKEN_TYPE_IDENTIFIER:
        node = node_create(&(struct node){.type = NODE_TYPE_IDENTIFIER, .sval = token->sval});
        break;
    case TOKEN_TYPE_STRING:
        node = node_create(&(struct node){.type = NODE_TYPE_STRING, .sval = token->sval});
        break;
    default:
        compiler_error(current_process, "Esse token nao pode ser convertido para node!\n");
        break;
    }
}

void parse_expressionable_for_op(struct history *history, const char *op)
{
    parse_expressionable(history);
}

static bool parser_get_precedence_for_operator(const char *op, struct expressionable_op_precedence_group **group_out)
{
    *group_out = NULL;
    for (int i = 0; i < TOTAL_OPERADOR_GROUPS; i++)
    {
        for (int j = 0; op_precedence[i].operators[j]; j++)
        {
            const char *_op = op_precedence[i].operators[j];
            if (S_EQ(op, _op))
            {
                *group_out = &op_precedence[i];
                return i;
            }
        }
    }

    return -1;
}

static bool parser_left_op_has_priority(const char *op_left, const char *op_right)
{
    struct expressionable_op_precedence_group *group_left = NULL;
    struct expressionable_op_precedence_group *group_right = NULL;

    // Se os operadores forem os mesmos, retornar falso.
    if (S_EQ(op_left, op_right))
        return false;

    int precedence_left = parser_get_precedence_for_operator(op_left, &group_left);
    int precedence_right = parser_get_precedence_for_operator(op_right, &group_right);

    // Essa funcao so trata de associatividade esquerda para direita
    if (group_left->associativity == ASSOCIATIVITY_RIGHT_TO_LEFT)
        return false;

    return precedence_left <= precedence_right;
}

void parser_node_shift_children_left(struct node *node)
{
    assert(node->type == NODE_TYPE_EXPRESSION);
    assert(node->exp.right == NODE_TYPE_EXPRESSION);
    
    const char *right_op = node->exp.right->exp.op;
    struct node *new_exp_left_node = node->exp.left;
    struct node *new_exp_right_node = node->exp.right->exp.left;
    make_exp_node(new_exp_left_node, new_exp_right_node, node->exp.op);
    
    // EX: 50*E(20+50) -> E(50*20)+50
    struct node *new_left_operand = node_pop();
    struct node *new_right_operand = node->exp.right->exp.right;
    node->exp.left = new_left_operand;
    node->exp.right = new_right_operand;
    node->exp.op = right_op;
}

void parser_reorder_expression(struct node **node_out)
{
    struct node *node = *node_out;
    // Se o node nao for do tipo expressao, finalizar.
    if (node->type != NODE_TYPE_EXPRESSION)
        return;

    // Se o node nao tiver filhos que sejam expressoes, finalizar.
    if (node->exp.left != NODE_TYPE_EXPRESSION && node->exp.right && node->exp.right != NODE_TYPE_EXPRESSION)
        return;

    if (node->exp.left != NODE_TYPE_EXPRESSION && node->exp.right && node->exp.right == NODE_TYPE_EXPRESSION)
    {
        const char *op = node->exp.right->exp.op;
        const char *right_op = node->exp.right->exp.op;

        if (parser_left_op_has_priority(node->exp.op, right_op))
        {
            // EX: 50*E(20+50) -> E(50*20)+50
            parser_node_shift_children_left(node);
            // Reordenar a arvore depois do shift ser realizado.
            parser_reorder_expression(&node->exp.left);
            parser_reorder_expression(&node->exp.right);
        }
    }
}

void parse_exp_normal(struct history *history)
{
    struct token *op_token = token_peek_next();
    char *op = op_token->sval;
    struct node *node_left = node_peek_expressionable_or_null();

    if (!node_left)
        return;

    // Retira da lista de tokens o token de operador. Ex: "123+456", retira o token "+".
    token_next();

    // Retira da lista de nodes, o ultimo node inserido.
    node_pop();

    node_left->flags |= NODE_FLAG_INSIDE_EXPRESSION;

    // Nesse momento, temos o node da esquerda e o operador. Essa funcao ira criar o node da direita.
    parse_expressionable_for_op(history_down(history, history->flags), op);
    struct node *node_right = node_pop();
    node_right->flags |= NODE_FLAG_INSIDE_EXPRESSION;

    // Cria o node de expressao, passando o node da esquerda, node da direita e operador.
    make_exp_node(node_left, node_right, op);
    struct node *exp_node = node_pop();

    // TODO: Inserir AQUI codigo para reordenador a expressao.
    parser_reorder_expression(&exp_node);

    node_push(exp_node);
}

int parse_exp(struct history *history)
{
    parse_exp_normal(history);
    return 0;
}

int parse_expressionable_single(struct history *history)
{
    struct token *token = token_peek_next();

    if (!token)
        return -1;

    history->flags |= NODE_FLAG_INSIDE_EXPRESSION;
    int res = -1;
    switch (token->type)
    {
    case TOKEN_TYPE_NUMBER:
        parse_single_token_to_node();
        res = 0;
        break;
    case TOKEN_TYPE_OPERATOR:
        parse_exp(history);
        res = 0;
        break;
    default:
        break;
    }
    return res;
}

void parse_expressionable(struct history *history)
{
    while (parse_expressionable_single(history) == 0)
    {
    }
}

int parse_next()
{
    struct token *token = token_peek_next();
    if (!token)
        return -1;

    int res = 0;
    switch (token->type)
    {
    case TOKEN_TYPE_NUMBER:
    case TOKEN_TYPE_IDENTIFIER:
    case TOKEN_TYPE_STRING:
        parse_expressionable(history_begin(0));
        break;
    default:
        break;
    }
    return 0;
}

int parse(struct compile_process *process)
{ /*LAB3: Adicionar o prototipo no compiler.h */
    current_process = process;
    parser_last_token = NULL;
    struct node *node = NULL;
    node_set_vector(process->node_vec, process->node_tree_vec);
    vector_set_peek_pointer(process->token_vec, 0);

    while (parse_next() == 0)
    {
        node = node_peek();
        vector_push(process->node_tree_vec, &node);
    }
    return PARSE_ALL_OK;
}
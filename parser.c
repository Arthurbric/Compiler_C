#include "compiler.h"
#include "helpers/vector.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

static struct compile_process *current_process;
static struct token *parser_last_token;

/* STRUCTS */
// Estrutura para passar comandos atraves de funcoes recursivas.
struct history
{
    int flags;
};
/* STRUCTS */

/* PROTÓTIPOS */
void parse_expressionable(struct history *history);
int parse_expressionable_single(struct history *history);

static bool parser_left_op_has_priority(const char *op_left, const char *op_right);
extern struct expressionable_op_precedence_group op_precedence[TOTAL_OPERADOR_GROUPS];

void parser_datatype_init_type_and_size_for_primitive(struct token *datatype_token, struct token *datatype_secondary_token, struct datatype *datatype_out);
static bool keyword_is_datatype(const char *val);
static bool is_keyword_variable_modifier(const char *val);
bool token_is_primitive_keyword(struct token *token);
void parser_get_datatype_tokens(struct token **datatype_token, struct token **datatype_secundary_token);
int parser_datatype_expected_for_type_string(const char *str);
int parser_get_random_type_index();
struct token *parser_build_random_type_name();
int parser_get_pointer_depth();
bool parser_datatype_is_secondary_allowed_for_type(const char *type);
void parser_datatype_adjust_size_for_secondary(struct datatype *datatype, struct token *datatype_secondary_token);
void parser_datatype_init_type_and_size(struct token *datatype_token, struct token *datatype_secondary_token, struct datatype *datatype_out, int pointer_depth, int expected_type);
void parser_datatype_init(struct token *datatype_token, struct token *datatype_secondary_token, struct datatype *datatype_out, int pointer_depth, int expected_type);
void parse_datatype_type(struct datatype *dtype);
void parse_datatype_modifiers(struct datatype *dtype);
void parse_datatype(struct datatype *dtype);
void parse_variable_function_or_struct_union(struct history *history);
void parse_keyword(struct history *history);
/* END PROTÓTIPOS */

static bool keyword_is_datatype(const char *val)
{
    return S_EQ(val, "void") ||
           S_EQ(val, "char") ||
           S_EQ(val, "int") ||
           S_EQ(val, "short") ||
           S_EQ(val, "float") ||
           S_EQ(val, "double") ||
           S_EQ(val, "long") ||
           S_EQ(val, "struct") ||
           S_EQ(val, "union");
}

static bool is_keyword_variable_modifier(const char *val)
{
    return S_EQ(val, "unsigned") ||
           S_EQ(val, "signed") ||
           S_EQ(val, "static") ||
           S_EQ(val, "const") ||
           S_EQ(val, "extern") ||
           S_EQ(val, "__ignore_typecheck__");
}

bool parser_datatype_is_secondary_allowed_for_type(const char *type)
{
    return S_EQ(type, "long") || S_EQ(type, "short") || S_EQ(type, "double") || S_EQ(type, "float");
}

bool token_is_primitive_keyword(struct token *token)
{
    if (!token)
        return false;
    if (token->type != TOKEN_TYPE_KEYWORD)
        return false;

    if (S_EQ(token->sval, "void"))
        return true;
    else if (S_EQ(token->sval, "char"))
        return true;
    else if (S_EQ(token->sval, "short"))
        return true;
    else if (S_EQ(token->sval, "int"))
        return true;
    else if (S_EQ(token->sval, "long"))
        return true;
    else if (S_EQ(token->sval, "float"))
        return true;
    else if (S_EQ(token->sval, "double"))
        return true;

    return false;
}

int parser_datatype_expected_for_type_string(const char *str)
{
    int type = DATATYPE_EXPECT_PRIMITIVE;

    if (S_EQ(str, "union"))
        type = DATATYPE_EXPECT_UNION;
    else if (S_EQ(str, "struct"))
        type = DATATYPE_EXPECT_STRUCT;
    return type;
}

int parser_get_random_type_index()
{
    static int x = 0;
    x++;
    return x;
}

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
    struct token *next_token = (struct token *)vector_peek_no_increment(current_process->token_vec);
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

static bool token_next_is_operator(const char *op)
{
    struct token *token = token_peek_next();
    return token_is_operator(token, op);
}

void make_variable_node(struct token *name_token, struct node *value_node)
{
    const char *name_str = name_token ? name_token->sval : NULL;
    node_create(&(struct node){.type = NODE_TYPE_VARIABLE, .var.name = name_str, .var.val = value_node});
}

void make_variable_node_and_register(struct history *history, struct token *name_token, struct node *value_node)
{
    make_variable_node(name_token, value_node);
    struct node *var_node = node_pop();
    symresolver_build_for_node(current_process, var_node);
    node_push(var_node);
}

static void expect_op(const char *op)
{
    struct token *next_token = token_next();
    if (!next_token || next_token->type != TOKEN_TYPE_OPERATOR || !S_EQ(next_token->sval, op))
    {
        compiler_error(current_process, "Esperado o operador %s, mas foi fornecido", op);
    }
}

static void expect_sym(char c)
{
    struct token *next_token = token_next();
    if (!next_token || next_token->type != TOKEN_TYPE_SYMBOL || next_token->cval != c)
    {
        compiler_error(current_process, "Esperado o simbolo %c, mas outra coisa foi fornecida", c);
    }
}

void parser_get_datatype_tokens(struct token **datatype_token, struct token **datatype_secundary_token)
{
    *datatype_token = token_next();
    struct token *next_token = token_peek_next();

    if (token_is_primitive_keyword(next_token))
    {
        *datatype_secundary_token = next_token;
        token_next();
    }
}

struct token *parser_build_random_type_name()
{
    char tmp_name[25];
    sprintf(tmp_name, "customtypename_%i", parser_get_random_type_index());
    char *sval = malloc(sizeof(tmp_name));
    strncpy(sval, tmp_name, sizeof(tmp_name));

    struct token *token = calloc(1, sizeof(struct token));
    token->type = TOKEN_TYPE_IDENTIFIER;
    token->sval = sval;

    return token;
}

int parser_get_pointer_depth()
{
    int depth = 0;
    struct token *token = NULL;

    while ((token = token_peek_next()) && (token->type == TOKEN_TYPE_OPERATOR) && S_EQ(token->sval, "*"))
    {
        depth++;
        token_next();
    }
    return depth;
}

void parse_identifier(struct history *history)
{
    assert(token_peek_next()->type == NODE_TYPE_IDENTIFIER);
    parse_single_token_to_node();
}

void parser_node_shift_children_left(struct node *node)
{
    assert(node->type == NODE_TYPE_EXPRESSION);
    assert(node->exp.right->type == NODE_TYPE_EXPRESSION);

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
    if (node->exp.left->type != NODE_TYPE_EXPRESSION &&
        node->exp.right &&
        node->exp.right->type != NODE_TYPE_EXPRESSION)
        return;

    if (node->exp.left->type != NODE_TYPE_EXPRESSION && node->exp.right && node->exp.right->type == NODE_TYPE_EXPRESSION)
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

void parse_expressionable_for_op(struct history *history, const char *op)
{
    parse_expressionable(history);
}

void parse_exp_normal(struct history *history)
{
    struct token *op_token = token_peek_next();
    char *op = (char *)op_token->sval;
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
    case TOKEN_TYPE_IDENTIFIER:
        res = 0;
        parse_identifier(history);
        break;
    case TOKEN_TYPE_OPERATOR:
        parse_exp(history);
        res = 0;
        break;
    case TOKEN_TYPE_KEYWORD:
        parse_keyword(history);
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

void parse_expressionable_root(struct history *history)
{
    parse_expressionable(history);
    struct node *result_node = node_peek_or_null();
    if (result_node)
    {
        result_node = node_pop();
        node_push(result_node);
    }
}

void parser_datatype_adjust_size_for_secondary(struct datatype *datatype, struct token *datatype_secondary_token)
{
    if (!datatype_secondary_token)
        return;

    struct datatype *secondary_data_type = calloc(1, sizeof(struct datatype));
    parser_datatype_init_type_and_size_for_primitive(datatype_secondary_token, NULL, secondary_data_type);
    datatype->size += secondary_data_type->size;
    datatype->datatype_secondary = secondary_data_type;
    datatype->flags |= DATATYPE_FLAG_IS_SECONDARY;
}

void parser_datatype_init_type_and_size_for_primitive(struct token *datatype_token, struct token *datatype_secondary_token, struct datatype *datatype_out)
{
    if (!parser_datatype_is_secondary_allowed_for_type(datatype_token->sval) && datatype_secondary_token)
        compiler_error(current_process, "Voce utilizou um datatype secundario invalido!\n");

    if (S_EQ(datatype_token->sval, "void"))
    {
        datatype_out->type = DATATYPE_VOID;
        datatype_out->size = 0;
    }
    else if (S_EQ(datatype_token->sval, "char"))
    {
        datatype_out->type = DATATYPE_CHAR;
        datatype_out->size = 1; // 1 BYTE
    }
    else if (S_EQ(datatype_token->sval, "short"))
    {
        datatype_out->type = DATATYPE_SHORT;
        datatype_out->size = 2; // 2 BYTES
    }
    else if (S_EQ(datatype_token->sval, "int"))
    {
        datatype_out->type = DATATYPE_INTEGER;
        datatype_out->size = 4; // 4 BYTES
    }
    else if (S_EQ(datatype_token->sval, "long"))
    {
        datatype_out->type = DATATYPE_LONG;
        datatype_out->size = 8; // 8 BYTES
    }
    else if (S_EQ(datatype_token->sval, "float"))
    {
        datatype_out->type = DATATYPE_FLOAT;
        datatype_out->size = 4; // 4 BYTES
    }
    else if (S_EQ(datatype_token->sval, "double"))
    {
        datatype_out->type = DATATYPE_DOUBLE;
        datatype_out->size = 8; // 8 BYTES
    }
    else
    {
        compiler_error(current_process, "BUG: Datatype invalido!\n");
    }
    parser_datatype_adjust_size_for_secondary(datatype_out, datatype_secondary_token);
}

void parser_datatype_init_type_and_size_struct_union(struct token *datatype_token, struct token *datatype_secondary_token, struct datatype *datatype_out)
{
    // Não sei fazer esta parte
}

void parser_datatype_init_type_and_size(struct token *datatype_token, struct token *datatype_secondary_token, struct datatype *datatype_out, int pointer_depth, int expected_type)
{
    if (!(expected_type == DATATYPE_EXPECT_PRIMITIVE) && datatype_secondary_token)
        compiler_error(current_process, "Voce utilizou um datatype secundario invalido!\n");

    switch (expected_type)
    {
    case DATATYPE_EXPECT_PRIMITIVE:
        parser_datatype_init_type_and_size_for_primitive(datatype_token, datatype_secondary_token, datatype_out);
        break;
    case DATATYPE_EXPECT_STRUCT:
    case DATATYPE_EXPECT_UNION:
        parser_datatype_init_type_and_size_struct_union(datatype_token, datatype_secondary_token, datatype_out);
        break;
    default:
        compiler_error(current_process, "BUG: Erro desconhecido!\n");
    }
}
void parser_datatype_init(struct token *datatype_token, struct token *datatype_secondary_token, struct datatype *datatype_out, int pointer_depth, int expected_type)
{
    parser_datatype_init_type_and_size(datatype_token, datatype_secondary_token, datatype_out, pointer_depth, expected_type);

    datatype_out->type_str = datatype_token->sval;
}

void parse_datatype_modifiers(struct datatype *dtype)
{
    struct token *token = token_peek_next();
    while (token && token->type == TOKEN_TYPE_KEYWORD)
    {
        if (!is_keyword_variable_modifier(token->sval))
            break;

        if (S_EQ(token->sval, "signed"))
            dtype->flags |= DATATYPE_FLAG_IS_SIGNED;
        else if (S_EQ(token->sval, "unsigned"))
            dtype->flags &= ~DATATYPE_FLAG_IS_SIGNED;
        else if (S_EQ(token->sval, "static"))
            dtype->flags |= DATATYPE_FLAG_IS_STATIC;
        else if (S_EQ(token->sval, "const"))
            dtype->flags |= DATATYPE_FLAG_IS_CONST;
        else if (S_EQ(token->sval, "extern"))
            dtype->flags |= DATATYPE_FLAG_IS_EXTERN;
        else if (S_EQ(token->sval, "__ignore_typecheck__"))
            dtype->flags |= DATATYPE_FLAG_IS_IGNORE_TYPE_CHECKING;

        token_next();
        token = token_peek_next();
    }
}

void parse_datatype_type(struct datatype *dtype)
{
    struct token *datatype_token = NULL;
    struct token *datatype_secundary_token = NULL;
    parser_get_datatype_tokens(&datatype_token, &datatype_secundary_token);

    int expected_type = parser_datatype_expected_for_type_string(datatype_token->sval);

    if (S_EQ(datatype_token->sval, "union") || S_EQ(datatype_token->sval, "struct"))
    {
        // Caso da struct com nome.
        if (token_peek_next()->type == TOKEN_TYPE_SYMBOL)
            dtype->flags |= DATATYPE_FLAG_IS_STRUCT_UNION_NO_NAME;
    }

    // Descobre a quantidade de ponteiros.
    int pointer_depth = parser_get_pointer_depth();

    parser_datatype_init(datatype_token, datatype_secundary_token, dtype, pointer_depth, expected_type);
}

void parse_datatype(struct datatype *dtype)
{
    memset(dtype, 0, sizeof(struct datatype));
    // Flag padrao.
    dtype->flags |= DATATYPE_FLAG_IS_SIGNED;

    parse_datatype_modifiers(dtype);
    parse_datatype_type(dtype);
    parse_datatype_modifiers(dtype);
}

void parse_array_brackets(struct history *history, struct datatype *dtype)
{
    if (!dtype->array_brackets)
    {
        dtype->array_brackets = vector_create(sizeof(struct node *));
    }
    while (token_next_is_operator("["))
    {
        expect_op("[");
        parse_expressionable_root(history);
        struct node *bracket_node = node_pop();
        node_create(&(struct node){.type = NODE_TYPE_BRACKET, .exp.left = bracket_node});
        struct node *final_bracket_node = node_pop();
        vector_push(dtype->array_brackets, &final_bracket_node);
        expect_op("]");
    }
}

void parse_variable(struct token *name_token, struct history *history)
{
    //
}

void parse_statement(struct history *history);

void parse_body(struct history *history)
{
    expect_sym('{');
    struct vector *statements = vector_create(sizeof(struct node *));
    scope_new(current_process, 0);
    while (token_peek_next()->type != TOKEN_TYPE_SYMBOL || token_peek_next()->cval != '}')
    {
        parse_statement(history);
        struct node *stmt_node = node_pop();
        vector_push(statements, &stmt_node);
    }
    expect_sym('}');
    scope_finish(current_process);
    node_create(&(struct node){.type = NODE_TYPE_BODY, .body.statements = statements});
}

void parse_struct_body(struct history *history)
{
    expect_sym('{');
    struct vector *statements = vector_create(sizeof(struct node *));
    scope_new(current_process, 0);
    while (token_peek_next()->type != TOKEN_TYPE_SYMBOL || token_peek_next()->cval != '}')
    {
        parse_variable_function_or_struct_union(history);
        struct node *stmt_node = node_pop();
        vector_push(statements, &stmt_node);
    }
    expect_sym('}');
    scope_finish(current_process);
    node_create(&(struct node){.type = NODE_TYPE_BODY, .body.statements = statements});
}

void parse_struct(struct datatype *dtype, struct token *name_token, struct history *history)
{
    node_create(&(struct node){.type = NODE_TYPE_STRUCT, ._struct.name = name_token->sval});
    struct node *struct_node = node_pop();
    symresolver_build_for_node(current_process, struct_node);

    parse_struct_body(history);
    struct_node->_struct.body_n = node_pop();

    node_push(struct_node);
    expect_sym(';');
}

void parse_function(struct datatype *dtype, struct token *name_token, struct history *history)
{
    expect_sym(')');
    parse_body(history);
    struct node *body_node = node_pop();

    node_create(&(struct node){
        .type = NODE_TYPE_FUNCTION,
        .func.name = name_token->sval,
        .func.rtype = *dtype,
        .func.body_n = body_node});

    struct node *func_node = node_pop();
    symresolver_build_for_node(current_process, func_node);
    node_push(func_node);
}

void parse_if_statement(struct history *history)
{
    parse_expressionable_root(history);
    struct node *cond_node = node_pop();

    parse_body(history);
    struct node *body_node = node_pop();

    node_create(&(struct node){.type = NODE_TYPE_STATEMENT_IF, .if_stmt.cond_node = cond_node, .if_stmt.body_node = body_node});
}

void parse_return_statement(struct history *history)
{
    struct node *exp_node = NULL;

    if (token_peek_next()->cval != ';')
    {
        parse_expressionable_root(history);
        exp_node = node_pop();
    }

    expect_sym(';');
    node_create(&(struct node){.type = NODE_TYPE_STATEMENT_RETURN, .ret_stmt.exp_node = exp_node});
}

void parse_statement(struct history *history)
{
    struct token *token = token_peek_next();
    if (token_is_keyword(token, "if"))
    {
        token_next();
        parse_if_statement(history);
        return;
    }
    if (token_is_keyword(token, "return"))
    {
        token_next();
        parse_return_statement(history);
        return;
    }

    parse_variable_function_or_struct_union(history);
}

void parse_variable_function_or_struct_union(struct history *history)
{
    struct datatype dtype;
    struct token *name_token = token_next();

    // Funções
    if (token_peek_next()->type == TOKEN_TYPE_OPERATOR && (strcmp(token_peek_next()->sval, "(") == 0))
    {
        parse_datatype(&dtype);
        token_next();
        parse_function(&dtype, name_token, history);
        return;
    }

    if (name_token->type == TOKEN_TYPE_IDENTIFIER)
    {
        if (strcmp(token_next()->sval, "=") == 0)
            make_variable_node_and_register(history, name_token, &(struct node){.any = NULL});
        parse_datatype(&dtype);
        struct node *var = node_pop();
        var->var.type = dtype;

        node_push(var);
    }

    // Structs
    if (dtype.flags & DATATYPE_FLAG_IS_STRUCT_UNION_NO_NAME)
    {
        parse_struct(&dtype, parser_build_random_type_name(), history);
        return;
    }
    else if (dtype.type == DATATYPE_STRUCT && token_peek_next()->sval == "{")
    {
        parse_struct(&dtype, name_token, history);
        return;
    }

    expect_sym(';');
}

void parse_keyword(struct history *history)
{
    struct token *token = token_peek_next();

    parse_variable_function_or_struct_union(history);
    return;
}

static int parser_get_precedence_for_operator(const char *op, struct expressionable_op_precedence_group **group_out)
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

void parse_keyword_for_global()
{
    parse_keyword(history_begin(0));
    // TODO: Essa funcao ainda nao cria o node corretamente.
    // struct node *node = node_pop();
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
    case TOKEN_TYPE_STRING:
        parse_expressionable(history_begin(0));
        break;
    case TOKEN_TYPE_KEYWORD:
    case TOKEN_TYPE_IDENTIFIER:
        parse_keyword_for_global();
        break;
    default:
        break;
    }
    return 0;
}

int parse(struct compile_process *process)
{
    current_process = process;
    parser_last_token = NULL;
    struct node *node = NULL;

    node_set_vector(process->node_vec, process->node_tree_vec);
    scope_create_root(process);
    symresolver_initialize(process);
    vector_set_peek_pointer(process->token_vec, 0);

    while (parse_next() == 0)
    {
        node = node_peek();
        if (node)
            vector_push(process->node_tree_vec, &node);
    }

    return PARSE_ALL_OK;
}

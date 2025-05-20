#include "compiler.h"

bool token_is_keyword(struct token* token, const char* value) {
    return token->type == TOKEN_TYPE_KEYWORD && S_EQ(token->sval, value);
}

bool token_is_symbol(struct token* token, char value) {
return (token->type == TOKEN_TYPE_SYMBOL) && (token->cval == value);
}

bool discart_token(struct token* token) {
    return      (token->type == TOKEN_TYPE_NEWLINE) ||
                (token->type == TOKEN_TYPE_COMMENT) ||
                token_is_symbol(token, '\\');
}
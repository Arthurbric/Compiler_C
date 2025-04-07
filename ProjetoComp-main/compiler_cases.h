#define NUMERIC_CASE \
    case '0':       \
    case '1':       \
    case '2':       \
    case '3':       \
    case '4':       \
    case '5':       \
    case '6':       \
    case '7':       \
    case '8':       \
    case '9'

#define SYMBOL_CASE \
    case '{':       \
    case '}':       \
    case ':':       \
    case ';':       \
    case '#':       \
    case ')':       \
    case ']':       \
    case '\\'

#define OPERATOR_CASE \
    case '+':        \
    case '-':        \
    case '*':        \
    case '>':        \
    case '<':        \
    case '^':        \
    case '%':        \
    case '!':        \
    case '=':        \
    case '~':        \
    case '|':        \
    case '&':        \
    case '(':        \
    case '['

#define STRING_CASE \
    case '"'

#define COMMENT_CASE \
    case '/'

// Observação: Operadores multi-caractere (>>, <<) devem ser tratados
// separadamente no código do lexer, não em switch-case
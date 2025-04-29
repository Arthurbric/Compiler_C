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
    case '[':        \
    case ',':        \
    case '?'

#define STRING_CASE \
    case '"'

#define COMMENT_CASE \
    case '/'

// Não sei se funciona, vou saber somente quando executar 😬
#define is_keyword(keyword) ( \
    strcmp(keyword, "unsigned") == 0 || \
    strcmp(keyword, "signed") == 0 || \
    strcmp(keyword, "char") == 0 || \ 
    strcmp(keyword, "short") == 0 || \
    strcmp(keyword, "int") == 0 || \
    strcmp(keyword, "long") == 0 || \
    strcmp(keyword, "float") == 0 || \
    strcmp(keyword, "double") == 0 || \
    strcmp(keyword, "void") == 0 || \
    strcmp(keyword, "struct") == 0 || \
    strcmp(keyword, "union") == 0 || \
    strcmp(keyword, "static") == 0 || \
    strcmp(keyword, "__ignore_typecheck") == 0 || \
    strcmp(keyword, "return") == 0 || \
    strcmp(keyword, "include") == 0 || \
    strcmp(keyword, "sizeof") == 0 || \
    strcmp(keyword, "if") == 0 || \
    strcmp(keyword, "else") == 0 || \
    strcmp(keyword, "while") == 0 || \
    strcmp(keyword, "for") == 0 || \
    strcmp(keyword, "do") == 0 || \
    strcmp(keyword, "break") == 0 || \
    strcmp(keyword, "continue") == 0 || \
    strcmp(keyword, "switch") == 0 || \
    strcmp(keyword, "case") == 0 || \
    strcmp(keyword, "default") == 0 || \
    strcmp(keyword, "goto") == 0 || \
    strcmp(keyword, "typedef") == 0 || \
    strcmp(keyword, "const") == 0 || \
    strcmp(keyword, "extern") == 0 || \
    strcmp(keyword, "restrict") == 0 \
) 

// Observação: Operadores multi-caractere (>>, <<) devem ser tratados
// separadamente no código do lexer, não em switch-case
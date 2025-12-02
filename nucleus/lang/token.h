#ifndef NULANG_TOKEN_H
#define NULANG_TOKEN_H

#include <nucleus/vm/math.h>
#include <nucleus/vm/types.h>

#define NULANG_NUMERIC_CHAR(c) (c >= '0' && c <= '9')
#define NULANG_IDENTIFIER_CHAR(c) \
    (NULANG_NUMERIC_CHAR(c) || (c >= 'a' && c <= 'z') || c == '_')
#define NULANG_MATCH_TOKEN(s, t, n) \
    (n == nu_strlen(t) && nu_strncmp(s, t, n) == 0)

#define NULANG_SOURCE_STRING_EQUALS(a, b) \
    (a.n == b.n && nu_strncmp(a.p, b.p, a.n) == 0)

typedef struct
{
    nu_u16_t line;
    nu_u16_t column;
    nu_u32_t index;
} nulang_location_t;

#define NULANG_FOREACH_TOKEN(TOKEN) \
    TOKEN(IDENTIFIER)               \
    TOKEN(PRIMITIVE)                \
    TOKEN(LITERAL)                  \
    TOKEN(IMPORT)                   \
    TOKEN(FROM)                     \
    TOKEN(EXPORT)                   \
    TOKEN(COMMENT)                  \
    TOKEN(SPACE)                    \
    TOKEN(LPAREN)                   \
    TOKEN(RPAREN)                   \
    TOKEN(LBRACKET)                 \
    TOKEN(RBRACKET)                 \
    TOKEN(COMMA)                    \
    TOKEN(DOT)                      \
    TOKEN(COLON)                    \
    TOKEN(ADD)                      \
    TOKEN(SUB)                      \
    TOKEN(MUL)                      \
    TOKEN(DIV)                      \
    TOKEN(EQUAL)                    \
    TOKEN(ASSIGN)                   \
    TOKEN(NEQUAL)                   \
    TOKEN(LEQUAL)                   \
    TOKEN(GEQUAL)                   \
    TOKEN(LESS)                     \
    TOKEN(GREATER)                  \
    TOKEN(AND)                      \
    TOKEN(OR)                       \
    TOKEN(NOT)                      \
    TOKEN(LET)                      \
    TOKEN(IF)                       \
    TOKEN(THEN)                     \
    TOKEN(ELSE)                     \
    TOKEN(ELIF)                     \
    TOKEN(END)                      \
    TOKEN(DO)                       \
    TOKEN(FOR)                      \
    TOKEN(IN)                       \
    TOKEN(WHILE)                    \
    TOKEN(FUNCTION)                 \
    TOKEN(BREAK)                    \
    TOKEN(CONTINUE)                 \
    TOKEN(RETURN)                   \
    TOKEN(INSERT)                   \
    TOKEN(DELETE)                   \
    TOKEN(SINGLETON)                \
    TOKEN(ITER)                     \
    TOKEN(NEXT)                     \
    TOKEN(EOF)
#define NULANG_GENERATE_TOKEN(TOKEN)      TOKEN_##TOKEN,
#define NULANG_GENERATE_TOKEN_NAME(TOKEN) #TOKEN,

typedef enum
{
    NULANG_FOREACH_TOKEN(NULANG_GENERATE_TOKEN)
} nulang__token_type_t;

static const nu_char_t *NULANG_TOKEN_NAMES[]
    = { NULANG_FOREACH_TOKEN(NULANG_GENERATE_TOKEN_NAME) };

typedef struct
{
    nulang_location_t start;
    nulang_location_t stop;
} nulang__span_t;

typedef struct
{
    const nu_char_t *p;
    nu_size_t        n;
} nulang__string_t;

typedef union
{
    nu_bool_t        b;
    nu_fix_t         f;
    nu_u32_t         i;
    nulang__string_t s;
} nulang__lit_value_t;

typedef enum
{
    LITERAL_NIL,
    LITERAL_BOOL,
    LITERAL_STRING,
    LITERAL_INT,
    LITERAL_FIX
} nulang__lit_type_t;

typedef struct
{
    nulang__lit_value_t value;
    nulang__lit_type_t  type;
} nulang__lit_t;

typedef union
{
    nu_primitive_t   primitive;
    nulang__lit_t    literal;
    nulang__string_t identifier;
} nulang__token_value_t;

typedef struct
{
    nulang__token_type_t  type;
    nulang__span_t        span;
    nulang__token_value_t value;
} nulang__token_t;

#define NULANG_TOKEN_EOF                     \
    (nulang__token_t)                        \
    {                                        \
        TOKEN_EOF, { { 0, 0 }, { 0, 0 } }, 0 \
    }
#define NULANG_TOKEN_SINGLE(t, loc) \
    (nulang__token_t)               \
    {                               \
        (t), { (loc), (loc) }, 0    \
    }
#define NULANG_TOKEN_DOUBLE(t, loc)                     \
    (nulang__token_t)                                   \
    {                                                   \
        (t), { (loc), { loc.line, loc.column + 1 } }, 0 \
    }

#endif

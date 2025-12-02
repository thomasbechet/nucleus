#ifndef NULANG_LEXER_H
#define NULANG_LEXER_H

#include <nucleus/lang/error.h>
#include <nucleus/lang/report.h>
#include <nucleus/lang/token.h>
#include <nucleus/vm/string.h>
#include <nucleus/vm/math.h>
#include <nucleus/vm/types.h>

#define NULANG_LEXER_MAX_PEEKS 5

typedef struct
{
    nulang__error_data_t *error;
    /* source streaming info */
    const nu_char_t  *source;
    const nu_char_t  *ptr;
    nulang_location_t next_loc;
    /* character peek */
    nu_char_t         peek_char;
    nulang_location_t peek_loc;
    nu_bool_t         has_peek;
    /* token peeks */
    nulang__token_t peeks[NULANG_LEXER_MAX_PEEKS];
    nu_size_t       peek_count;
} nulang__lexer_t;

#ifdef NU_IMPL

static void
nulang__lexer_init (const nu_char_t      *source,
                    nulang__error_data_t *error,
                    nulang__lexer_t      *lexer)
{
    lexer->error = error;

    lexer->source          = source;
    lexer->ptr             = source;
    lexer->next_loc.line   = 1;
    lexer->next_loc.column = 1;
    lexer->next_loc.index  = 0;

    lexer->has_peek = NU_FALSE;

    lexer->peek_count = 0;
}

static nu_bool_t
nulang__source_stream_next (nulang__lexer_t   *lexer,
                            nu_char_t         *c,
                            nulang_location_t *loc)
{
    if (!lexer->ptr)
    {
        /* end reached */
        return NU_FALSE;
    }
    *loc = lexer->next_loc;
    lexer->next_loc.index++;
    *c = *(lexer->ptr++);
    if (*c == '\0')
    {
        lexer->ptr = NU_NULL;
        return NU_FALSE;
    }
    if (*c == '\n')
    {
        *c = ' ';
        lexer->next_loc.line += 1;
        lexer->next_loc.column = 1;
    }
    else
    {
        lexer->next_loc.column += 1;
    }
    return NU_TRUE;
}

static nu_bool_t
nulang__lexer_next_char (nulang__lexer_t   *lexer,
                         nu_char_t         *c,
                         nulang_location_t *loc)
{
    if (lexer->has_peek)
    {
        lexer->has_peek = NU_FALSE;
        *c              = lexer->peek_char;
        *loc            = lexer->peek_loc;
        return NU_TRUE;
    }
    else
    {
        return nulang__source_stream_next(lexer, c, loc);
    }
}
static nu_bool_t
nulang__lexer_peek_char (nulang__lexer_t   *lexer,
                         nu_char_t         *c,
                         nulang_location_t *loc)
{
    if (lexer->has_peek)
    {
        *c   = lexer->peek_char;
        *loc = lexer->peek_loc;
        return NU_TRUE;
    }
    else
    {
        lexer->has_peek = nulang__source_stream_next(
            lexer, &lexer->peek_char, &lexer->peek_loc);
        *c   = lexer->peek_char;
        *loc = lexer->peek_loc;
        return lexer->has_peek;
    }
}

static void
nulang__consume_char (nulang__lexer_t *lexer)
{
    nu_char_t         c;
    nulang_location_t dloc;
    nulang__lexer_next_char(lexer, &c, &dloc);
}
static nulang__error_t
nulang__consume_single_char_token (nulang__lexer_t     *lexer,
                                   nulang__token_type_t type,
                                   nulang_location_t    loc,
                                   nulang__token_t     *token)
{
    nulang__consume_char(lexer);
    *token = NULANG_TOKEN_SINGLE(type, loc);
    return NULANG_ERROR_NONE;
}
static void
nulang__consume_comment (nulang__lexer_t *lexer)
{
    nulang_location_t start_loc, loc;
    nu_char_t         c;
    nulang__lexer_peek_char(lexer, &c, &start_loc);
    while (nulang__lexer_peek_char(lexer, &c, &loc))
    {
        if (loc.line != start_loc.line)
        {
            break;
        }
        nulang__consume_char(lexer);
    }
}
static void
nulang__consume_spaces (nulang__lexer_t *lexer)
{
    nu_char_t         c;
    nulang_location_t loc;
    while (nulang__lexer_peek_char(lexer, &c, &loc))
    {
        if (c != ' ')
        {
            break;
        }
        nulang__consume_char(lexer);
    }
}

static nulang__error_t
nulang__consume_string (nulang__lexer_t *lexer, nulang__token_t *token)
{
    nulang_location_t start_loc, stop_loc, loc;
    nu_char_t         c;
    const nu_char_t  *s;
    nu_size_t         n;

    nulang__lexer_next_char(lexer, &c, &start_loc);
    n        = 0;
    s        = lexer->ptr;
    stop_loc = start_loc;
    while (nulang__lexer_peek_char(lexer, &c, &loc))
    {
        nulang__consume_char(lexer);
        if (c == '\'')
        {
            stop_loc                       = loc;
            token->span.start              = start_loc;
            token->span.stop               = stop_loc;
            token->type                    = TOKEN_LITERAL;
            token->value.literal.type      = LITERAL_STRING;
            token->value.literal.value.s.p = s;
            token->value.literal.value.s.n = n;
            return NULANG_ERROR_NONE;
        }
        n++;
    }
    lexer->error->span.start = start_loc;
    lexer->error->span.stop  = stop_loc;
    return NULANG_ERROR_UNTERMINATED_STRING;
}
static nulang__error_t
nulang__consume_number (nulang__lexer_t *lexer, nulang__token_t *token)
{
    nulang_location_t start_loc, stop_loc, loc;
    nu_char_t         c;
    const nu_char_t  *s;
    nu_size_t         n;
    nu_bool_t         has_dot = NU_FALSE;

    s = lexer->ptr - 1; /* small hack */
    n = 0;
    nulang__lexer_peek_char(lexer, &c, &start_loc);
    stop_loc = start_loc;
    while (nulang__lexer_peek_char(lexer, &c, &loc))
    {
        if (NULANG_NUMERIC_CHAR(c) || c == '.')
        {
            stop_loc = loc;
            if (c == '.')
            {
                if (has_dot)
                {
                    lexer->error->span.start = start_loc;
                    lexer->error->span.stop  = stop_loc;
                    return NULANG_ERROR_ILLEGAL_CHARACTER;
                }
                has_dot = NU_TRUE;
            }
            n++;
            nulang__consume_char(lexer);
        }
        else if (NULANG_IDENTIFIER_CHAR(c))
        {
            lexer->error->span.start = start_loc;
            lexer->error->span.stop  = stop_loc;
            return NULANG_ERROR_ILLEGAL_CHARACTER;
        }
        else
        {
            break;
        }
    }
    token->span.start = start_loc;
    token->span.stop  = stop_loc;
    if (has_dot)
    {
        nu_fix_t v;
        if (nu_fparse(s, n, &v) != NU_ERROR_NONE)
        {
            lexer->error->span.start = start_loc;
            lexer->error->span.stop  = stop_loc;
            return NULANG_ERROR_PARSE_FIXED_POINT;
        }
        token->type                  = TOKEN_LITERAL;
        token->value.literal.type    = LITERAL_FIX;
        token->value.literal.value.f = v;
    }
    else
    {
        nu_i32_t v;
        if (nu_iparse(s, n, &v) != NU_ERROR_NONE)
        {
            lexer->error->span.start = start_loc;
            lexer->error->span.stop  = stop_loc;
            return NULANG_ERROR_ILLEGAL_CHARACTER;
        }
        token->type                  = TOKEN_LITERAL;
        token->value.literal.type    = LITERAL_INT;
        token->value.literal.value.i = v;
    }
    return NULANG_ERROR_NONE;
}
static nulang__error_t
nulang__consume_identifier (nulang__lexer_t *lexer, nulang__token_t *token)
{
    nulang_location_t start_loc, stop_loc, loc;
    nu_char_t         c;
    const nu_char_t  *s;
    nu_size_t         n;

    s = lexer->ptr - 1; /* small hack */
    n = 0;
    nulang__lexer_peek_char(lexer, &c, &start_loc);
    stop_loc = start_loc;
    while (nulang__lexer_peek_char(lexer, &c, &loc))
    {
        if (!NULANG_IDENTIFIER_CHAR(c))
        {
            break;
        }
        stop_loc = loc;
        n++;
        nulang__consume_char(lexer);
    }
    token->span.start = start_loc;
    token->span.stop  = stop_loc;

    /* check keyword */
    token->type = TOKEN_EOF;
    if (NULANG_MATCH_TOKEN(s, "import", n))
    {
        token->type = TOKEN_IMPORT;
    }
    else if (NULANG_MATCH_TOKEN(s, "from", n))
    {
        token->type = TOKEN_FROM;
    }
    else if (NULANG_MATCH_TOKEN(s, "export", n))
    {
        token->type = TOKEN_EXPORT;
    }
    else if (NULANG_MATCH_TOKEN("and", s, n))
    {
        token->type = TOKEN_AND;
    }
    else if (NULANG_MATCH_TOKEN(s, "or", n))
    {
        token->type = TOKEN_OR;
    }
    else if (NULANG_MATCH_TOKEN(s, "not", n))
    {
        token->type = TOKEN_NOT;
    }
    else if (NULANG_MATCH_TOKEN(s, "let", n))
    {
        token->type = TOKEN_LET;
    }
    else if (NULANG_MATCH_TOKEN(s, "if", n))
    {
        token->type = TOKEN_IF;
    }
    else if (NULANG_MATCH_TOKEN(s, "then", n))
    {
        token->type = TOKEN_THEN;
    }
    else if (NULANG_MATCH_TOKEN(s, "else", n))
    {
        token->type = TOKEN_ELSE;
    }
    else if (NULANG_MATCH_TOKEN(s, "elif", n))
    {
        token->type = TOKEN_ELIF;
    }
    else if (NULANG_MATCH_TOKEN(s, "end", n))
    {
        token->type = TOKEN_END;
    }
    else if (NULANG_MATCH_TOKEN(s, "do", n))
    {
        token->type = TOKEN_DO;
    }
    else if (NULANG_MATCH_TOKEN(s, "for", n))
    {
        token->type = TOKEN_FOR;
    }
    else if (NULANG_MATCH_TOKEN(s, "in", n))
    {
        token->type = TOKEN_IN;
    }
    else if (NULANG_MATCH_TOKEN(s, "while", n))
    {
        token->type = TOKEN_WHILE;
    }
    else if (NULANG_MATCH_TOKEN(s, "function", n))
    {
        token->type = TOKEN_FUNCTION;
    }
    else if (NULANG_MATCH_TOKEN(s, "break", n))
    {
        token->type = TOKEN_BREAK;
    }
    else if (NULANG_MATCH_TOKEN(s, "continue", n))
    {
        token->type = TOKEN_CONTINUE;
    }
    else if (NULANG_MATCH_TOKEN(s, "return", n))
    {
        token->type = TOKEN_RETURN;
    }
    else if (NULANG_MATCH_TOKEN(s, "insert", n))
    {
        token->type = TOKEN_INSERT;
    }
    else if (NULANG_MATCH_TOKEN(s, "delete", n))
    {
        token->type = TOKEN_DELETE;
    }
    else if (NULANG_MATCH_TOKEN(s, "singleton", n))
    {
        token->type = TOKEN_SINGLETON;
    }
    else if (NULANG_MATCH_TOKEN(s, "iter", n))
    {
        token->type = TOKEN_ITER;
    }
    else if (NULANG_MATCH_TOKEN(s, "next", n))
    {
        token->type = TOKEN_NEXT;
    }
    if (token->type != TOKEN_EOF)
    {
        return NULANG_ERROR_NONE;
    }

    /* check if token is a literal */
    if (NULANG_MATCH_TOKEN(s, "true", n))
    {
        token->type                  = TOKEN_LITERAL;
        token->value.literal.value.b = NU_TRUE;
        token->value.literal.type    = LITERAL_BOOL;
    }
    else if (NULANG_MATCH_TOKEN(s, "false", n))
    {
        token->type                  = TOKEN_LITERAL;
        token->value.literal.value.b = NU_FALSE;
        token->value.literal.type    = LITERAL_BOOL;
    }
    else if (NULANG_MATCH_TOKEN(s, "nil", n))
    {
        token->type               = TOKEN_LITERAL;
        token->value.literal.type = LITERAL_NIL;
    }
    if (token->type == TOKEN_LITERAL)
    {
        return NULANG_ERROR_NONE;
    }

    /* check primitive */
    if (NULANG_MATCH_TOKEN(s, "int", n))
    {
        token->type            = TOKEN_PRIMITIVE;
        token->value.primitive = NU_PRIMITIVE_INT;
    }
    else if (NULANG_MATCH_TOKEN(s, "fix", n))
    {
        token->type            = TOKEN_PRIMITIVE;
        token->value.primitive = NU_PRIMITIVE_FIX;
    }
    else if (NULANG_MATCH_TOKEN(s, "str", n))
    {
        token->type = TOKEN_PRIMITIVE;
        /* TODO */
        token->value.primitive = NU_PRIMITIVE_INT;
    }
    else if (NULANG_MATCH_TOKEN(s, "vec2", n))
    {
        token->type            = TOKEN_PRIMITIVE;
        token->value.primitive = NU_PRIMITIVE_VEC2;
    }
    else if (NULANG_MATCH_TOKEN(s, "vec3", n))
    {
        token->type            = TOKEN_PRIMITIVE;
        token->value.primitive = NU_PRIMITIVE_VEC3;
    }
    else if (NULANG_MATCH_TOKEN(s, "vec4", n))
    {
        token->type            = TOKEN_PRIMITIVE;
        token->value.primitive = NU_PRIMITIVE_VEC4;
    }
    else if (NULANG_MATCH_TOKEN(s, "ivec2", n))
    {
        token->type            = TOKEN_PRIMITIVE;
        token->value.primitive = NU_PRIMITIVE_IVEC2;
    }
    else if (NULANG_MATCH_TOKEN(s, "ivec3", n))
    {
        token->type            = TOKEN_PRIMITIVE;
        token->value.primitive = NU_PRIMITIVE_IVEC3;
    }
    else if (NULANG_MATCH_TOKEN(s, "ivec4", n))
    {
        token->type            = TOKEN_PRIMITIVE;
        token->value.primitive = NU_PRIMITIVE_IVEC4;
    }
    if (token->type == TOKEN_PRIMITIVE)
    {
        return NULANG_ERROR_NONE;
    }

    /* check identifier */
    token->type               = TOKEN_IDENTIFIER;
    token->value.identifier.p = s;
    token->value.identifier.n = n;

    return NULANG_ERROR_NONE;
}

static nulang__error_t
nulang__lexer_parse_token (nulang__lexer_t *lexer, nulang__token_t *token)
{
    nu_char_t         c, next;
    nulang_location_t loc, loc2;
    while (nulang__lexer_peek_char(lexer, &c, &loc))
    {
        switch (c)
        {
            case '+':
                return nulang__consume_single_char_token(
                    lexer, TOKEN_ADD, loc, token);
            case '-':
                nulang__consume_char(lexer);
                if (nulang__lexer_peek_char(lexer, &next, &loc2))
                {
                    if (next == '-')
                    {
                        nulang__consume_char(lexer);
                        nulang__consume_comment(lexer);
                    }
                    else
                    {
                        *token = NULANG_TOKEN_SINGLE(TOKEN_SUB, loc);
                        return NULANG_ERROR_NONE;
                    }
                }
                else
                {
                    *token = NULANG_TOKEN_SINGLE(TOKEN_SUB, loc);
                    return NULANG_ERROR_NONE;
                }
                break;
            case '*':
                return nulang__consume_single_char_token(
                    lexer, TOKEN_MUL, loc, token);
            case '/':
                return nulang__consume_single_char_token(
                    lexer, TOKEN_DIV, loc, token);
            case '(':
                return nulang__consume_single_char_token(
                    lexer, TOKEN_LPAREN, loc, token);
            case ')':
                return nulang__consume_single_char_token(
                    lexer, TOKEN_RPAREN, loc, token);
            case ']':
                return nulang__consume_single_char_token(
                    lexer, TOKEN_LBRACKET, loc, token);
            case '[':
                return nulang__consume_single_char_token(
                    lexer, TOKEN_RBRACKET, loc, token);
            case ',':
                return nulang__consume_single_char_token(
                    lexer, TOKEN_COMMA, loc, token);
            case '.':
                return nulang__consume_single_char_token(
                    lexer, TOKEN_DOT, loc, token);
            case ':':
                return nulang__consume_single_char_token(
                    lexer, TOKEN_COLON, loc, token);
            case '=':
                nulang__consume_char(lexer);
                if (nulang__lexer_peek_char(lexer, &next, &loc2))
                {
                    if (next == '=')
                    {
                        nulang__consume_char(lexer);
                        *token = NULANG_TOKEN_DOUBLE(TOKEN_EQUAL, loc);
                        return NULANG_ERROR_NONE;
                    }
                }
                *token = NULANG_TOKEN_SINGLE(TOKEN_ASSIGN, loc);
                return NULANG_ERROR_NONE;
            case '<':
                nulang__consume_char(lexer);
                if (nulang__lexer_peek_char(lexer, &next, &loc2))
                {
                    if (next == '=')
                    {
                        nulang__consume_char(lexer);
                        *token = NULANG_TOKEN_DOUBLE(TOKEN_LEQUAL, loc);
                        return NULANG_ERROR_NONE;
                    }
                }
                *token = NULANG_TOKEN_SINGLE(TOKEN_LESS, loc);
                return NULANG_ERROR_NONE;
            case '>':
                nulang__consume_char(lexer);
                if (nulang__lexer_peek_char(lexer, &next, &loc2))
                {
                    if (next == '=')
                    {
                        nulang__consume_char(lexer);
                        *token = NULANG_TOKEN_DOUBLE(TOKEN_GEQUAL, loc);
                        return NULANG_ERROR_NONE;
                    }
                }
                *token = NULANG_TOKEN_SINGLE(TOKEN_GREATER, loc);
                return NULANG_ERROR_NONE;
            case '!':
                nulang__consume_char(lexer);
                if (nulang__lexer_peek_char(lexer, &next, &loc2))
                {
                    if (next == '=')
                    {
                        nulang__consume_char(lexer);
                        *token = NULANG_TOKEN_DOUBLE(TOKEN_NEQUAL, loc);
                        return NULANG_ERROR_NONE;
                    }
                }
                return NULANG_ERROR_ILLEGAL_CHARACTER;
            case ' ':
                nulang__consume_spaces(lexer);
                break;
            case '\'':
                return nulang__consume_string(lexer, token);
            default:
                if (NULANG_NUMERIC_CHAR(c))
                {
                    return nulang__consume_number(lexer, token);
                }
                else if (NULANG_IDENTIFIER_CHAR(c))
                {
                    return nulang__consume_identifier(lexer, token);
                }
                else
                {
                    nulang__consume_char(lexer);
                }
                break;
        }
    }
    *token = NULANG_TOKEN_EOF;
    return NULANG_ERROR_NONE;
}

static nulang__error_t
nulang__lexer_next (nulang__lexer_t *lexer, nulang__token_t *token)
{
    if (lexer->peek_count)
    {
        nu_size_t i = 0;
        *token      = lexer->peeks[0];
        for (i = 1; i < lexer->peek_count; ++i)
        {
            lexer->peeks[i - 1] = lexer->peeks[i];
        }
        lexer->peek_count--;
        return NULANG_ERROR_NONE;
    }
    else
    {
        return nulang__lexer_parse_token(lexer, token);
    }
}

static nulang__error_t
nulang__lexer_peek (nulang__lexer_t *lexer,
                    nu_size_t        lookahead,
                    nulang__token_t *token)
{
    nulang__error_t error;
    while (lexer->peek_count <= lookahead)
    {
        error = nulang__lexer_parse_token(lexer, token);
        if (error != NULANG_ERROR_NONE)
        {
            return error;
        }
        lexer->peeks[lexer->peek_count++] = *token;
    }
    *token = lexer->peeks[lookahead];
    return NULANG_ERROR_NONE;
}

#endif

#endif

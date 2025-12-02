#include <stdio.h>
#define NU_IMPL
#include <nucleus/lang.h>

static const nu_char_t *source
    = "local abc = 0\n"
      "if (abc == 2) then\n"
      "   abc = 3\n"
      "end";

int
main (void)
{
    nulang__lexer_t      lexer;
    nu_char_t            c;
    nulang_location_t    loc;
    nu_bool_t            has;
    nulang__token_t      token;
    nulang__error_t      error;
    nulang__error_data_t error_data;

    nulang__lexer_init(source, &error_data, &lexer);

    for (;;)
    {
        error = nulang__lexer_next(&lexer, &token);
        NU_ASSERT(error == NULANG_ERROR_NONE);
        if (token.type == TOKEN_EOF)
        {
            break;
        }
        printf("%s %d:%d %d:%d\n",
               NULANG_TOKEN_NAMES[token.type],
               token.span.start.line,
               token.span.start.column,
               token.span.stop.line,
               token.span.stop.column);
        if (token.type == TOKEN_LITERAL)
        {
            printf("%d\n", token.value.literal.value.i);
        }
        if (token.type == TOKEN_IDENTIFIER)
        {
            printf("%.*s\n",
                   (int)token.value.literal.value.s.n,
                   token.value.literal.value.s.p);
        }

        printf("%s\n", NULANG_AST_NAMES[AST_MODULE]);
    }

    return 0;
}

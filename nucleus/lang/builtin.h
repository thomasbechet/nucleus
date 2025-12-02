#ifndef NULANG_BUITLIN_H
#define NULANG_BUITLIN_H

#include <nucleus/lang/lexer.h>
#include <nucleus/lang/token.h>
#include <nucleus/vm/types.h>

#ifdef NU_IMPL

typedef struct
{
    nu_primitive_t   primitive;
    const nu_char_t *name;
    nu_primitive_t   return_type;
    nu_primitive_t   args[4];
} nulang__builtin_function_t;

#define NULANG_FOREACH_BUILTIN(BUILTIN) \
    BUILTIN(CONSTRUCTOR)                \
    BUILTIN(CONSTANT)                   \
    BUILTIN(FUNCTION)
#define NULANG_GENERATE_BUILTIN(BUILTIN)      BUILTIN_##BUILTIN,
#define NULANG_GENERATE_BUILTIN_NAME(BUILTIN) #BUILTIN,
typedef enum
{
    NULANG_FOREACH_BUILTIN(NULANG_GENERATE_BUILTIN)
} nulang__builtin_type_t;
static const nu_char_t *NULANG_BUILTIN_NAMES[]
    = { NULANG_FOREACH_BUILTIN(NULANG_GENERATE_BUILTIN_NAME) };

typedef union
{
    const nulang__builtin_function_t *function;
    nu_primitive_t                    constructor;
} nulang__builtin_value_t;

typedef struct
{
    nulang__builtin_type_t  type;
    nulang__builtin_value_t value;
} nulang__builtin_t;

static nulang__builtin_function_t *
nulang__find_builtin_function (nu_primitive_t   primitive,
                               nulang__string_t builtin)
{
    static nulang__builtin_function_t builtins[] = {
        { NU_PRIMITIVE_VEC2,
          "norm",
          NU_PRIMITIVE_FIX,
          { NU_PRIMITIVE_VEC2, NU_PRIMITIVE_UNKNOWN } },
        { NU_PRIMITIVE_VEC2,
          "dist",
          NU_PRIMITIVE_FIX,
          { NU_PRIMITIVE_VEC2, NU_PRIMITIVE_VEC2, NU_PRIMITIVE_UNKNOWN } },
    };

    nu_size_t i;
    for (i = 0; i < NU_ARRAY_SIZE(builtins); ++i)
    {
        if (builtins[i].primitive == primitive)
        {
            nulang__string_t s;
            s.p = builtins[i].name;
            s.n = nu_strlen(builtins[i].name);
            if (NULANG_SOURCE_STRING_EQUALS(builtin, s))
            {
                return &builtins[i];
            }
        }
    }
    return NU_NULL;
}

#endif

#endif

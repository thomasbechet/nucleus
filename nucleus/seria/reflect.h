#ifndef NU_SERIA_REFLECT_H
#define NU_SERIA_REFLECT_H

#include <nucleus/seria/api.h>

typedef struct
{
    const nu_char_t *name;
    nu_int_t         prev;
    union
    {
        struct
        {
            nu_size_t card;
            nu_bool_t is_primitive;
            union
            {
                const nu_char_t *primitive;
                nu_int_t         type;
            };
        } field;
        struct
        {
            nu_int_t  last_field;
            nu_size_t depth;
        } type;
    };
} nu__seria_reflect_entry_t;

typedef struct
{
    NU_VEC(nu__seria_reflect_entry_t) entries;
    nu_int_t  last_type;
    nu_int_t  working_type;
    nu_size_t skip_depth;
} nu__seria_reflect_t;

#endif

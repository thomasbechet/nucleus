#ifndef NU_SERIA_JSON_H
#define NU_SERIA_JSON_H

#include <nucleus/seria/api.h>

typedef struct
{
    const nu_char_t *json;
    const jsmntok_t *toks;
    nu_size_t        toks_count;
    nu_bool_t        owned;
} nu__seria_json_t;

#endif

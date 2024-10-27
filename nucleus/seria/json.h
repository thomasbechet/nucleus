#ifndef NU_SERIA_JSON_H
#define NU_SERIA_JSON_H

#include <nucleus/seria/api.h>
#include <nucleus/external/jsmn/jsmn.h>

typedef struct
{
    nu_str_t   json;
    jsmntok_t *toks;
    nu_size_t  toks_size;
    nu_size_t  toks_capa;

    const jsmntok_t *it;
    nu_size_t        it_remaining;
} nu__seria_json_t;

static jsmntok_t *nu__seria_json_parse(nu_str_t   json,
                                       nu_size_t *size,
                                       nu_size_t *count);
static nu_str_t   nu__seria_json_value(nu_str_t json, const jsmntok_t *tok);
static nu_bool_t  nu__seria_json_eq(nu_str_t         json,
                                    const jsmntok_t *tok,
                                    nu_str_t         s);
static const jsmntok_t *nu__seria_json_skip(const jsmntok_t *token);
static const jsmntok_t *nu__seria_json_object_member(nu_str_t         json,
                                                     const jsmntok_t *object,
                                                     nu_str_t         name);

#endif

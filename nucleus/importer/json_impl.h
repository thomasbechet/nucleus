#ifndef NU_IMPORTER_JSON_IMPL_H
#define NU_IMPORTER_JSON_IMPL_H

#include <nucleus/internal.h>

#define JSMN_PARENT_LINKS
#define JSMN_STRICT
#define JSMN_STATIC
#include <nucleus/external/jsmn/jsmn.h>

static jsmntok_t *
nu__json_parse (nu_str_t json, nu_size_t *size, nu_size_t *count)
{
    nu_scope_push();
    nu_size_t   cap  = 256;
    jsmntok_t  *toks = (jsmntok_t *)nu_scope_alloc(sizeof(*toks) * cap);
    jsmn_parser parser;
    jsmn_init(&parser);
    int r = jsmn_parse(&parser, (const char *)json.data, json.size, toks, cap);
    while (r == JSMN_ERROR_NOMEM)
    {
        cap *= 2;
        nu_scope_pop();
        nu_scope_push();
        toks = (jsmntok_t *)nu_scope_alloc(sizeof(*toks) * cap);
        r = jsmn_parse(&parser, (const char *)json.data, json.size, toks, cap);
    }
    NU_CHECK(r >= 1, goto cleanup0);
    *count = r;
    *size  = sizeof(*toks) * cap;
    return toks;
cleanup0:
    nu_scope_pop();
    return NU_NULL;
}
static nu_str_t
nu__json_value (nu_str_t json, const jsmntok_t *tok)
{
    return nu_str(json.data + tok->start, (nu_size_t)(tok->end - tok->start));
}
static nu_bool_t
nu__json_eq (nu_str_t json, const jsmntok_t *tok, nu_str_t s)
{
    return (tok->type == JSMN_STRING
            && nu_str_eq(s, nu__json_value(json, tok)));
}
static const jsmntok_t *
nu__json_skip (const jsmntok_t *token)
{
    int pending = 1;
    do
    {
        pending += token->size - 1;
        token++;
    } while (pending);
    return token;
}
static const jsmntok_t *
nu__json_object_member (nu_str_t json, const jsmntok_t *object, nu_str_t name)
{
    if (!object || object->type != JSMN_OBJECT)
    {
        return NULL;
    }

    int              members = object->size;
    const jsmntok_t *token   = object + 1;
    while (members && !nu__json_eq(json, token, name))
    {
        members--;
        token = nu__json_skip(token + 1);
    }
    if (!members)
    {
        return NULL;
    }
    return token + 1;
}

#endif

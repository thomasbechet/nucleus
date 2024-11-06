#ifndef NU_SERIA_JSON_IMPL_H
#define NU_SERIA_JSON_IMPL_H

#include <nucleus/internal.h>

#ifdef NU_BUILD_SERIA_JSON
#define JSMN_PARENT_LINKS
#define JSMN_STRICT
#define JSMN_STATIC
#include <nucleus/external/jsmn/jsmn.h>
#endif

static jsmntok_t *
nu__seria_json_parse (nu_str_t json, nu_size_t *size, nu_size_t *count)
{
    nu_size_t   cap  = 256;
    jsmntok_t  *toks = (jsmntok_t *)nu_alloc(sizeof(*toks) * cap);
    jsmn_parser parser;
    jsmn_init(&parser);
    int r = jsmn_parse(&parser, (const char *)json.data, json.size, toks, cap);
    while (r == JSMN_ERROR_NOMEM)
    {
        nu_size_t old_cap = cap;
        cap *= 2;
        toks = (jsmntok_t *)nu_realloc(
            toks, sizeof(*toks) * old_cap, sizeof(*toks) * cap);
        r = jsmn_parse(&parser, (const char *)json.data, json.size, toks, cap);
    }
    NU_CHECK(r >= 1, goto cleanup0);
    *count = r;
    *size  = sizeof(*toks) * cap;
    return toks;
cleanup0:
    nu_free(toks, sizeof(*toks) * cap);
    return NU_NULL;
}
static nu_str_t
nu__seria_json_value (nu_str_t json, const jsmntok_t *tok)
{
    return nu_str(json.data + tok->start, (nu_size_t)(tok->end - tok->start));
}
static nu_bool_t
nu__seria_json_eq (nu_str_t json, const jsmntok_t *tok, nu_str_t s)
{
    return (tok->type == JSMN_STRING
            && nu_str_eq(s, nu__seria_json_value(json, tok)));
}
static const jsmntok_t *
nu__seria_json_skip (const jsmntok_t *token)
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
nu__seria_json_object_member (nu_str_t         json,
                              const jsmntok_t *object,
                              nu_str_t         name)
{
    if (!object || object->type != JSMN_OBJECT)
    {
        return NULL;
    }

    int              members = object->size;
    const jsmntok_t *token   = object + 1;
    while (members && !nu__seria_json_eq(json, token, name))
    {
        members--;
        token = nu__seria_json_skip(token + 1);
    }
    if (!members)
    {
        return NULL;
    }
    return token + 1;
}

static void
nu__seria_json_open (nu__seria_json_t *j,
                     nu_seria_mode_t   mode,
                     nu_byte_t        *bytes,
                     nu_size_t         size)
{
    NU_ASSERT(mode == NU_SERIA_READ);
    j->json = nu_str((nu_byte_t *)bytes, size);
    // parse file
    j->toks = nu__seria_json_parse(j->json, &j->toks_capa, &j->toks_size);
    NU_ASSERT(j->toks);
}
static nu_size_t
nu__seria_json_close (nu__seria_json_t *j)
{
    nu_free(j->toks, sizeof(*j->toks) * j->toks_capa);
    return 0;
}

static void
nu__seria_json_parse_f32_array (nu__seria_json_t *j,
                                const jsmntok_t  *tok,
                                nu_size_t         size,
                                nu_byte_t        *data)
{
    if (tok->type != JSMN_ARRAY || tok->size != (int)size)
    {
        NU_ERROR("json token is not an array of size %d", size);
        return;
    }
    nu_f32_t *p = (nu_f32_t *)data;
    for (nu_size_t i = 0; i < NU_V3_SIZE; ++i)
    {
        const jsmntok_t *primitive = tok + i + 1;
        if (primitive->type != JSMN_PRIMITIVE)
        {
            NU_ERROR("json float array doesn't contains primitives");
            return;
        }
        NU_ASSERT(
            nu_str_to_f32(nu__seria_json_value(j->json, primitive), p + i));
    }
}

#endif

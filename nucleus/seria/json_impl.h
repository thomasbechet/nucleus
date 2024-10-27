#ifndef NU_SERIA_JSON_IMPL_H
#define NU_SERIA_JSON_IMPL_H

#include <nucleus/internal.h>

#ifdef NU_BUILD_JSMN
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
nu__seria_json_init (nu__seria_json_t *j)
{
    j->toks       = NU_NULL;
    j->toks_count = 0;
}
static void
nu__seria_json_free (nu__seria_json_t *j)
{
}

static void
nu__seria_json_open_toks (nu__seria_json_t *j,
                          nu_str_t          json,
                          const jsmntok_t  *toks,
                          nu_size_t         toks_count)
{
    j->json       = json;
    j->toks       = toks;
    j->toks_count = toks_count;
    j->owned      = NU_FALSE;
}
static void
nu__seria_json_close (nu__seria_json_t *j)
{
    j->toks       = NU_NULL;
    j->toks_count = 0;
}

static nu_bool_t
nu__seria_json_parse_int (nu_str_t json, const jsmntok_t *tok, nu_size_t *value)
{
    return NU_FALSE;
}
static nu_bool_t
nu__seria_json_parse_float (nu_str_t         json,
                            const jsmntok_t *tok,
                            nu_f32_t        *value)
{
    return NU_FALSE;
}
static void
nu__seria_json_parse_value (nu__seria_json_t       *j,
                            const nu__seria_type_t *t,
                            const jsmntok_t        *tok,
                            nu_byte_t              *data)
{
    switch (t->kind)
    {
        case NU__SERIA_PRIMITIVE: {
            switch (t->primitive)
            {
                case NU_SERIA_PRIMITIVE_BUF: {
                }
                break;
                case NU_SERIA_PRIMITIVE_U32: {
                }
                break;
                case NU_SERIA_PRIMITIVE_F32: {
                }
                break;
                case NU_SERIA_PRIMITIVE_V3: {
                }
                break;
                case NU_SERIA_PRIMITIVE_Q4: {
                }
                break;
            }
        }
        break;
        case NU__SERIA_STRUCT: {
            if (tok->type != JSMN_OBJECT)
            {
                NU_ERROR("json token is not an object for type " NU_STR_FMT,
                         NU_STR_ARGS(t->name));
                return;
            }
            for (nu_size_t f = 0; f < t->fields.size; ++f)
            {
                const nu__seria_struct_field_t *field = t->fields.data + f;
                // find json field name
                const jsmntok_t *child_tok
                    = nu__seria_json_object_member(j->json, tok, field->name);
                if (!child_tok)
                {
                    NU_ERROR("json member not found '" NU_STR_FMT "'",
                             NU_STR_ARGS(field->name));
                    return;
                }
                if ((nu_size_t)child_tok->size != field->count)
                {
                    NU_ERROR("invalid field value count, expect %d got %d",
                             field->count,
                             child_tok->size);
                    return;
                }
                if (field->count > 1 && child_tok->type != JSMN_ARRAY)
                {
                    NU_ERROR("field count > 1 but token is not an array");
                    return;
                }
                const nu__seria_type_t *subtype = _ctx.seria.types.data + f;
                for (nu_size_t i = 0; i < field->count; ++i)
                {
                    nu_byte_t *ptr = data + i * subtype->size;
                    nu__seria_json_parse_value(j, subtype, child_tok, ptr);
                    child_tok = nu__seria_json_skip(child_tok);
                }
            }
        }
        break;
        case NU__SERIA_ENUM: {
            if (tok->type != JSMN_STRING)
            {
                NU_ERROR("enum field is not a json");
                return;
            }
            nu_str_t value = nu__seria_json_value(j->json, tok);

            for (nu_size_t i = 0; i < t->values.size; ++i)
            {
                if (NU_MATCH(value, t->values.data[i].name))
                {
                    *((nu_u32_t *)data) = t->values.data[i].value;
                    return;
                }
            }
            NU_ERROR("unknown enum value '" NU_STR_FMT "' for type '" NU_STR_FMT
                     "'",
                     NU_STR_ARGS(value),
                     NU_STR_ARGS(t->name));
            return;
        }
        break;
    }
}
static nu_size_t
nu__seria_json_read (nu__seria_json_t *j,
                     nu_seria_buffer_t buffer,
                     nu_seria_type_t   type,
                     nu_size_t         capacity,
                     void             *data)
{
    const nu__seria_type_t *t = _ctx.seria.types.data + NU_HANDLE_INDEX(type);

    // construct name
    NU_STR_BUF(name_buf, 32);
    nu_str_t name
        = nu_str_fmt(name_buf, NU_STR("_%d"), NU_HANDLE_INDEX(buffer));

    // find token
    if (j->toks_count == 0 || j->toks[0].type != JSMN_OBJECT)
    {
        return 0;
    }
    const jsmntok_t *tok = nu__seria_json_object_member(j->json, j->toks, name);
    if (!tok)
    {
        NU_ERROR("buffer index '" NU_STR_FMT "' not found", NU_STR_ARGS(name));
        return 0;
    }
    if (tok->type != JSMN_ARRAY)
    {
        NU_ERROR("invalid root object type (must be an array)");
        return 0;
    }
    if (tok->size == 0)
    {
        return 0; // empty buffer
    }
    tok = tok + 1; // first array item

    // parse objects
    nu_size_t n = 0;
    for (n = 0; n < capacity; ++n)
    {
        nu_byte_t *ptr = (nu_byte_t *)((nu_size_t)data + n * t->size);
        nu__seria_json_parse_value(j, t, tok, ptr);
        tok = nu__seria_json_skip(tok);
    }

    return n;
}
static nu_seria_buffer_t
nu__seria_json_write (nu__seria_json_t *j,
                      nu_seria_type_t   type,
                      nu_size_t         count,
                      void             *data)
{
    return NU_NULL;
}

#endif

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
                    if (tok->type != JSMN_PRIMITIVE)
                    {
                        NU_ERROR("json token is not a primitive (expect buf)");
                        return;
                    }
                    nu_u32_t id;
                    NU_ASSERT(
                        nu_str_to_u32(nu__seria_json_value(j->json, tok), &id));
                    *((nu_seria_buffer_t *)data)
                        = NU_HANDLE_MAKE(nu_seria_buffer_t, id);
                }
                break;
                case NU_SERIA_PRIMITIVE_U32: {
                    if (tok->type != JSMN_PRIMITIVE)
                    {
                        NU_ERROR("json token is not a primitive (expect u32)");
                        return;
                    }
                    NU_ASSERT(nu_str_to_u32(nu__seria_json_value(j->json, tok),
                                            (nu_u32_t *)data));
                }
                break;
                case NU_SERIA_PRIMITIVE_F32: {
                    if (tok->type != JSMN_PRIMITIVE)
                    {
                        NU_ERROR("json token is not a primitive(expect f32)");
                        return;
                    }
                    NU_ASSERT(nu_str_to_f32(nu__seria_json_value(j->json, tok),
                                            (nu_f32_t *)data));
                }
                break;
                case NU_SERIA_PRIMITIVE_V3: {
                    nu__seria_json_parse_f32_array(j, tok, 3, data);
                }
                break;
                case NU_SERIA_PRIMITIVE_Q4: {
                    nu__seria_json_parse_f32_array(j, tok, 4, data);
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
                if (field->count > 1 && child_tok->type != JSMN_ARRAY)
                {
                    NU_ERROR("field count > 1 but token is not an array");
                    return;
                }
                const nu__seria_type_t *subtype
                    = _ctx.seria.types.data + NU_HANDLE_INDEX(field->type);
                for (nu_size_t i = 0; i < field->count; ++i)
                {
                    nu_byte_t *ptr = data + field->offset + i * subtype->size;
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
nu__seria_json_begin_read (nu__seria_json_t *j,
                           nu_seria_type_t   type,
                           nu_seria_buffer_t buffer)
{
    // construct name
    NU_STR_BUF(name_buf, 32);
    nu_str_t name;
    if (buffer)
    {
        name = nu_str_fmt(name_buf, NU_STR("_%d"), NU_HANDLE_INDEX(buffer));
    }
    else
    {
        name = NU_STR("_root"); // root buffer
    }

    // find token
    if (j->toks_size == 0 || j->toks[0].type != JSMN_OBJECT)
    {
        NU_ERROR("malformed json");
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
    j->it           = tok + 1; // first array item
    j->it_remaining = tok->size;
    j->type         = type;
    return tok->size;
}
static nu_size_t
nu__seria_json_read (nu__seria_json_t *j, nu_size_t count, void *data)
{
    const nu__seria_type_t *t
        = _ctx.seria.types.data + NU_HANDLE_INDEX(j->type);
    NU_ASSERT(j->it);

    // parse objects
    nu_size_t n = 0;
    for (n = 0; n < count; ++n)
    {
        if (!j->it_remaining)
        {
            break;
        }
        --j->it_remaining;
        nu_byte_t *ptr = (nu_byte_t *)((nu_size_t)data + n * t->size);
        nu__seria_json_parse_value(j, t, j->it, ptr);
        j->it = nu__seria_json_skip(j->it);
    }

    return n;
}

#endif

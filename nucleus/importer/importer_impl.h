#ifndef NU_IMPORTER_IMPL_H
#define NU_IMPORTER_IMPL_H

#include <nucleus/internal.h>

#ifdef NU_BUILD_JSMN
#define JSMN_PARENT_LINKS
#define JSMN_STRICT
#define JSMN_STATIC
#include <nucleus/external/jsmn/jsmn.h>
#endif

#include <nucleus/importer/package_impl.h>
#include <nucleus/importer/model_impl.h>
#include <nucleus/importer/image_impl.h>
#include <nucleus/importer/cubemap_impl.h>

static jsmntok_t *
nu__json_parse (const nu_char_t *json,
                nu_size_t        json_size,
                nu_size_t       *size,
                nu_size_t       *count)
{
    nu_assert(json);
    nu_size_t   cap  = 256;
    jsmntok_t  *toks = (jsmntok_t *)nu_alloc(sizeof(*toks) * cap);
    jsmn_parser parser;
    jsmn_init(&parser);
    int r = jsmn_parse(&parser, json, json_size, toks, cap);
    while (r == JSMN_ERROR_NOMEM)
    {
        nu_size_t old_cap = cap;
        cap *= 2;
        toks = (jsmntok_t *)nu_realloc(
            toks, sizeof(*toks) * old_cap, sizeof(*toks) * cap);
        r = jsmn_parse(&parser, json, json_size, toks, cap);
    }
    nu_check(r >= 1, goto cleanup0);
    *count = r;
    *size  = sizeof(*toks) * cap;
    return toks;
cleanup0:
    nu_free(toks, sizeof(*toks) * cap);
    return NU_NULL;
}
static nu_bool_t
nu__json_eq (const nu_char_t *json, jsmntok_t *tok, const nu_char_t *s)
{
    return (tok->type == JSMN_STRING && (int)strlen(s) == tok->end - tok->start
            && strncmp(json + tok->start, s, tok->end - tok->start) == 0);
}
static void
nu__json_value (const nu_char_t *json,
                jsmntok_t       *tok,
                nu_char_t       *s,
                nu_size_t        n)
{
    nu_memset(s, 0, n);
    nu_strncpy(s,
               json + tok->start,
               nu_min((nu_size_t)(tok->end - tok->start), n - 1));
}
static jsmntok_t *
nu__json_skip (jsmntok_t *token)
{
    int pending = 1;
    do
    {
        pending += token->size - 1;
        token++;
    } while (pending);
    return token;
}
static jsmntok_t *
nu__json_object_member (const nu_char_t *json,
                        jsmntok_t       *object,
                        const nu_char_t *name)
{
    if (!object || object->type != JSMN_OBJECT || !name)
    {
        return NULL;
    }

    int        members = object->size;
    jsmntok_t *token   = object + 1;
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

static nu_byte_t *
nu__bytes_load_filename (const nu_char_t *filename, nu_size_t *size)
{
    FILE *f = fopen(filename, "rb");
    if (!f)
    {
        NU_ERROR("failed to open file %s", filename);
        return NU_NULL;
    }
    fseek(f, 0, SEEK_END);
    nu_size_t fsize = ftell(f);
    fseek(f, 0, SEEK_SET);
    nu_byte_t *bytes = (nu_byte_t *)nu_alloc(fsize);
    fread(bytes, fsize, 1, f);
    *size = fsize;
    return bytes;
}

static void
nu__importer_init (void)
{
#ifdef NU_BUILD_CGLTF
    nu__model_gltf_loader_init();
#endif
}
static void
nu__importer_free (void)
{
#ifdef NU_BUILD_CGLTF
    nu__model_gltf_loader_free();
#endif
}

#endif

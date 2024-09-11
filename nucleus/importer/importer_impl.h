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

static int
nu__jsoneq (const nu_char_t *json, jsmntok_t *tok, const nu_char_t *s)
{
    if (tok->type == JSMN_STRING && (int)strlen(s) == tok->end - tok->start
        && strncmp(json + tok->start, s, tok->end - tok->start) == 0)
    {
        return 0;
    }
    return -1;
}
static jsmntok_t *
nu__json_load_filename (const nu_char_t *filename, nu_size_t *s)
{
    nu_size_t  n;
    nu_char_t *json = (nu_char_t *)nu__bytes_load_filename(filename, &n);
    if (!json)
    {
        return NU_NULL;
    }
    nu_size_t   size = 256;
    jsmntok_t  *toks = (jsmntok_t *)nu_alloc(sizeof(*toks) * size);
    jsmn_parser parser;
    jsmn_init(&parser);
    int r = jsmn_parse(&parser, json, n, toks, size);
    nu_check(r >= 1, goto cleanup0);
    return toks;
cleanup0:
    nu_free(toks, sizeof(*toks) * size);
    return NU_NULL;
}

static nu_byte_t *
nu__bytes_load_filename (const nu_char_t *filename, nu_size_t *size)
{
    FILE *f = fopen(filename, "rb");
    fseek(f, 0, SEEK_END);
    nu_size_t fsize = ftell(f);
    fseek(f, 0, SEEK_SET);
    nu_byte_t *bytes = (nu_byte_t *)nu_alloc(fsize);
    fread(bytes, fsize, 1, f);
    *size = fsize;
    return bytes;
}

nu_asset_t
nuext_asset_load_filename (nu_asset_type_t  type,
                           const nu_char_t *name,
                           const nu_char_t *filename)
{
    nu_asset_t handle = nu_asset_add(type, name);
    nu_handle_check(handle, return handle);
    nu_asset_data_t *data = &_ctx.asset.entries.data[handle.id].data;

    switch (type)
    {
        case NU_ASSET_TEXTURE: {
            nu_image_t image = nuext_image_load_filename(filename);
            nu_handle_check(image, return NU_HANDLE_INVALID(nu_asset_t));
            data->texture = nu_texture_create_image(image);
            nu_image_delete(image);
        }
        break;
        case NU_ASSET_CUBEMAP: {
            data->cubemap = nuext_cubemap_load_filename(filename);
            nu_handle_check(data->cubemap,
                            return NU_HANDLE_INVALID(nu_asset_t));
        }
        break;
        case NU_ASSET_MATERIAL:
        case NU_ASSET_MODEL:
            data->model = nuext_model_load_filename(filename);
            nu_handle_check(data->model, return NU_HANDLE_INVALID(nu_asset_t));
            break;
        case NU_ASSET_INPUT:
        case NU_ASSET_TABLE:
            break;
    }

    return handle;
}
nu_error_t
nuext_asset_load_package (const nu_char_t *filename)
{
    const nu_char_t *dir = nuext_path_dirname(filename);

    nu_size_t size;
    // nu_char_t *json = nuext_bytes_load_filename(filename, &size);

    // nu_free(json);
    return NU_ERROR_NONE;
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

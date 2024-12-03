#ifndef NU_IMPORTER_PACKAGE_H
#define NU_IMPORTER_PACKAGE_H

#include <nucleus/internal.h>

static void
nu__import_image (nu_str_t filename, nu_uid_t group, nu_uid_t uid)
{
    nu_image_t image = nuext_image_load_file(filename);
    NU_ASSERT(image);
    nu_object_set_uid(image, uid);
}
static void
nu__import_colormap (nu_str_t filename, nu_uid_t group, nu_uid_t uid)
{
    nu_image_t image = nuext_image_load_file(filename);
    NU_ASSERT(image);
    nu_texture_t texture
        = nu_texture_new_image_texture(NU_TEXTURE_COLORMAP, image);
    NU_ASSERT(texture);
    nu_object_set_uid(texture, uid);
}
static void
nu__import_cubemap (nu_str_t filename, nu_uid_t group, nu_uid_t uid)
{
    nu_image_t image = nuext_image_load_file(filename);
    NU_ASSERT(image);
    nu_texture_t texture
        = nu_texture_new_image_texture(NU_TEXTURE_CUBEMAP, image);
    NU_ASSERT(texture);
    nu_object_set_uid(texture, uid);
}
static void
nu__import_model (nu_str_t filename, nu_uid_t group, nu_uid_t uid)
{
    nu_model_t model = nuext_model_load_file(filename);
    NU_ASSERT(model);
    nu_object_set_uid(model, uid);
}
static void
nu__import_input (nu_uid_t group, nu_uid_t uid)
{
    nu_input_t input = nu_input_new();
    NU_ASSERT(input);
    nu_object_set_uid(input, uid);
}
nu_error_t
nuext_import_package (nu_str_t filename, nu_uid_t group)
{
    nu_error_t error = NU_ERROR_RESOURCE_LOADING;

    nu_str_t dir = nuext_path_dirname(filename);

    NU_INFO("loading package " NU_STR_FMT, NU_STR_ARGS(filename));

    nu_scope_push();
    nu_size_t  json_size;
    nu_byte_t *json_buf = nu__seria_load_bytes(filename, &json_size);
    nu_str_t   json     = nu_str(json_buf, json_size);
    if (!json_buf)
    {
        NU_ERROR("failed to load json package");
        goto cleanup0;
    }

    nu_size_t  toks_size, toks_count;
    jsmntok_t *toks = nu__json_parse(json, &toks_size, &toks_count);
    if (!toks)
    {
        NU_ERROR("failed to parse json package");
        goto cleanup0;
    }

    if (toks[0].type != JSMN_ARRAY)
    {
        NU_ERROR("top level json must be an array for packages");
        goto cleanup0;
    }
    nu_size_t max_asset_count = toks[0].size;

    const jsmntok_t *tok = &toks[1];
    for (nu_size_t i = 0; i < max_asset_count; ++i)
    {
        if (tok->type == JSMN_OBJECT)
        {
            // Parse name
            const jsmntok_t *tname
                = nu__json_object_member(json, tok, NU_STR("name"));
            if (!tname)
            {
                NU_ERROR("name member not found");
                goto cleanup0;
            }
            nu_str_t name = nu__json_value(json, tname);

            // Parse path
            const jsmntok_t *tpath
                = nu__json_object_member(json, tok, NU_STR("path"));
            if (!tpath)
            {
                NU_ERROR("path member not found");
                goto cleanup0;
            }
            nu_str_t path = nu__json_value(json, tpath);
            NU_STR_BUF(final_path_buf, NUEXT_PATH_MAX);
            nu_str_t final_path = nuext_path_concat(final_path_buf, dir, path);

            // Parse type
            const jsmntok_t *ttype
                = nu__json_object_member(json, tok, NU_STR("type"));
            if (!ttype)
            {
                NU_ERROR("type member not found");
                goto cleanup0;
            }
            if (nu__json_eq(json, ttype, NU_STR("model")))
            {
                nu__import_model(final_path, group, nu_str_hash(name));
            }
            else if (nu__json_eq(json, ttype, NU_STR("colormap")))
            {
                nu__import_colormap(final_path, group, nu_str_hash(name));
            }
            else if (nu__json_eq(json, ttype, NU_STR("cubemap")))
            {
                nu__import_cubemap(final_path, group, nu_str_hash(name));
            }
            else if (nu__json_eq(json, ttype, NU_STR("image")))
            {
                nu__import_image(final_path, group, nu_str_hash(name));
            }
            else if (nu__json_eq(json, ttype, NU_STR("input")))
            {
                nu__import_input(group, nu_str_hash(name));
            }
            else
            {
                NU_ERROR("unknown asset type " NU_STR_FMT,
                         nu__json_value(json, ttype));
                goto cleanup0;
            }

            NU_INFO("'" NU_STR_FMT "' resource added (%llu)",
                    NU_STR_ARGS(name),
                    nu_str_hash(name));
        }
        tok = nu__json_skip(tok);
    }

    error = NU_ERROR_NONE;
cleanup0:
    // nu_scope_pop();
    return error;
}

#endif

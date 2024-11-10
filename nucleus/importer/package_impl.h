#ifndef NU_IMPORTER_PACKAGE_H
#define NU_IMPORTER_PACKAGE_H

#include <nucleus/internal.h>

void
nuext_import_image (nu_str_t filename, nu_uid_t uid)
{
    nu_image_t image = nuext_image_load_file(filename);
    NU_ASSERT(image);
    nu_resource_add(_ctx.graphics.res_image, uid, image);
}
void
nuext_import_model (nu_str_t filename, nu_uid_t uid)
{
    nu_model_t model = nuext_model_load_file(filename);
    NU_ASSERT(model);
    nu_resource_add(_ctx.graphics.res_model, uid, model);
}
void
nuext_import_cubemap (nu_str_t filename, nu_uid_t uid)
{
    nu_texture_t cubemap = nuext_cubemap_load_file(filename);
    NU_ASSERT(cubemap);
    nu_resource_add(_ctx.graphics.res_texture, uid, cubemap);
}
nu_error_t
nuext_import_package (nu_str_t filename)
{
    nu_error_t error = NU_ERROR_RESOURCE_LOADING;

    nu_str_t dir = nuext_path_dirname(filename);

    NU_INFO("loading package " NU_STR_FMT, NU_STR_ARGS(filename));

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
        goto cleanup1;
    }

    if (toks[0].type != JSMN_ARRAY)
    {
        NU_ERROR("top level json must be an array for packages");
        goto cleanup2;
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
                goto cleanup2;
            }
            nu_str_t name = nu__json_value(json, tname);

            // Parse path
            const jsmntok_t *tpath
                = nu__json_object_member(json, tok, NU_STR("path"));
            if (!tpath)
            {
                NU_ERROR("path member not found");
                goto cleanup2;
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
                goto cleanup2;
            }
            if (nu__json_eq(json, ttype, NU_STR("model")))
            {
                nuext_import_model(final_path, nu_str_hash(name));
            }
            else if (nu__json_eq(json, ttype, NU_STR("texture")))
            {
                if (nuext_path_extension(final_path) == NUEXT_EXTENSION_JSON)
                {
                    nuext_import_cubemap(final_path, nu_str_hash(name));
                }
                else
                {
                    nuext_import_image(final_path, nu_str_hash(name));
                }
            }
            else
            {
                NU_ERROR("unknown asset type " NU_STR_FMT,
                         nu__json_value(json, ttype));
                goto cleanup2;
            }

            NU_INFO("'" NU_STR_FMT "' resource added (%llu)",
                    NU_STR_ARGS(name),
                    nu_str_hash(name));
        }
        tok = nu__json_skip(tok);
    }

    error = NU_ERROR_NONE;
cleanup2:
    nu_free(toks, toks_size);
cleanup1:
    nu_free(json_buf, json_size);
cleanup0:
    return error;
}

#endif

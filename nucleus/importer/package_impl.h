#ifndef NU_IMPORTER_PACKAGE_H
#define NU_IMPORTER_PACKAGE_H

#include <nucleus/internal.h>

nu_resource_t
nuext_import_resource (nu_resource_type_t type, nu_str_t name, nu_str_t filename)
{
    nu_resource_t handle = nu_resource_add(type, name);
    NU_CHECK(handle, return handle);
    nu__resource_entry_t *entry
        = &_ctx.resource.entries.data[NU_HANDLE_INDEX(handle)];

    switch (type)
    {
        case NU_RESOURCE_TEXTURE: {
            if (nuext_path_extension(filename) == NUEXT_EXTENSION_JSON)
            {
                entry->data = nuext_cubemap_load_filename(filename);
                NU_CHECK(entry->data, return NU_NULL);
            }
            else
            {
                nu_image_t image = nuext_image_load_filename(filename);
                NU_CHECK(image, return NU_NULL);
                entry->data = nu_image_create_texture(image);
                nu_image_delete(image);
            }
        }
        break;
        case NU_RESOURCE_MATERIAL:
        case NU_RESOURCE_MODEL:
            entry->data = nuext_model_load_filename(filename);
            NU_CHECK(entry->data, return NU_NULL);
            break;
        case NU_RESOURCE_INPUT:
        case NU_RESOURCE_UNKNOWN:
            break;
    }

    return handle;
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
            nu_resource_type_t type = NU_RESOURCE_UNKNOWN;
            const jsmntok_t   *ttype
                = nu__json_object_member(json, tok, NU_STR("type"));
            if (!ttype)
            {
                NU_ERROR("type member not found");
                goto cleanup2;
            }
            if (nu__json_eq(json, ttype, NU_STR("model")))
            {
                nuext_import_resource(NU_RESOURCE_MODEL, name, final_path);
            }
            else if (nu__json_eq(json, ttype, NU_STR("texture")))
            {
                nuext_import_resource(NU_RESOURCE_TEXTURE, name, final_path);
            }
            else
            {
                NU_ERROR("unknown asset type " NU_STR_FMT,
                         nu__json_value(json, ttype));
                goto cleanup2;
            }

            NU_INFO("'" NU_STR_FMT "' asset added", NU_STR_ARGS(name));
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

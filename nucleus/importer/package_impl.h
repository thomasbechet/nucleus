#ifndef NU_IMPORTER_PACKAGE_H
#define NU_IMPORTER_PACKAGE_H

#include <nucleus/internal.h>

nu_asset_t
nuext_import_asset (nu_asset_type_t  type,
                    const nu_char_t *name,
                    const nu_char_t *filename)
{
    nu_asset_t handle = nu_asset_add(type, name);
    NU_CHECK(handle, return handle);
    nu__asset_entry_t *entry
        = &_ctx.asset.entries.data[NU_HANDLE_INDEX(handle)];

    switch (type)
    {
        case NU_ASSET_TEXTURE: {
            nu_image_t image = nuext_image_load_filename(filename);
            NU_CHECK(image, return NU_NULL);
            entry->data = nu_image_create_texture(image);
            nu_image_delete(image);
        }
        break;
        case NU_ASSET_CUBEMAP: {
            entry->data = nuext_cubemap_load_filename(filename);
            NU_CHECK(entry->data, return NU_NULL);
        }
        break;
        case NU_ASSET_MATERIAL:
        case NU_ASSET_MODEL:
            entry->data = nuext_model_load_filename(filename);
            NU_CHECK(entry->data, return NU_NULL);
            break;
        case NU_ASSET_INPUT:
        case NU_ASSET_TABLE:
        case NU_ASSET_UNKNOWN:
            break;
    }

    return handle;
}
nu_error_t
nuext_import_package (const nu_char_t *filename)
{
    nu_error_t error = NU_ERROR_RESOURCE_LOADING;

    nu_char_t dir[NUEXT_PATH_MAX];
    nuext_path_dirname(filename, dir);

    NU_INFO("loading package %s", filename);

    nu_size_t  json_size;
    nu_char_t *json
        = (nu_char_t *)nu__bytes_load_filename(filename, &json_size);
    if (!json)
    {
        NU_ERROR("failed to load json package");
        goto cleanup0;
    }

    nu_size_t  toks_size, toks_count;
    jsmntok_t *toks = nu__json_parse(json, json_size, &toks_size, &toks_count);
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

    jsmntok_t *tok = &toks[1];
    for (nu_size_t i = 0; i < max_asset_count; ++i)
    {
        if (tok->type == JSMN_OBJECT)
        {
            // Parse name
            jsmntok_t *tname = nu__json_object_member(json, tok, "name");
            if (!tname)
            {
                NU_ERROR("name member not found");
                goto cleanup2;
            }
            nu_char_t name[NU_ASSET_NAME_MAX];
            nu__json_value(json, tname, name, NU_ASSET_NAME_MAX);
            NU_INFO("name:%s", name);

            // Parse path
            jsmntok_t *tpath = nu__json_object_member(json, tok, "path");
            if (!tpath)
            {
                NU_ERROR("path member not found");
                goto cleanup2;
            }
            nu_char_t path[NUEXT_PATH_MAX];
            nu__json_value(json, tpath, path, NUEXT_PATH_MAX);
            NU_INFO("path:%s", path);
            nu_char_t final_path[NUEXT_PATH_MAX];
            nuext_path_concat(dir, path, final_path);

            // Parse type
            nu_asset_type_t type  = NU_ASSET_UNKNOWN;
            jsmntok_t      *ttype = nu__json_object_member(json, tok, "type");
            if (!ttype)
            {
                NU_ERROR("type member not found");
                goto cleanup2;
            }
            if (nu__json_eq(json, ttype, "model"))
            {
                nuext_import_asset(NU_ASSET_MODEL, name, final_path);
            }
            else if (nu__json_eq(json, ttype, "texture"))
            {
                nuext_import_asset(NU_ASSET_TEXTURE, name, final_path);
            }
            else if (nu__json_eq(json, ttype, "cubemap"))
            {
                nuext_import_asset(NU_ASSET_CUBEMAP, name, final_path);
            }
            else
            {
                NU_ERROR("unknown asset type %s", json + ttype->start);
                goto cleanup2;
            }

            NU_INFO("'%s' asset added", name);
        }
        tok = nu__json_skip(tok);
    }

    error = NU_ERROR_NONE;
cleanup2:
    nu_free(toks, toks_size);
cleanup1:
    nu_free(json, json_size);
cleanup0:
    return error;
}

#endif

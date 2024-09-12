#ifndef NU_IMPORTER_PACKAGE_H
#define NU_IMPORTER_PACKAGE_H

#include <nucleus/internal.h>

nu_asset_t
nuext_import_asset (nu_asset_type_t  type,
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

    nu_info("loading package %s", filename);

    nu_size_t  json_size;
    nu_char_t *json
        = (nu_char_t *)nu__bytes_load_filename(filename, &json_size);
    if (!json)
    {
        nu_error("failed to load json package");
        goto cleanup0;
    }

    nu_size_t  toks_size, toks_count;
    jsmntok_t *toks = nu__json_parse(json, json_size, &toks_size, &toks_count);
    if (!json)
    {
        nu_error("failed to parse json package");
        goto cleanup1;
    }

    if (toks[0].type != JSMN_ARRAY)
    {
        nu_error("top level json must be an array for packages");
        goto cleanup2;
    }
    nu_size_t max_asset_count = toks[0].size;

    jsmntok_t *tok = &toks[1];
    while ((nu_size_t)(tok - &toks[0]) / sizeof(*tok) < max_asset_count)
    {
        if (tok->type != JSMN_OBJECT)
        {
            tok = nu__json_skip(tok);
        }
        else
        {
            // Parse name
            jsmntok_t *tname = nu__json_object_member(json, tok, "name");
            if (!tname)
            {
                nu_error("name member not found");
                goto cleanup2;
            }
            nu_char_t name[NU_ASSET_NAME_MAX];
            nu__json_value(json, tname, name, NU_ASSET_NAME_MAX);
            nu_info("name:%s", name);

            // Parse path
            jsmntok_t *tpath = nu__json_object_member(json, tok, "path");
            if (!tpath)
            {
                nu_error("path member not found");
                goto cleanup2;
            }
            nu_char_t path[NUEXT_PATH_MAX];
            nu__json_value(json, tpath, path, NUEXT_PATH_MAX);
            nu_info("path:%s", path);
            nu_char_t final_path[NUEXT_PATH_MAX];
            nuext_path_concat(dir, path, final_path);

            // Parse type
            nu_asset_type_t type  = NU_ASSET_UNKNOWN;
            jsmntok_t      *ttype = nu__json_object_member(json, tok, "type");
            if (!ttype)
            {
                nu_error("type member not found");
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
                nu_error("unknown asset type %s", json + ttype->start);
                goto cleanup2;
            }

            nu_info("'%s' asset added", name);

            tok++;
        }
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

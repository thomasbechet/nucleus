#ifndef NU_IMPORTER_CUBEMAP_H
#define NU_IMPORTER_CUBEMAP_H

#include <nucleus/internal.h>

nu_texture_t
nuext_cubemap_load_filename (nu_str_t filename)
{
    nu_image_t   images[NU_CUBEMAP_FACE_COUNT];
    nu_texture_t cubemap = NU_NULL;
    NU_ARRAY_FILL(images, NU_CUBEMAP_FACE_COUNT, NU_NULL);

    nu_size_t  json_size;
    nu_byte_t *json_buf = nu__bytes_load_filename(filename, &json_size);
    nu_str_t   json     = nu_str(json_buf, json_size);
    NU_CHECK(json_buf, goto cleanup0);
    nu_size_t  toks_size, toks_count;
    jsmntok_t *toks = nu__seria_json_parse(json, &toks_size, &toks_count);
    NU_CHECK(toks, goto cleanup1);

    nu_str_t json_path = nuext_path_dirname(filename);

    nu_size_t image_count = 0;
    NU_CHECK(toks[0].type == JSMN_OBJECT, goto cleanup0);

    static nu_str_t faces[]
        = { NU_STR("posx"), NU_STR("negx"), NU_STR("posy"),
            NU_STR("negy"), NU_STR("posz"), NU_STR("negz") };
    for (nu_size_t f = 0; f < NU_ARRAY_SIZE(faces); ++f)
    {
        const jsmntok_t *tok
            = nu__seria_json_object_member(json, &toks[0], faces[f]);
        if (!tok)
        {
            NU_ERROR("cubemap face not found '%s'", faces[f]);
            goto cleanup2;
        }
        if (tok->type != JSMN_STRING)
        {
            NU_ERROR("invalid face path");
            goto cleanup2;
        }
        if (!images[f])
        {
            nu_str_t path = nu__seria_json_value(json, tok);
            NU_STR_BUF(final_path_buf, NUEXT_PATH_MAX);
            nu_str_t final_path
                = nuext_path_concat(final_path_buf, json_path, path);
            images[f] = nuext_image_load_filename(final_path);
            if (!images[f])
            {
                NU_ERROR("cubemap face loading error '%s'", path);
                goto cleanup2;
            }
        }
    }

    nu_v2u_t size = nu_image_size(images[0]);
    cubemap       = nu_texture_create(NU_TEXTURE_CUBEMAP_COLOR,
                                nu_v3u(size.x, size.y, 0));
    for (nu_size_t i = 0; i < NU_CUBEMAP_FACE_COUNT; ++i)
    {
        nu_texture_write_cubemap_colors(cubemap, i, nu_image_colors(images[i]));
    }

cleanup2:
    for (nu_size_t i = 0; i < NU_ARRAY_SIZE(images); ++i)
    {
        if (images[i])
        {
            nu_image_delete(images[i]);
        }
    }
cleanup1:
    nu_free(toks, toks_size);
cleanup0:
    nu_free(json_buf, json_size);
    return cubemap;
}

#endif

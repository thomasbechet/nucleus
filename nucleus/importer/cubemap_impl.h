#ifndef NU_IMPORTER_CUBEMAP_H
#define NU_IMPORTER_CUBEMAP_H

#include <nucleus/internal.h>

nu_texture_t
nuext_cubemap_load_filename (const nu_char_t *filename)
{
    nu_image_t   images[NU_CUBEMAP_FACE_COUNT];
    nu_texture_t cubemap = NU_NULL;
    NU_ARRAY_FILL(images, NU_CUBEMAP_FACE_COUNT, NU_NULL);

    nu_size_t  json_size;
    nu_char_t *json
        = (nu_char_t *)nu__bytes_load_filename(filename, &json_size);
    NU_CHECK(json, goto cleanup0);
    nu_size_t  toks_size, toks_count;
    jsmntok_t *toks = nu__json_parse(json, json_size, &toks_size, &toks_count);
    NU_CHECK(toks, goto cleanup1);

    nu_char_t json_path[NUEXT_PATH_MAX];
    nuext_path_dirname(filename, json_path);

    nu_size_t image_count = 0;
    NU_CHECK(toks[0].type == JSMN_OBJECT, goto cleanup0);

    static const nu_char_t *faces[]
        = { "posx", "negx", "posy", "negy", "posz", "negz" };
    for (nu_size_t f = 0; f < NU_ARRAY_SIZE(faces); ++f)
    {
        jsmntok_t *tok = nu__json_object_member(json, &toks[0], faces[f]);
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
            nu_char_t path[NUEXT_PATH_MAX];
            nu__json_value(json, tok, path, NUEXT_PATH_MAX);
            nu_char_t final_path[NUEXT_PATH_MAX];
            nuext_path_concat(json_path, path, final_path);
            images[f] = nuext_image_load_filename(final_path);
            if (!images[f])
            {
                NU_ERROR("cubemap face loading error '%s'", path);
                goto cleanup2;
            }
        }
    }

    nu_vec2u_t size = nu_image_size(images[0]);
    cubemap         = nu_texture_create(NU_TEXTURE_CUBEMAP_COLOR,
                                nu_vec3u(size.x, size.y, 0));
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
    nu_free(json, json_size);
    return cubemap;
}

#endif

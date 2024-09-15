#ifndef NU_IMPORTER_CUBEMAP_H
#define NU_IMPORTER_CUBEMAP_H

#include <nucleus/internal.h>

nu_cubemap_t
nuext_cubemap_load_filename (const nu_char_t *filename)
{
    nu_image_t   images[NU_CUBEMAP_FACE_COUNT];
    nu_cubemap_t cubemap = NU_NULL;
    NU_ARRAY_FILL(images, NU_CUBEMAP_FACE_COUNT, NU_NULL);

    nu_size_t  json_size;
    nu_char_t *json
        = (nu_char_t *)nu__bytes_load_filename(filename, &json_size);
    NU_CHECK(json, goto cleanup0);
    nu_size_t  toks_size, toks_count;
    jsmntok_t *toks = nu__json_parse(json, json_size, &toks_size, &toks_count);
    NU_CHECK(toks, goto cleanup1);

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
            images[f] = nuext_image_load_filename(path);
            if (!images[f])
            {
                NU_ERROR("cubemap face loading error '%s'", path);
                goto cleanup2;
            }
        }
    }

    nu_cubemap_info_t info;
    info.size        = nu_image_size(images[0]).x;
    info.usage       = NU_TEXTURE_USAGE_SAMPLE;
    info.colors_posx = nu_image_colors(images[0]);
    info.colors_negx = nu_image_colors(images[1]);
    info.colors_posy = nu_image_colors(images[2]);
    info.colors_negy = nu_image_colors(images[3]);
    info.colors_posz = nu_image_colors(images[4]);
    info.colors_negz = nu_image_colors(images[5]);
    cubemap          = nu_cubemap_create(&info);

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

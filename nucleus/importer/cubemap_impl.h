#ifndef NU_IMPORTER_CUBEMAP_H
#define NU_IMPORTER_CUBEMAP_H

#include <nucleus/internal.h>

nu_cubemap_t
nuext_cubemap_load_filename (const nu_char_t *filename)
{
    nu_image_t   images[NU_CUBEMAP_FACE_COUNT];
    nu_cubemap_t cubemap = NU_HANDLE_INVALID(nu_cubemap_t);
    nu_array_fill(images, NU_CUBEMAP_FACE_COUNT, NU_HANDLE_INVALID(nu_image_t));

    nu_size_t  json_size;
    nu_char_t *json
        = (nu_char_t *)nu__bytes_load_filename(filename, &json_size);
    nu_check(json, goto cleanup0);
    nu_size_t  toks_size, toks_count;
    jsmntok_t *toks = nu__json_parse(json, json_size, &toks_size, &toks_count);
    nu_check(toks, goto cleanup1);

    nu_size_t image_count = 0;
    nu_check(toks[0].type == JSMN_OBJECT, goto cleanup0);

    static const nu_char_t *faces[]
        = { "posx", "negx", "posy", "negy", "posz", "negz" };
    for (nu_size_t f = 0; f < nu_array_size(faces); ++f)
    {
        jsmntok_t *tok = nu__json_object_member(json, &toks[0], faces[f]);
        if (!tok)
        {
            nu_error("cubemap face not found '%s'", faces[f]);
            goto cleanup2;
        }
        if (tok->type != JSMN_STRING)
        {
            nu_error("invalid face path");
            goto cleanup2;
        }
        if (nu_handle_is_invalid(images[f]))
        {
            nu_char_t path[NUEXT_PATH_MAX];
            nu__json_value(json, tok, path, NUEXT_PATH_MAX);
            images[f] = nuext_image_load_filename(path);
            if (nu_handle_is_invalid(images[f]))
            {
                nu_error("cubemap face loading error '%s'", path);
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
    for (nu_size_t i = 0; i < nu_array_size(images); ++i)
    {
        if (nu_handle_is_valid(images[i]))
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

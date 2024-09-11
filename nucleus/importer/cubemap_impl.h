#ifndef NU_IMPORTER_CUBEMAP_H
#define NU_IMPORTER_CUBEMAP_H

#include <nucleus/internal.h>

nu_cubemap_t
nuext_cubemap_load_filename (const nu_char_t *filename)
{
#if defined(NU_BUILD_JSMN) && defined(NU_STDLIB)
    nu_size_t  size;
    nu_char_t *json = (nu_char_t *)nuext_bytes_load_filename(filename, &size);
    if (!json)
    {
        return NU_HANDLE_INVALID(nu_cubemap_t);
    }
    nu_image_t   images[NU_CUBEMAP_FACE_COUNT];
    nu_cubemap_t cubemap;
    nu_array_fill(images, NU_CUBEMAP_FACE_COUNT, NU_HANDLE_INVALID(nu_image_t));
    nu_size_t   image_count = 0;
    jsmntok_t   toks[256];
    jsmn_parser parser;
    jsmn_init(&parser);
    int r = jsmn_parse(&parser, json, size, toks, 256);
    nu_check(r >= 1, goto cleanup0);
    nu_check(toks[0].type == JSMN_OBJECT, goto cleanup0);
    static const nu_char_t *faces[]
        = { "posx", "negx", "posy", "negy", "posz", "negz" };
    for (int i = 1; i < r; ++i)
    {
        nu_check(toks[i].type == JSMN_STRING, goto cleanup1);
        for (nu_size_t f = 0; f < nu_array_size(faces); ++f)
        {
            if (nu__jsoneq(json, &toks[i], faces[f]) == 0)
            {
                ++i;
                nu_check(toks[i].type == JSMN_STRING, goto cleanup1);
                if (nu_handle_is_invalid(images[f]))
                {
                    nu_char_t path[NUEXT_PATH_MAX];
                    nu_memset(path, 0, sizeof(path));
                    nu_strncpy(
                        path,
                        json + toks[i].start,
                        nu_min(toks[i].end - toks[i].start, NUEXT_PATH_MAX));
                    images[f] = nuext_image_load_filename(path);
                    if (nu_handle_is_invalid(images[f]))
                    {
                        nu_error("cuebmap face loading error '%s'", path);
                        goto cleanup1;
                    }
                }
                break;
            }
        }
    }
    nu_free(json, size);
    for (nu_size_t i = 0; i < nu_array_size(images); ++i)
    {
        if (nu_handle_is_invalid(images[i]))
        {
            nu_error("missing cubemap face '%s'", faces[i]);
            goto cleanup1;
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
    for (nu_size_t i = 0; i < nu_array_size(images); ++i)
    {
        nu_image_delete(images[i]);
    }
    return cubemap;
#endif
cleanup1:
    for (nu_size_t i = 0; i < nu_array_size(images); ++i)
    {
        if (!nu_handle_is_invalid(images[i]))
        {
            nu_image_delete(images[i]);
        }
    }
cleanup0:
    nu_free(json, size);
    return NU_HANDLE_INVALID(nu_cubemap_t);
}

#endif

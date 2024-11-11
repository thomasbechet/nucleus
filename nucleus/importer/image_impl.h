#ifndef NU_IMPORTER_IMAGE_H
#define NU_IMPORTER_IMAGE_H

#include <nucleus/internal.h>

#ifdef NU_BUILD_IMPORTER_STBIMAGE
#define STB_IMAGE_IMPLEMENTATION
#include <nucleus/external/stb/stb_image.h>
#endif

static void
nu__parse_rgba (const nu_byte_t *src,
                nu_byte_t       *dst,
                nu_size_t        pixel_count,
                nu_size_t        comp)
{
    for (nu_size_t i = 0; i < pixel_count; ++i)
    {
        for (nu_size_t c = 0; c < comp; ++c)
        {
            dst[i * 4 + c] = src[i * comp + c];
        }
        if (comp == 4)
        {
            dst[i * 4 + 3] = src[i * comp + 3];
        }
        else
        {
            dst[i * 4 + 3] = 255;
        }
    }
}
static nu_image_t
nu__image_load_file (nu_str_t filename)
{
#ifdef NU_BUILD_IMPORTER_STBIMAGE
    int       w, h, n;
    nu_byte_t fn[256];
    nu_str_to_cstr(filename, fn, 256);
    nu_byte_t *img = stbi_load((char *)fn, &w, &h, &n, STBI_default);
    NU_CHECK(img, return NU_NULL);
    nu_v3u_t   size   = nu_v3u(w, h, 1);
    nu_image_t handle = nu_image_create(NU_IMAGE_RGBA, size, 1);
    nu__parse_rgba(img, nu_image_data(handle, 0), size.x * size.y, n);
    stbi_image_free(img);
    return handle;
#endif
    return NU_NULL;
}
static nu_image_t
nu__image_load_memory (const nu_byte_t *data, nu_size_t size)
{
#ifdef NU_BUILD_IMPORTER_STBIMAGE
    int        w, h, n;
    nu_byte_t *img
        = stbi_load_from_memory(data, size, &w, &h, &n, STBI_default);
    NU_CHECK(img, return NU_NULL);
    nu_v3u_t   image_size = nu_v3u(w, h, 1);
    nu_image_t handle     = nu_image_create(NU_IMAGE_RGBA, image_size, 1);
    nu__parse_rgba(
        img, nu_image_data(handle, 0), image_size.x * image_size.y, n);
    stbi_image_free(img);
    return handle;
#endif
    return NU_NULL;
}
static nu_image_t
nu__image_load_cubemap (nu_str_t filename)
{
    nu_image_t images[NU_CUBEMAP_FACE_COUNT];
    nu_image_t cubemap = NU_NULL;
    NU_ARRAY_FILL(images, NU_CUBEMAP_FACE_COUNT, NU_NULL);

    nu_size_t  json_size;
    nu_byte_t *json_buf = nu__seria_load_bytes(filename, &json_size);
    nu_str_t   json     = nu_str(json_buf, json_size);
    NU_CHECK(json_buf, goto cleanup0);
    nu_size_t  toks_size, toks_count;
    jsmntok_t *toks = nu__json_parse(json, &toks_size, &toks_count);
    NU_CHECK(toks, goto cleanup1);

    nu_str_t json_path = nuext_path_dirname(filename);

    nu_size_t image_count = 0;
    NU_CHECK(toks[0].type == JSMN_OBJECT, goto cleanup0);

    static nu_str_t faces[]
        = { NU_STR("posx"), NU_STR("negx"), NU_STR("posy"),
            NU_STR("negy"), NU_STR("posz"), NU_STR("negz") };
    for (nu_size_t f = 0; f < NU_ARRAY_SIZE(faces); ++f)
    {
        const jsmntok_t *tok = nu__json_object_member(json, &toks[0], faces[f]);
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
            nu_str_t path = nu__json_value(json, tok);
            NU_STR_BUF(final_path_buf, NUEXT_PATH_MAX);
            nu_str_t final_path
                = nuext_path_concat(final_path_buf, json_path, path);
            images[f] = nuext_image_load_file(final_path);
            if (!images[f])
            {
                NU_ERROR("cubemap face loading error '%s'", path);
                goto cleanup2;
            }
        }
    }

    nu_v3u_t size = nu_image_size(images[0]);
    cubemap       = nu_image_create(
        NU_IMAGE_RGBA, nu_v3u(size.x, size.y, 1), NU_CUBEMAP_FACE_COUNT);
    for (nu_size_t i = 0; i < NU_CUBEMAP_FACE_COUNT; ++i)
    {
        nu_memcpy(nu_image_data(cubemap, i),
                  nu_image_data(images[i], 0),
                  4 * size.x * size.y);
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

nu_image_t
nuext_image_load_file (nu_str_t filename)
{
    if (nuext_path_extension(filename) == NUEXT_EXTENSION_JSON)
    {
        return nu__image_load_cubemap(filename);
    }
    else
    {
#ifdef NU_BUILD_IMPORTER_STBIMAGE
        return nu__image_load_file(filename);
#endif
    }
    return NU_NULL;
}
nu_image_t
nuext_image_load_memory (const nu_byte_t *data, nu_size_t size)
{
#ifdef NU_BUILD_IMPORTER_STBIMAGE
    return nu__image_load_memory(data, size);
#endif
    return NU_NULL;
}

#endif

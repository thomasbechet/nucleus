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
static nu_texture_t
nu__texture_load_file (nu_str_t filename)
{
#ifdef NU_BUILD_IMPORTER_STBIMAGE
    int       w, h, n;
    nu_byte_t fn[256];
    nu_str_to_cstr(filename, fn, 256);
    nu_byte_t *img = stbi_load((char *)fn, &w, &h, &n, STBI_default);
    NU_CHECK(img, return NU_NULL);
    nu_v3u_t     size   = nu_v3u(w, h, 1);
    nu_texture_t handle = nu_texture_new(NU_TEXTURE_COLORMAP, size, 1);
    nu__parse_rgba(img, nu_texture_data(handle, 0), size.x * size.y, n);
    stbi_image_free(img);
    return handle;
#endif
    return NU_NULL;
}
static nu_texture_t
nu__texture_load_memory (const nu_byte_t *data, nu_size_t size)
{
#ifdef NU_BUILD_IMPORTER_STBIMAGE
    int        w, h, n;
    nu_byte_t *img
        = stbi_load_from_memory(data, size, &w, &h, &n, STBI_default);
    NU_CHECK(img, return NU_NULL);
    nu_v3u_t     texture_size = nu_v3u(w, h, 1);
    nu_texture_t handle = nu_texture_new(NU_TEXTURE_COLORMAP, texture_size, 1);
    nu__parse_rgba(
        img, nu_texture_data(handle, 0), texture_size.x * texture_size.y, n);
    stbi_image_free(img);
    return handle;
#endif
    return NU_NULL;
}
static nu_texture_t
nu__texture_load_cubemap (nu_str_t filename)
{
    nu_texture_t textures[NU_CUBEMAP_FACE_COUNT];
    nu_texture_t cubemap = NU_NULL;
    NU_ARRAY_FILL(textures, NU_CUBEMAP_FACE_COUNT, NU_NULL);

    nu_scope_push();
    nu_size_t  json_size;
    nu_byte_t *json_buf = nu__seria_load_bytes(filename, &json_size);
    nu_str_t   json     = nu_str(json_buf, json_size);
    NU_CHECK(json_buf, goto cleanup);
    nu_size_t  toks_size, toks_count;
    jsmntok_t *toks = nu__json_parse(json, &toks_size, &toks_count);
    NU_CHECK(toks, goto cleanup);

    nu_str_t json_path = nuext_path_dirname(filename);

    nu_size_t texture_count = 0;
    NU_CHECK(toks[0].type == JSMN_OBJECT, goto cleanup);

    static nu_str_t faces[]
        = { NU_STR("posx"), NU_STR("negx"), NU_STR("posy"),
            NU_STR("negy"), NU_STR("posz"), NU_STR("negz") };
    for (nu_size_t f = 0; f < NU_ARRAY_SIZE(faces); ++f)
    {
        const jsmntok_t *tok = nu__json_object_member(json, &toks[0], faces[f]);
        if (!tok)
        {
            NU_ERROR("cubemap face not found '%s'", faces[f]);
            goto cleanup;
        }
        if (tok->type != JSMN_STRING)
        {
            NU_ERROR("invalid face path");
            goto cleanup;
        }
        if (!textures[f])
        {
            nu_str_t path = nu__json_value(json, tok);
            NU_STR_BUF(final_path_buf, NUEXT_PATH_MAX);
            nu_str_t final_path
                = nuext_path_concat(final_path_buf, json_path, path);
            textures[f]
                = nuext_texture_load_file(NU_TEXTURE_COLORMAP, final_path);
            if (!textures[f])
            {
                NU_ERROR("cubemap face loading error '%s'", path);
                goto cleanup;
            }
        }
    }

    nu_v3u_t size = nu_texture_size(textures[0]);
    cubemap       = nu_texture_new(
        NU_TEXTURE_CUBEMAP, nu_v3u(size.x, size.y, 1), NU_CUBEMAP_FACE_COUNT);
    for (nu_size_t i = 0; i < NU_CUBEMAP_FACE_COUNT; ++i)
    {
        nu_memcpy(nu_texture_data(cubemap, i),
                  nu_texture_data(textures[i], 0),
                  4 * size.x * size.y);
    }

cleanup:
    nu_scope_pop();
    return cubemap;
}

nu_texture_t
nuext_texture_load_file (nu_texture_type_t type, nu_str_t filename)
{
    if (type == NU_TEXTURE_CUBEMAP)
    {
        return nu__texture_load_cubemap(filename);
    }
    else
    {
#ifdef NU_BUILD_IMPORTER_STBIMAGE
        return nu__texture_load_file(filename);
#endif
    }
    return NU_NULL;
}
nu_texture_t
nuext_texture_load_memory (nu_texture_type_t type,
                           const nu_byte_t  *data,
                           nu_size_t         size)
{
    NU_ASSERT(type == NU_TEXTURE_COLORMAP);
#ifdef NU_BUILD_IMPORTER_STBIMAGE
    return nu__texture_load_memory(data, size);
#endif
    return NU_NULL;
}

#endif

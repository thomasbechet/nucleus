#ifndef NU_IMPORTER_IMAGE_H
#define NU_IMPORTER_IMAGE_H

#include <nucleus/internal.h>

#ifdef NU_BUILD_STBIMAGE
#define STB_IMAGE_IMPLEMENTATION
#include <nucleus/external/stb/stb_image.h>
#endif

static void
nu__parse_colors (const nu_byte_t *src,
                  nu_color_t      *dst,
                  nu_vec2u_t       size,
                  nu_size_t        comp)
{
    for (nu_size_t i = 0; i < (size.x * size.y); ++i)
    {
        for (nu_size_t n = 0; n < comp; ++n)
        {
        }
        dst[i].r = src[i * comp + 0];
        dst[i].g = src[i * comp + 1];
        dst[i].b = src[i * comp + 2];
        if (comp == 4)
        {
            dst[i].a = src[i * comp + 3];
        }
        else
        {
            dst[i].a = 255;
        }
    }
}
static nu_image_t
nu__image_load_filename (const nu_char_t *filename)
{
#ifdef NU_BUILD_STBIMAGE
    int        w, h, n;
    nu_byte_t *img = stbi_load(filename, &w, &h, &n, STBI_default);
    NU_CHECK(img, return NU_NULL);
    nu_vec2u_t size   = nu_vec2u(w, h);
    nu_image_t handle = nu_image_create(size);
    nu__parse_colors(img, nu_image_colors(handle), size, n);
    stbi_image_free(img);
    return handle;
#endif
    return NU_NULL;
}
static nu_image_t
nu__image_load_memory (const nu_byte_t *data, nu_size_t size)
{
#ifdef NU_BUILD_STBIMAGE
    int        w, h, n;
    nu_byte_t *img
        = stbi_load_from_memory(data, size, &w, &h, &n, STBI_default);
    NU_CHECK(img, return NU_NULL);
    nu_vec2u_t image_size = nu_vec2u(w, h);
    nu_image_t handle     = nu_image_create(image_size);
    nu__parse_colors(img, nu_image_colors(handle), image_size, n);
    stbi_image_free(img);
    return handle;
#endif
    return NU_NULL;
}

nu_image_t
nuext_image_load_filename (const nu_char_t *filename)
{
#ifdef NU_BUILD_STBIMAGE
    return nu__image_load_filename(filename);
#endif
    return NU_NULL;
}
nu_image_t
nuext_image_load_memory (const nu_byte_t *data, nu_size_t size)
{
#ifdef NU_BUILD_STBIMAGE
    return nu__image_load_memory(data, size);
#endif
    return NU_NULL;
}

#endif

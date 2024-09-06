#ifndef NU_IMAGE_IMPL_H
#define NU_IMAGE_IMPL_H

#include <nucleus/internal.h>

nu_image_t
nu_image_create (nu_uvec2_t size)
{
    nu_image_t   handle;
    nu__image_t *im = nu_pool_add(&_ctx.graphics.images, &handle._index);
    im->size        = size;
    im->colors = (nu_color_t *)nu_alloc(sizeof(nu_color_t) * size.x * size.y);
    return handle;
}
void
nu_image_delete (nu_image_t ima)
{
    nu__image_t *im = &_ctx.graphics.images.data[ima._index];
    nu_free(im->colors, sizeof(nu_color_t) * im->size.x * im->size.y);
}
nu_color_t *
nu_image_colors (nu_image_t image)
{
    return _ctx.graphics.images.data[image._index].colors;
}
nu_uvec2_t
nu_image_size (nu_image_t image)
{
    return _ctx.graphics.images.data[image._index].size;
}
nu_texture_info_t
nu_image_texture_info (nu_image_t image)
{
    nu_texture_info_t info;
    info.size   = nu_image_size(image);
    info.colors = nu_image_colors(image);
    info.usage  = NU_TEXTURE_USAGE_SAMPLE;
    info.format = NU_TEXTURE_FORMAT_COLOR;
    return info;
}

#endif

#ifndef NU_IMAGE_IMPL_H
#define NU_IMAGE_IMPL_H

#include <nucleus/internal.h>

nu_image_t
nu_image_create (nu_uvec2_t size)
{
    nu_size_t    index;
    nu__image_t *im = NU_POOL_ADD(&_ctx.graphics.images, &index);
    im->size        = size;
    im->colors = (nu_color_t *)nu_alloc(sizeof(nu_color_t) * size.x * size.y);
    return nu_handle_make(nu_image_t, index);
}
void
nu_image_delete (nu_image_t ima)
{
    nu__image_t *im = &_ctx.graphics.images.data[nu_handle_index(ima)];
    nu_free(im->colors, sizeof(nu_color_t) * im->size.x * im->size.y);
}
nu_color_t *
nu_image_colors (nu_image_t image)
{
    return _ctx.graphics.images.data[nu_handle_index(image)].colors;
}
nu_uvec2_t
nu_image_size (nu_image_t image)
{
    return _ctx.graphics.images.data[nu_handle_index(image)].size;
}

#endif

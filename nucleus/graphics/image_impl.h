#ifndef NU_IMAGE_IMPL_H
#define NU_IMAGE_IMPL_H

#include <nucleus/internal.h>

nu_image_t
nu_image_create (nu_vec2u_t size)
{
    nu_size_t    index;
    nu__image_t *im = NU_POOL_ADD(&_ctx.graphics.images, &index);
    im->size        = size;
    im->colors = (nu_color_t *)nu_alloc(sizeof(nu_color_t) * size.x * size.y);
    return NU_HANDLE_MAKE(nu_image_t, index);
}
void
nu_image_delete (nu_image_t ima)
{
    nu__image_t *im = &_ctx.graphics.images.data[NU_HANDLE_INDEX(ima)];
    nu_free(im->colors, sizeof(nu_color_t) * im->size.x * im->size.y);
}
nu_color_t *
nu_image_colors (nu_image_t image)
{
    return _ctx.graphics.images.data[NU_HANDLE_INDEX(image)].colors;
}
nu_vec2u_t
nu_image_size (nu_image_t image)
{
    return _ctx.graphics.images.data[NU_HANDLE_INDEX(image)].size;
}

#endif

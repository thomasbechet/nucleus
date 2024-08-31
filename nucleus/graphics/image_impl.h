#ifndef NU_IMAGE_IMPL_H
#define NU_IMAGE_IMPL_H

#include <nucleus/graphics/image.h>

nu_error_t
nu_image_init (nu_allocator_t alloc, nu_uvec2_t size, nu_image_t *ima)
{
    ima->size = size;
    ima->data
        = (nu_color_t *)nu_alloc(alloc, sizeof(nu_color_t) * size.x * size.y);
    return NU_ERROR_NONE;
}
void
nu_image_free (nu_image_t *ima, nu_allocator_t alloc)
{
    nu_free(alloc, ima->data, sizeof(nu_color_t) * ima->size.x * ima->size.y);
}

#endif

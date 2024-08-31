#ifndef NU_IMAGE_IMPL_H
#define NU_IMAGE_IMPL_H

#include <nucleus/graphics/image.h>

nu_error_t
nu_image_create (nu_allocator_t alloc, nu_uvec2_t size, nu_image_t *ima)
{
    ima->_ptr            = (nu__image_t *)nu_alloc(alloc, sizeof(*ima->_ptr));
    ima->_ptr->allocator = alloc;
    ima->_ptr->size      = size;
    ima->_ptr->colors
        = (nu_color_t *)nu_alloc(alloc, sizeof(nu_color_t) * size.x * size.y);
    return NU_ERROR_NONE;
}
void
nu_image_delete (nu_image_t ima)
{
    nu_free(ima._ptr->allocator,
            ima._ptr->colors,
            sizeof(nu_color_t) * ima._ptr->size.x * ima._ptr->size.y);
    nu_free(ima._ptr->allocator, ima._ptr, sizeof(*ima._ptr));
}
nu_color_t *
nu_image_colors (nu_image_t image)
{
    return image._ptr->colors;
}
nu_uvec2_t
nu_image_size (nu_image_t image)
{
    return image._ptr->size;
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

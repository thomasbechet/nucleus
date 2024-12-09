#ifndef NU_IMAGE_IMPL_H
#define NU_IMAGE_IMPL_H

#include <nucleus/internal.h>

nu_image_t
nu_image_new (nu_image_type_t type, nu_v3u_t size, nu_size_t layer)
{
    NU_ASSERT(size.x && size.y && size.z && layer);
    nu__image_t *im = nu_object_new(_ctx.graphics.obj_image);
    im->type        = type;
    im->size        = size;
    im->layer       = layer;
    im->data = (nu_byte_t *)nu_malloc(4 * size.x * size.y * size.z * layer);
    return (nu_image_t)im;
}

#endif

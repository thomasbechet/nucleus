#ifndef NU_IMAGE_IMPL_H
#define NU_IMAGE_IMPL_H

#include <nucleus/internal.h>

static void
nu__image_handler (nu_object_hook_t hook, void *data)
{
    switch (hook)
    {
        case NU_OBJECT_CLEANUP:
            break;
        case NU_OBJECT_SAVE:
        case NU_OBJECT_LOAD:
            break;
    }
}
nu_image_t
nu_image_new (nu_image_type_t type, nu_v3u_t size, nu_size_t layer)
{
    NU_ASSERT(size.x && size.y && size.z && layer);
    nu__image_t *im = nu_object_new(_ctx.graphics.obj_image);
    im->type        = type;
    im->size        = size;
    im->layer       = layer;
    im->data
        = (nu_byte_t *)nu_scope_alloc(4 * size.x * size.y * size.z * layer);
    return (nu_image_t)im;
}
nu_byte_t *
nu_image_data (nu_image_t image, nu_size_t layer)
{
    nu__image_t *im = (nu__image_t *)image;
    NU_ASSERT(layer < im->layer);
    const nu_size_t offset = 4 * im->size.x * im->size.y * im->size.z * layer;
    return im->data + offset;
}
nu_v3u_t
nu_image_size (nu_image_t image)
{
    nu__image_t *im = (nu__image_t *)image;
    return im->size;
}
nu_image_t
nu_image_load (nu_seria_t seria)
{
    nu_u32_t x     = nu_seria_read_1u32(seria);
    nu_u32_t y     = nu_seria_read_1u32(seria);
    nu_u32_t z     = nu_seria_read_1u32(seria);
    nu_u32_t layer = nu_seria_read_1u32(seria);
    NU_ASSERT(layer);
    nu_image_t   image = nu_image_new(NU_IMAGE_RGBA, nu_v3u(x, y, z), layer);
    nu__image_t *im    = (nu__image_t *)image;
    nu_seria_read_byte(seria, 4 * x * y * z * layer, im->data);
    return image;
}
void
nu_image_save (nu_image_t image, nu_seria_t seria)
{
    nu__image_t *ima = (nu__image_t *)image;
    nu_seria_write_1u32(seria, ima->size.x);
    nu_seria_write_1u32(seria, ima->size.y);
    nu_seria_write_1u32(seria, ima->size.z);
    nu_seria_write_1u32(seria, ima->layer);
    nu_seria_write_byte(seria,
                        4 * ima->size.x * ima->size.y * ima->size.z
                            * ima->layer,
                        ima->data);
}

#endif

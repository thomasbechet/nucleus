#ifndef NU_IMAGE_IMPL_H
#define NU_IMAGE_IMPL_H

#include <nucleus/internal.h>

nu_image_t
nu_image_create (nu_image_type_t type, nu_v3u_t size, nu_size_t layer)
{
    NU_ASSERT(size.x && size.y && size.z && layer);
    nu_size_t    index;
    nu__image_t *im = NU_POOL_ADD(&_ctx.graphics.images, &index);
    im->type        = type;
    im->size        = size;
    im->layer       = layer;
    im->data = (nu_byte_t *)nu_alloc(4 * size.x * size.y * size.z * layer);
    return NU_HANDLE_MAKE(nu_image_t, index);
}
void
nu_image_delete (nu_image_t ima)
{
    nu__image_t *im = &_ctx.graphics.images.data[NU_HANDLE_INDEX(ima)];
    nu_free(im->data, 4 * im->size.x * im->size.y * im->size.z * im->layer);
}
nu_byte_t *
nu_image_data (nu_image_t image, nu_size_t layer)
{
    nu__image_t *im = &_ctx.graphics.images.data[NU_HANDLE_INDEX(image)];
    NU_ASSERT(layer < im->layer);
    const nu_size_t offset = 4 * im->size.x * im->size.y * im->size.z * layer;
    return im->data + offset;
}
nu_v3u_t
nu_image_size (nu_image_t image)
{
    return _ctx.graphics.images.data[NU_HANDLE_INDEX(image)].size;
}
nu_image_t
nu_image_load (nu_seria_t seria)
{
    nu_u32_t x     = nu_seria_read_u32(seria);
    nu_u32_t y     = nu_seria_read_u32(seria);
    nu_u32_t z     = nu_seria_read_u32(seria);
    nu_u32_t layer = nu_seria_read_u32(seria);
    NU_ASSERT(layer);
    nu_image_t   image = nu_image_create(NU_IMAGE_RGBA, nu_v3u(x, y, z), layer);
    nu__image_t *im    = &_ctx.graphics.images.data[NU_HANDLE_INDEX(image)];
    nu_seria_read(seria, NU_SERIA_BYTE, 4 * x * y * z * layer, im->data);
    return image;
}
void
nu_image_save (nu_image_t image, nu_seria_t seria)
{
    nu__image_t *ima = &_ctx.graphics.images.data[NU_HANDLE_INDEX(image)];
    nu_seria_write_u32(seria, ima->size.x);
    nu_seria_write_u32(seria, ima->size.y);
    nu_seria_write_u32(seria, ima->size.z);
    nu_seria_write_u32(seria, ima->layer);
    nu_seria_write(seria,
                   NU_SERIA_BYTE,
                   4 * ima->size.x * ima->size.y * ima->size.z * ima->layer,
                   ima->data);
}

#ifdef NU_BUILD_RESOURCE
nu_image_t
nu_image (nu_uid_t uid)
{
    return nu_resource_data(NU_UID(NU_RESOURCE_IMAGE), uid);
}
static void *
nu__image_resource_load (nu_seria_t seria)
{
    return nu_image_load(seria);
}
static void
nu__image_resource_unload (void *data)
{
    nu_image_delete(data);
}
static void
nu__image_resource_save (void *data, nu_seria_t seria)
{
    nu_image_save(data, seria);
}
static void
nu__image_resource_register (void)
{
    nu_resource_register(NU_UID(NU_RESOURCE_IMAGE),
                         nu__image_resource_load,
                         nu__image_resource_unload,
                         nu__image_resource_save);
}
#endif

#endif

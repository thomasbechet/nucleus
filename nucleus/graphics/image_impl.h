#ifndef NU_IMAGE_IMPL_H
#define NU_IMAGE_IMPL_H

#include <nucleus/internal.h>

nu_image_t
nu_image_create (nu_v2u_t size)
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
nu_v2u_t
nu_image_size (nu_image_t image)
{
    return _ctx.graphics.images.data[NU_HANDLE_INDEX(image)].size;
}
nu_image_t
nu_image_load (nu_seria_t seria)
{
    nu_u32_t x, y;
    nu_seria_read(seria, NU_SERIA_U32, 1, &x);
    nu_seria_read(seria, NU_SERIA_U32, 1, &y);
    nu_image_t   image = nu_image_create(nu_v2u(x, y));
    nu__image_t *im    = &_ctx.graphics.images.data[NU_HANDLE_INDEX(image)];
    nu_seria_read(seria, NU_SERIA_BYTE, 4 * x * y, im->colors);
    return image;
}
void
nu_image_save (nu_image_t image, nu_seria_t seria)
{
    nu__image_t *ima = &_ctx.graphics.images.data[NU_HANDLE_INDEX(image)];
    nu_seria_write(seria, NU_SERIA_U32, 1, &ima->size.x);
    nu_seria_write(seria, NU_SERIA_U32, 1, &ima->size.y);
    nu_seria_write(
        seria, NU_SERIA_BYTE, 4 * ima->size.x * ima->size.y, ima->colors);
}

#ifdef NU_BUILD_RESOURCE
nu_image_t
nu_image_resource (nu_uid_t uid)
{
    return nu_resource_data(_ctx.graphics.res_image, uid);
}
static void
nu__image_resource_removed (void *data)
{
    nu_image_delete(data);
}
static void *
nu__image_resource_load (nu_seria_t seria)
{
    return nu_image_load(seria);
}
static void
nu__image_resource_save (void *data, nu_seria_t seria)
{
    nu_image_save(data, seria);
}
static void
nu__image_resource_register (void)
{
    _ctx.graphics.res_image = nu_resource_register(NU_UID("image"),
                                                   NU_NULL,
                                                   nu__image_resource_removed,
                                                   nu__image_resource_load,
                                                   nu__image_resource_save);
}
#endif

#endif

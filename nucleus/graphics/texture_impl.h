#ifndef NU_TEXTURE_IMPL_H
#define NU_TEXTURE_IMPL_H

#include <nucleus/internal.h>
#include <nucleus/graphics/backend_impl.h>

static void
nu__texture_cleanup (void *data)
{
    nu__texture_t *tex = data;
#ifdef NU_BUILD_GRAPHICS_GL
    nugl__texture_free(tex);
#endif
}
nu_object_type_t
nu_texture (void)
{
    return _ctx.graphics.obj_texture;
}
nu_texture_t
nu_texture_new (nu_texture_type_t type, nu_v3u_t size, nu_size_t layer)
{
    NU_ASSERT(size.x && size.y && size.z && layer);
    nu__texture_t *p      = nu_object_new(nu_texture());
    nu_texture_t   handle = (nu_texture_t)p;

    p->type  = type;
    p->size  = size;
    p->layer = layer;
    p->data  = nu_malloc(4 * size.x * size.y * size.z * layer);
#ifdef NU_BUILD_GRAPHICS_GL
    nugl__texture_init(p);
#endif
    return handle;
}
nu_texture_t
nu_texture_new_from_color (nu_color_t color)
{
    nu_texture_t tex = nu_texture_new(NU_TEXTURE_COLORMAP, NU_V3U_ONES, 1);
    NU_CHECK(tex, return tex);
    nu_byte_t data[] = { color.r, color.g, color.b, color.a };
    nu_texture_set_data(tex, 0, data);
    return tex;
}
// nu_texture_t
// nu_texture_new_from_image (nu_texture_type_t type, nu_image_t image)
// {
//     NU_ASSERT(image);
//     nu__image_t *ima = (nu__image_t *)image;
//     nu_texture_t tex = NU_NULL;
//     switch (type)
//     {
//         case NU_TEXTURE_COLORMAP:
//         case NU_TEXTURE_COLORMAP_TARGET:
//             tex = nu_texture_new(type, ima->size, ima->layer);
//             for (nu_size_t i = 0; i < ima->layer; ++i)
//             {
//                 nu_texture_set_data(tex, i, nu_image_data(image, i));
//             }
//             break;
//         case NU_TEXTURE_DEPTHBUFFER_TARGET:
//             NU_UNREACHABLE();
//             break;
//         case NU_TEXTURE_SHADOWMAP_TARGET:
//             NU_UNREACHABLE();
//             break;
//         case NU_TEXTURE_CUBEMAP:
//             NU_ASSERT(ima->layer == NU_CUBEMAP_FACE_COUNT);
//             tex = nu_texture_new(type, ima->size, NU_CUBEMAP_FACE_COUNT);
//             for (nu_size_t i = 0; i < NU_CUBEMAP_FACE_COUNT; ++i)
//             {
//                 nu_texture_set_data(tex, i, nu_image_data(image, i));
//             }
//             break;
//     }
//     return tex;
// }
void
nu_texture_set_data (nu_texture_t     texture,
                     nu_size_t        layer,
                     const nu_byte_t *data)
{
    nu__texture_t *tex = (nu__texture_t *)texture;
    nu_memcpy(nu_texture_data(texture, layer),
              data,
              4 * tex->size.x * tex->size.y * tex->size.z);
#ifdef NU_BUILD_GRAPHICS_GL
    nugl__texture_upload(tex, layer);
#endif
}
nu_v3u_t
nu_texture_size (nu_texture_t texture)
{
    nu__texture_t *tex = (nu__texture_t *)texture;
    return tex->size;
}
nu_texture_type_t
nu_texture_type (nu_texture_t texture)
{
    nu__texture_t *tex = (nu__texture_t *)texture;
    return tex->type;
}
nu_byte_t *
nu_texture_data (nu_texture_t texture, nu_size_t layer)
{
    nu__texture_t *tex = (nu__texture_t *)texture;
    NU_ASSERT(layer < tex->layer);
    return tex->data + 4 * tex->size.x * tex->size.y * tex->size.z * layer;
}
void
nu_texture_upload (nu_texture_t texture)
{
    nu__texture_t *tex = (nu__texture_t *)texture;
    nugl__texture_upload(tex, 0);
}
nu_texture_t
nu_texture_load (nu_seria_t seria)
{
    nu_u32_t x, y, z, layer;
    nu_seria_read_u32(seria, 1, &x);
    nu_seria_read_u32(seria, 1, &y);
    nu_seria_read_u32(seria, 1, &z);
    nu_seria_read_u32(seria, 1, &layer);
    NU_ASSERT(layer);
    nu_texture_t texture
        = nu_texture_new(NU_TEXTURE_COLORMAP, nu_v3u(x, y, z), layer);
    nu_seria_read_byte(
        seria, 4 * x * y * z * layer, nu_texture_data(texture, 0));
    return texture;
}
void
nu_texture_save (nu_texture_t texture, nu_seria_t seria)
{
    nu__texture_t *t = (nu__texture_t *)texture;
    nu_seria_write_u32(seria, 1, &t->size.x);
    nu_seria_write_u32(seria, 1, &t->size.y);
    nu_seria_write_u32(seria, 1, &t->size.z);
    nu_seria_write_u32(seria, 1, &t->layer);
    nu_seria_write_byte(
        seria, 4 * t->size.x * t->size.y * t->size.z * t->layer, t->data);
}

#endif

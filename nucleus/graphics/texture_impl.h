#ifndef NU_TEXTURE_IMPL_H
#define NU_TEXTURE_IMPL_H

#include <nucleus/internal.h>
#include <nucleus/graphics/backend_impl.h>

static void
nu__texture_cleanup (void *data)
{
    nugfx__texture_t *tex = data;
#ifdef NU_BUILD_GRAPHICS_GL
    nugl__texture_free(tex);
#endif
}
nugfx_texture_t
nugfx_texture_new (nu_v3u_t size)
{
    nugfx__texture_t *texture = nu_object_new(_ctx.graphics.obj_texture);
    texture->size             = size;
    texture->data             = nu_malloc(4 * size.x * size.y * size.z);
#ifdef NU_BUILD_GRAPHICS_GL
    nugl__texture_init(texture);
#endif
    return (nugfx_texture_t)texture;
}
nu_byte_t *
nugfx_texture_data (nugfx_texture_t texture)
{
    return ((nugfx__texture_t *)texture)->data;
}
void
nugfx_texture_upload (nugfx_texture_t texture)
{
    nugfx__texture_t *tex = (nugfx__texture_t *)texture;
    NU_ASSERT(tex->data);
#ifdef NU_BUILD_GRAPHICS_GL
    nugl__texture_upload(tex);
#endif
}
nugfx_texture_t
nugfx__load_texture (nu_seria_t seria)
{
    nu_u32_t x, y, z, layer;
    nu_seria_read_u32(seria, 1, &x);
    nu_seria_read_u32(seria, 1, &y);
    nu_seria_read_u32(seria, 1, &z);
    nu_seria_read_u32(seria, 1, &layer);
    NU_ASSERT(layer);
    nugfx_texture_t texture = nugfx_texture_new(nu_v3u(x, y, z));
    nu_seria_read_byte(
        seria, 4 * x * y * z * layer, nugfx_texture_data(texture));
    return texture;
}
void
nugfx__save_texture (nugfx_texture_t texture, nu_seria_t seria)
{
    nugfx__texture_t *t = (nugfx__texture_t *)texture;
    nu_seria_write_u32(seria, 1, &t->size.x);
    nu_seria_write_u32(seria, 1, &t->size.y);
    nu_seria_write_u32(seria, 1, &t->size.z);
    nu_seria_write_byte(seria, 4 * t->size.x * t->size.y * t->size.z, t->data);
}

#endif

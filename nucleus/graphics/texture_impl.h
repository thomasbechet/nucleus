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
nugfx_texture_t
nugfx__load_texture (nu_seria_t seria)
{
    nu_u32_t x, y, z, layer;
    nu_seria_read_u32(seria, 1, &x);
    nu_seria_read_u32(seria, 1, &y);
    nu_seria_read_u32(seria, 1, &z);
    nu_seria_read_u32(seria, 1, &layer);
    NU_ASSERT(layer);
    nugfx_texture_t texture = nugfx_new_texture(nu_v3u(x, y, z));
    nu_seria_read_byte(
        seria, 4 * x * y * z * layer, nu_texture_data(texture, 0));
    return texture;
}
void
nugfx__save_texture (nugfx_texture_t texture, nu_seria_t seria)
{
    nu__texture_t *t = (nu__texture_t *)texture;
    nu_seria_write_u32(seria, 1, &t->size.x);
    nu_seria_write_u32(seria, 1, &t->size.y);
    nu_seria_write_u32(seria, 1, &t->size.z);
    nu_seria_write_byte(seria, 4 * t->size.x * t->size.y * t->size.z, t->data);
}

#endif

#ifndef NU_TEXTURE_IMPL_H
#define NU_TEXTURE_IMPL_H

#include <nucleus/internal.h>

nu_texture_t
nu_texture_create (nu_texture_type_t type, nu_v3u_t size)
{
    nu_size_t      index;
    nu__texture_t *ptex   = NU_POOL_ADD(&_ctx.graphics.textures, &index);
    nu_texture_t   handle = NU_HANDLE_MAKE(nu_texture_t, index);

    ptex->type = type;
    ptex->size = size;
#ifdef NU_BUILD_GRAPHICS_GL
    nugl__texture_init(ptex);
#endif
    return handle;
}
void
nu_texture_delete (nu_texture_t texture)
{
    nu__texture_t *tex = _ctx.graphics.textures.data + NU_HANDLE_INDEX(texture);
#ifdef NU_BUILD_GRAPHICS_GL
    nugl__texture_free(tex);
#endif
    NU_POOL_REMOVE(&_ctx.graphics.textures, NU_HANDLE_INDEX(texture));
}
void
nu_texture_write_colors (nu_texture_t texture, const nu_color_t *colors)
{
    nu__texture_t *tex = _ctx.graphics.textures.data + NU_HANDLE_INDEX(texture);
#ifdef NU_BUILD_GRAPHICS_GL
    nugl__texture_write_colors(tex, colors);
#endif
}
void
nu_texture_write_cubemap_colors (nu_texture_t      cubemap,
                                 nu_cubemap_face_t face,
                                 const nu_color_t *colors)
{
    nu__texture_t *tex = _ctx.graphics.textures.data + NU_HANDLE_INDEX(cubemap);
#ifdef NU_BUILD_GRAPHICS_GL
    nugl__texture_write_cubemap_colors(tex, face, colors);
#endif
}

#endif

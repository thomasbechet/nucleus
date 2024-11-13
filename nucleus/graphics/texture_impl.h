#ifndef NU_TEXTURE_IMPL_H
#define NU_TEXTURE_IMPL_H

#include <nucleus/internal.h>
#include <nucleus/graphics/backend_impl.h>

nu_texture_t
nu_texture_create (nu_texture_type_t type, nu_v3u_t size, nu_size_t layer)
{
    NU_ASSERT(size.x && size.y && size.z && layer);
    nu_size_t      index;
    nu__texture_t *ptex   = NU_POOL_ADD(&_ctx.graphics.textures, &index);
    nu_texture_t   handle = NU_HANDLE_MAKE(nu_texture_t, index);

    ptex->type          = type;
    ptex->size          = size;
    ptex->layer         = layer;
    ptex->image_texture = NU_NULL;
#ifdef NU_BUILD_GRAPHICS_GL
    nugl__texture_init(ptex);
#endif
    return handle;
}
nu_texture_t
nu_texture_create_from_color (nu_color_t color)
{
    nu_texture_t tex = nu_texture_create(NU_TEXTURE_COLORMAP, NU_V3U_ONES, 1);
    NU_CHECK(tex, return tex);
    nu_byte_t data[] = { color.r, color.g, color.b, color.a };
    nu_texture_set_data(tex, 0, data);
    return tex;
}
nu_texture_t
nu_texture_create_from_image (nu_texture_type_t type, nu_image_t image)
{
    nu__image_t *ima = &_ctx.graphics.images.data[NU_HANDLE_INDEX(image)];
    nu_texture_t tex = NU_NULL;
    switch (type)
    {
        case NU_TEXTURE_COLORMAP:
        case NU_TEXTURE_COLORMAP_TARGET:
            tex = nu_texture_create(type, ima->size, ima->layer);
            for (nu_size_t i = 0; i < ima->layer; ++i)
            {
                nu_texture_set_data(tex, i, nu_image_data(image, i));
            }
            break;
        case NU_TEXTURE_DEPTHBUFFER_TARGET:
            NU_UNREACHABLE();
            break;
        case NU_TEXTURE_SHADOWMAP_TARGET:
            NU_UNREACHABLE();
            break;
        case NU_TEXTURE_CUBEMAP:
            NU_ASSERT(ima->layer == NU_CUBEMAP_FACE_COUNT);
            tex = nu_texture_create(type, ima->size, NU_CUBEMAP_FACE_COUNT);
            for (nu_size_t i = 0; i < NU_CUBEMAP_FACE_COUNT; ++i)
            {
                nu_texture_set_data(tex, i, nu_image_data(image, i));
            }
            break;
    }
    return tex;
}
nu_texture_t
nu_texture_create_image_texture (nu_texture_type_t type, nu_image_t image)
{
    nu_texture_t tex = nu_texture_create_from_image(type, image);
    _ctx.graphics.textures.data[NU_HANDLE_INDEX(tex)].image_texture = image;
    return tex;
}
void
nu_texture_delete (nu_texture_t texture)
{
    nu__texture_t *tex = _ctx.graphics.textures.data + NU_HANDLE_INDEX(texture);
    if (tex->image_texture) // is image_texture
    {
        nu_image_delete(tex->image_texture);
    }
#ifdef NU_BUILD_GRAPHICS_GL
    nugl__texture_free(tex);
#endif
    NU_POOL_REMOVE(&_ctx.graphics.textures, NU_HANDLE_INDEX(texture));
}
void
nu_texture_set_data (nu_texture_t     texture,
                     nu_size_t        layer,
                     const nu_byte_t *data)
{
    nu__texture_t *tex = _ctx.graphics.textures.data + NU_HANDLE_INDEX(texture);
    NU_ASSERT(!tex->image_texture);
#ifdef NU_BUILD_GRAPHICS_GL
    nugl__texture_set_data(tex, layer, data);
#endif
}
nu_texture_type_t
nu_texture_type (nu_texture_t texture)
{
    nu__texture_t *tex = _ctx.graphics.textures.data + NU_HANDLE_INDEX(texture);
    return tex->type;
}

#endif

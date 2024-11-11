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

    ptex->type  = type;
    ptex->size  = size;
    ptex->layer = layer;
    ptex->image = NU_NULL;
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
    nu__texture_t *tex_data
        = _ctx.graphics.textures.data + NU_HANDLE_INDEX(tex);
    tex_data->image = image;
    return tex;
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
nu_texture_set_data (nu_texture_t     texture,
                     nu_size_t        layer,
                     const nu_byte_t *data)
{
    nu__texture_t *tex = _ctx.graphics.textures.data + NU_HANDLE_INDEX(texture);
    NU_ASSERT(!tex->image);
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

#ifdef NU_BUILD_RESOURCE
nu_texture_t
nu_image_texture (nu_uid_t uid)
{
    return nu_resource_data(_ctx.graphics.res_image_texture, uid);
}
static void
nu__image_texture_resource_removed (void *data)
{
    nu_texture_delete(data);
}
static void *
nu__image_texture_resource_load (nu_seria_t seria)
{
    nu_texture_type_t type = NU_TEXTURE_COLORMAP;
    if (nu_seria_read_u32(seria))
    {
        type = NU_TEXTURE_CUBEMAP;
    }
    nu_image_t image = nu_image_load(seria);
    NU_ASSERT(image);
    nu_texture_t   texture = nu_texture_create_from_image(type, image);
    nu__texture_t *tex = _ctx.graphics.textures.data + NU_HANDLE_INDEX(texture);
    tex->image         = image;
    return texture;
}
static void
nu__image_texture_resource_save (void *data, nu_seria_t seria)
{
    nu_texture_t   texture = data;
    nu__texture_t *tex = _ctx.graphics.textures.data + NU_HANDLE_INDEX(texture);
    nu_seria_write_u32(seria, tex->type == NU_TEXTURE_CUBEMAP ? 1 : 0);
    NU_ASSERT(tex->image);
    nu_image_save(tex->image, seria);
}
static void
nu__image_texture_resource_register (void)
{
    _ctx.graphics.res_image_texture
        = nu_resource_register(NU_UID("image_texture"),
                               NU_NULL,
                               nu__image_texture_resource_removed,
                               nu__image_texture_resource_load,
                               nu__image_texture_resource_save);
}
#endif

#endif

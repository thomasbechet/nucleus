#ifndef NU_TEXTURE_IMPL_H
#define NU_TEXTURE_IMPL_H

#include <nucleus/internal.h>
#include <nucleus/graphics/backend_impl.h>

static void
nu__texture_handler (nu_object_hook_t hook, void *data)
{
    switch (hook)
    {
        case NU_OBJECT_CLEANUP: {
            nu__texture_t *tex = data;
#ifdef NU_BUILD_GRAPHICS_GL
            nugl__texture_free(tex);
#endif
        }
        break;
        case NU_OBJECT_SAVE:
        case NU_OBJECT_LOAD:
            break;
    }
}
nu_texture_t
nu_texture_new (nu_scope_t        scope,
                nu_texture_type_t type,
                nu_v3u_t          size,
                nu_size_t         layer)
{
    NU_ASSERT(size.x && size.y && size.z && layer);
    nu__texture_t *ptex   = nu_object_new(scope, _ctx.graphics.obj_texture);
    nu_texture_t   handle = (nu_texture_t)ptex;

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
nu_texture_new_from_color (nu_scope_t scope, nu_color_t color)
{
    nu_texture_t tex
        = nu_texture_new(scope, NU_TEXTURE_COLORMAP, NU_V3U_ONES, 1);
    NU_CHECK(tex, return tex);
    nu_byte_t data[] = { color.r, color.g, color.b, color.a };
    nu_texture_set_data(tex, 0, data);
    return tex;
}
nu_texture_t
nu_texture_new_from_image (nu_scope_t        scope,
                           nu_texture_type_t type,
                           nu_image_t        image)
{
    NU_ASSERT(image);
    nu__image_t *ima = (nu__image_t *)image;
    nu_texture_t tex = NU_NULL;
    switch (type)
    {
        case NU_TEXTURE_COLORMAP:
        case NU_TEXTURE_COLORMAP_TARGET:
            tex = nu_texture_new(scope, type, ima->size, ima->layer);
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
            tex = nu_texture_new(scope, type, ima->size, NU_CUBEMAP_FACE_COUNT);
            for (nu_size_t i = 0; i < NU_CUBEMAP_FACE_COUNT; ++i)
            {
                nu_texture_set_data(tex, i, nu_image_data(image, i));
            }
            break;
    }
    return tex;
}
nu_texture_t
nu_texture_new_image_texture (nu_scope_t        scope,
                              nu_texture_type_t type,
                              nu_image_t        image)
{
    nu_texture_t   tex  = nu_texture_new_from_image(scope, type, image);
    nu__texture_t *ptex = (nu__texture_t *)tex;
    ptex->image         = image;
    return tex;
}
void
nu_texture_set_data (nu_texture_t     texture,
                     nu_size_t        layer,
                     const nu_byte_t *data)
{
    nu__texture_t *tex = (nu__texture_t *)texture;
    NU_ASSERT(!tex->image);
#ifdef NU_BUILD_GRAPHICS_GL
    nugl__texture_set_data(tex, layer, data);
#endif
}
nu_texture_type_t
nu_texture_type (nu_texture_t texture)
{
    nu__texture_t *tex = (nu__texture_t *)texture;
    return tex->type;
}
nu_image_t
nu_texture_image (nu_texture_t texture)
{
    nu__texture_t *tex = (nu__texture_t *)texture;
    return tex->image;
}

#endif

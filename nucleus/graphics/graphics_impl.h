#ifndef NU_GRAPHICS_IMPL_H
#define NU_GRAPHICS_IMPL_H

#include <nucleus/graphics/renderer_impl.h>
#include <nucleus/graphics/model_impl.h>
#include <nucleus/graphics/font_impl.h>
#include <nucleus/graphics/image_impl.h>

static nu_error_t
nu__graphics_init (void)
{
    NU_POOL_INIT(10, &_ctx.graphics.fonts);
    NU_POOL_INIT(10, &_ctx.graphics.images);
    NU_POOL_INIT(10, &_ctx.graphics.models);

    nu__renderer_t *renderer = &_ctx.graphics.renderer;
    renderer->null_api       = NU_FALSE;
    switch (_ctx.config.graphics.api)
    {
        case NU_RENDERER_NULL:
            nu_memset(&renderer->api, 0, sizeof(renderer->api));
            renderer->null_api = NU_TRUE;
            break;
        case NU_RENDERER_GL:
            nugl__setup_api(&renderer->api);
            break;
        case NU_RENDERER_DX11:
            break;
        case NU_RENDERER_SOFTRAST:
            break;
    }

    // Initialize backend
    NU_INFO("initialize renderer context");
    if (!renderer->null_api)
    {
        nu_error_t error = renderer->api.init();
        NU_ERROR_CHECK(error, return error);
    }

    // Create surface texture
    if (!renderer->null_api)
    {
        renderer->surface_color
            = renderer->api.create_surface_color(_ctx.platform.size);
    }

    return NU_ERROR_NONE;
}
static nu_error_t
nu__graphics_free (void)
{
    nu__renderer_t *renderer = &_ctx.graphics.renderer;
    NU_INFO("terminate renderer context");
    if (!renderer->null_api)
    {
        renderer->api.free();
    }
    // TODO: free fonts and models
    NU_POOL_FREE(&_ctx.graphics.models);
    NU_POOL_FREE(&_ctx.graphics.images);
    NU_POOL_FREE(&_ctx.graphics.fonts);
    return NU_ERROR_NONE;
}
static nu_error_t
nu__graphics_render (void)
{
    nu__renderer_t *renderer = &_ctx.graphics.renderer;
    if (!renderer->null_api)
    {
        return _ctx.graphics.renderer.api.render(
            &_ctx.platform.viewport.extent, &_ctx.platform.viewport.viewport);
    }
    return NU_ERROR_NONE;
}

#endif

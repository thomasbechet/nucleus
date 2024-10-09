#ifndef NU_GRAPHICS_IMPL_H
#define NU_GRAPHICS_IMPL_H

#include <nucleus/graphics/renderer_impl.h>
#include <nucleus/graphics/font_impl.h>
#include <nucleus/graphics/image_impl.h>
#include <nucleus/graphics/model_impl.h>
#include <nucleus/graphics/immediate_impl.h>

static nu_error_t
nu__graphics_init (void)
{
    NU_POOL_INIT(10, &_ctx.graphics.fonts);
    NU_POOL_INIT(10, &_ctx.graphics.images);
    NU_POOL_INIT(10, &_ctx.graphics.models);

    // Initialize backend
    NU_INFO("initialize renderer context");
    nu__renderer_init();

    // Create surface texture
    _ctx.graphics.surface_color
        = nugl__create_surface_color(_ctx.platform.size);

    // Initialize immediate context
    nu__graphics_immediate_init();

    return NU_ERROR_NONE;
}
static nu_error_t
nu__graphics_free (void)
{
    // Terminate immediate context
    nu__graphics_immediate_free();

    NU_INFO("terminate renderer context");
    nu__renderer_free();

    // TODO: free fonts and models
    NU_POOL_FREE(&_ctx.graphics.models);
    NU_POOL_FREE(&_ctx.graphics.images);
    NU_POOL_FREE(&_ctx.graphics.fonts);
    return NU_ERROR_NONE;
}
static nu_error_t
nu__graphics_render (void)
{
    nu__renderer_render(_ctx.platform.viewport.extent,
                        _ctx.platform.viewport.viewport);
    nu__graphics_immediate_reset();
    return NU_ERROR_NONE;
}

#endif

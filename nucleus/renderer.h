#ifndef NU_RENDERER_H
#define NU_RENDERER_H

#include <nucleus/types.h>

NU_API void       nu_clear(nu_context_t *ctx);
NU_API nu_error_t nu_render(nu_context_t *ctx);

#ifdef NU_IMPLEMENTATION

#ifdef NU_BUILD_RENDERER_GL
#include <nucleus/backend/gl.h>
#endif

static nu_error_t
nu__renderer_null_init (void *ctx)
{
    return NU_ERROR_NONE;
}
static nu_error_t
nu__renderer_null_clear (void)
{
    return NU_ERROR_NONE;
}
static nu_error_t
nu__renderer_null_render (void           *ctx,
                          const nu_int_t *global_viewport,
                          const float    *viewport)
{
    NU_UNUSED(ctx);
    NU_UNUSED(viewport);
    return NU_ERROR_NONE;
}

static nu_error_t
nu__init_renderer (nu_context_t *ctx)
{
    switch (ctx->_renderer_backend)
    {
        case NU_RENDERER_NULL:
            ctx->_renderer.api.init   = nu__renderer_null_init;
            ctx->_renderer.api.clear  = nu__renderer_null_clear;
            ctx->_renderer.api.render = nu__renderer_null_render;
            ctx->_renderer.ctx        = NU_NULL;
            break;
        case NU_RENDERER_GL:
            ctx->_renderer.api.init   = nugl__init;
            ctx->_renderer.api.clear  = nugl__clear;
            ctx->_renderer.api.render = nugl__render;
            ctx->_renderer.ctx        = &ctx->_renderer.ctx_data.gl;
            break;
        case NU_RENDERER_DX11:
            break;
        case NU_RENDERER_SOFTRAST:
            break;
    }

    // Initialize backend
    ctx->_renderer.api.init(ctx->_renderer.ctx);

    return NU_ERROR_NONE;
}
static nu_error_t
nu__terminate_renderer (nu_context_t *ctx)
{
    return NU_ERROR_NONE;
}

void
nu_clear (nu_context_t *ctx)
{
}
nu_error_t
nu_render (nu_context_t *ctx)
{
    ctx->_renderer.api.render(ctx->_renderer.ctx,
                              ctx->_glfw_surface.viewport.extent,
                              ctx->_glfw_surface.viewport.viewport);
    return NU_ERROR_NONE;
}

#endif

#endif

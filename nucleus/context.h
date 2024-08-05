#ifndef NU_CONTEXT_H
#define NU_CONTEXT_H

#include <nucleus/memory.h>
#include <nucleus/logger.h>
#include <nucleus/surface.h>
#include <nucleus/input.h>
#include <nucleus/renderer.h>

NU_API nu_context_info_t nu_context_info_default(void);

NU_API nu_error_t nu_context_init(const nu_context_info_t *info,
                                  nu_context_t            *ctx);
NU_API nu_error_t nu_context_free(nu_context_t *ctx);
NU_API nu_error_t nu_poll_events(nu_context_t *ctx);
NU_API nu_bool_t  nu_exit_requested(const nu_context_t *ctx);

#ifdef NU_IMPLEMENTATION

nu_context_info_t
nu_context_info_default (void)
{
    nu_context_info_t info;
    info.width    = 640;
    info.height   = 400;
    info.renderer = NU_RENDERER_NULL;
    return info;
}

nu_error_t
nu_context_init (const nu_context_info_t *info, nu_context_t *ctx)
{
    nu_error_t error;

    // Initialize context
    nu_context_info_t cinfo;
    if (info)
    {
        cinfo = *info;
    }
    else
    {
        cinfo = nu_context_info_default();
    }
    ctx->_allocator       = cinfo.allocator;
    ctx->_surface.size    = nu_uvec2(cinfo.width, cinfo.height);
    ctx->_close_requested = NU_FALSE;

    // Initialize surface (and inputs)
    error = nu__init_surface(ctx);
    NU_ERROR_CHECK(error, return error);

    // Initialize renderer
    error = nu__init_renderer(ctx, cinfo.renderer);
    NU_ERROR_CHECK(error, return error);

    return NU_ERROR_NONE;
}
nu_error_t
nu_context_free (nu_context_t *ctx)
{
    nu_error_t error;

    // Terminate renderer
    error = nu__terminate_renderer(ctx);
    NU_ERROR_CHECK(error, return error);

    // Terminate surface (and inputs)
#ifdef NU_BUILD_GLFW
    nuglfw__terminate();
#endif
    return NU_ERROR_NONE;
}
nu_error_t
nu_poll_events (nu_context_t *ctx)
{
#ifdef NU_BUILD_GLFW
    nuglfw__poll_events(
        &ctx->_input.glfw, &ctx->_surface.glfw, &ctx->_close_requested);
#endif
    return NU_ERROR_NONE;
}
nu_bool_t
nu_exit_requested (const nu_context_t *ctx)
{
    return ctx->_close_requested;
}

#endif

#endif

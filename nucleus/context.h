#ifndef NU_CONTEXT_H
#define NU_CONTEXT_H

#include <nucleus/memory.h>
#include <nucleus/surface.h>
#include <nucleus/input.h>
#include <nucleus/renderer.h>

NU_API void nu_context_info_default(nu_context_info_t *info);

NU_API nu_error_t nu_init(const nu_context_info_t *info,
                          nu_allocator_t          *alloc,
                          nu_context_t            *ctx);
NU_API nu_error_t nu_terminate(nu_context_t *ctx, nu_allocator_t *alloc);
NU_API nu_error_t nu_poll_events(nu_context_t *ctx);
NU_API nu_bool_t  nu_exit_requested(const nu_context_t *ctx);

#ifdef NU_IMPLEMENTATION

void
nu_context_info_default (nu_context_info_t *info)
{
    info->width    = 640;
    info->height   = 400;
    info->renderer = NU_RENDERER_NULL;
}

nu_error_t
nu_init (const nu_context_info_t *info,
         nu_allocator_t          *alloc,
         nu_context_t            *ctx)
{
    nu_error_t error;
    NU_UNUSED(alloc);

    // Initialize context
    nu_context_info_t cinfo;
    if (info)
    {
        cinfo = *info;
    }
    else
    {
        nu_context_info_default(&cinfo);
    }
    ctx->_surface_size[0]  = (nu_i32_t)info->width;
    ctx->_surface_size[1]  = (nu_i32_t)info->height;
    ctx->_renderer_backend = info->renderer;
    ctx->_close_requested  = NU_FALSE;

    // Initialize surface (and inputs)
    error = nu__init_surface(ctx);
    NU_ERROR_CHECK(error, return error);

    // Initialize renderer
    error = nu__init_renderer(ctx);
    NU_ERROR_CHECK(error, return error);

    return NU_ERROR_NONE;
}
nu_error_t
nu_terminate (nu_context_t *ctx, nu_allocator_t *alloc)
{
    nu_error_t error;
    NU_UNUSED(alloc);

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
        &ctx->_glfw_input, &ctx->_glfw_surface, &ctx->_close_requested);
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

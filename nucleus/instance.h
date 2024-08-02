#ifndef NU_INSTANCE_H
#define NU_INSTANCE_H

#include <nucleus/memory.h>
#include <nucleus/surface.h>
#include <nucleus/input.h>
#include <nucleus/renderer.h>

NU_API void nu_instance_info_default(nu_instance_info_t *info);

NU_API nu_error_t nu_init_instance(const nu_instance_info_t *info,
                                   nu_instance_t            *instance);
NU_API nu_error_t nu_free_instance(nu_instance_t  *instance,
                                   nu_allocator_t *alloc);
NU_API nu_error_t nu_poll_events(nu_instance_t *ctx);
NU_API nu_bool_t  nu_exit_requested(const nu_instance_t *ctx);

#ifdef NU_IMPLEMENTATION

void
nu_instance_info_default (nu_instance_info_t *info)
{
    info->width    = 640;
    info->height   = 400;
    info->renderer = NU_RENDERER_NULL;
}

nu_error_t
nu_init_instance (const nu_instance_info_t *info, nu_instance_t *ctx)
{
    nu_error_t error;

    // Initialize context
    nu_instance_info_t cinfo;
    if (info)
    {
        cinfo = *info;
    }
    else
    {
        nu_instance_info_default(&cinfo);
    }
    ctx->_allocator        = info->allocator;
    ctx->_surface_size     = nu_uvec2(info->width, info->height);
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
nu_free_instance (nu_instance_t *ctx, nu_allocator_t *alloc)
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
nu_poll_events (nu_instance_t *ctx)
{
#ifdef NU_BUILD_GLFW
    nuglfw__poll_events(
        &ctx->_glfw_input, &ctx->_glfw_surface, &ctx->_close_requested);
#endif
    return NU_ERROR_NONE;
}
nu_bool_t
nu_exit_requested (const nu_instance_t *ctx)
{
    return ctx->_close_requested;
}

#endif

#endif

#ifndef NU_SURFACE_H
#define NU_SURFACE_H

#include <nucleus/types.h>

#ifdef NU_BUILD_GLFW
#include <nucleus/backend/glfw.h>
#endif

NU_API nu_error_t nu_swap_buffers(nu_context_t *ctx);

NU_API nu_error_t nuext_set_viewport_mode(nu_context_t         *ctx,
                                          nuext_viewport_mode_t mode);

#ifdef NU_IMPLEMENTATION

static nu_error_t
nu__init_surface (nu_context_t *ctx)
{
    nu_error_t error;
#ifdef NU_BUILD_GLFW
    error = nuglfw__init(ctx);
    NU_ERROR_CHECK(error, return error);
#endif
    return error;
}

nu_error_t
nu_swap_buffers (nu_context_t *ctx)
{
#ifdef NU_BUILD_GLFW
    nuglfw__swap_buffers(&ctx->_glfw_surface);
#endif
    return NU_ERROR_NONE;
}

nu_error_t
nuext_set_viewport_mode (nu_context_t *ctx, nuext_viewport_mode_t mode)
{
#ifdef NU_BUILD_GLFW
    nuglfw__swap_buffers(&ctx->_glfw_surface);
#endif
    return NU_ERROR_NONE;
}

#endif

#endif

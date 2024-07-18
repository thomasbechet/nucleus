#ifndef NU_SURFACE_H
#define NU_SURFACE_H

#include <nucleus/types.h>

NU_API nu_error_t nu_open_surface(nu_context_t            *ctx,
                                  const nu_surface_info_t *info,
                                  nu_allocator_t          *alloc);
NU_API nu_error_t nu_swap_buffers(nu_context_t *ctx);

#ifdef NU_IMPLEMENTATION

nu_error_t
nu_open_surface (nu_context_t            *ctx,
                 const nu_surface_info_t *info,
                 nu_allocator_t          *alloc)
{
    NU_UNUSED(alloc);
    switch (ctx->_info.surface_api)
    {
        case NU_SURFACE_API_NONE:
            break;
        case NU_SURFACE_API_GLFW:
#ifdef NU_BUILD_GLFW
            ctx->_glfw_window = glfwCreateWindow(
                info->width, info->height, "", NU_NULL, NU_NULL);
            if (!ctx->_glfw_window)
            {
                return NU_ERROR_BACKEND;
            }
            glfwMakeContextCurrent(ctx->_glfw_window);
#endif
            break;
    }
    return NU_ERROR_NONE;
}
nu_error_t
nu_swap_buffers (nu_context_t *ctx)
{
    switch (ctx->_info.surface_api)
    {
        case NU_SURFACE_API_NONE:
            break;
        case NU_SURFACE_API_GLFW:
#ifdef NU_BUILD_GLFW
            if (ctx->_glfw_window)
            {
                glfwSwapBuffers(ctx->_glfw_window);
            }
#endif
            break;
    }
    return NU_ERROR_NONE;
}

#endif

#endif

#ifndef NU_SURFACE_H
#define NU_SURFACE_H

#include <nucleus/types.h>

NU_API nu_error_t nu_swap_buffers(nu_context_t *ctx);

#ifdef NU_IMPLEMENTATION

nu_error_t
nu_swap_buffers (nu_context_t *ctx)
{
#ifdef NU_BUILD_GLFW
    if (ctx->_glfw_window)
    {
        glfwSwapBuffers(ctx->_glfw_window);
    }
#endif
    return NU_ERROR_NONE;
}

#endif

#endif

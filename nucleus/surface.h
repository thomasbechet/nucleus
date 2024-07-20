#ifndef NU_SURFACE_H
#define NU_SURFACE_H

#include <nucleus/types.h>

#ifdef NU_BUILD_GLFW
#include <nucleus/backend/glfw.h>
#endif

NU_API nu_error_t nu_swap_buffers(nu_context_t *ctx);

#ifdef NU_IMPLEMENTATION

nu_error_t
nu_swap_buffers (nu_context_t *ctx)
{
#ifdef NU_BUILD_GLFW
    nuglfw__swap_buffers(ctx);
#endif
    return NU_ERROR_NONE;
}

#endif

#endif

#ifndef NU_SURFACE_IMPL_H
#define NU_SURFACE_IMPL_H

#include <nucleus/internal.h>

nu_error_t
nu_swap_buffers (void)
{
#ifdef NU_BUILD_GRAPHICS
    nu__graphics_render();
#endif
#ifdef NU_BUILD_GLFW
    nuglfw__swap_buffers();
#endif
    return NU_ERROR_NONE;
}

nu_error_t
nuext_viewport_mode (nuext_viewport_mode_t mode)
{
    return NU_ERROR_NONE;
}

#endif

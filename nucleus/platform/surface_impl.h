#ifndef NU_SURFACE_IMPL_H
#define NU_SURFACE_IMPL_H

#include <nucleus/platform/surface.h>

#ifdef NU_BUILD_GLFW
#include <nucleus/platform/glfw_impl.h>
#endif

static nu_error_t
nu__init_surface (nu_platform_t *platform)
{
    nu_error_t error;
#ifdef NU_BUILD_GLFW
    error = nuglfw__init(platform);
    NU_ERROR_CHECK(error, return error);
#endif
    return error;
}

nu_error_t
nu_swap_buffers (nu_platform_t *platform)
{
#ifdef NU_BUILD_GLFW
    nuglfw__swap_buffers(&platform->_surface.glfw);
#endif
    return NU_ERROR_NONE;
}

nu_error_t
nuext_set_viewport_mode (nu_platform_t *platform, nuext_viewport_mode_t mode)
{
#ifdef NU_BUILD_GLFW
    nuglfw__swap_buffers(&platform->_surface.glfw);
#endif
    return NU_ERROR_NONE;
}

#endif

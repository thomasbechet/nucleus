#ifndef NU_SURFACE_IMPL_H
#define NU_SURFACE_IMPL_H

#include <nucleus/platform/surface.h>
#include <nucleus/platform/input.h>

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
nu_ivec2_t
nu_cursor_position (const nu_platform_t *platform,
                    nu_input_handle_t    cursor_x,
                    nu_input_handle_t    cursor_y)
{
    float cx = nu_input_value(platform, cursor_x);
    float cy = nu_input_value(platform, cursor_y);
    return nu_ivec2((nu_i32_t)(cx * (float)platform->_surface.size.x),
                    (nu_i32_t)(cy * (float)platform->_surface.size.y));
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

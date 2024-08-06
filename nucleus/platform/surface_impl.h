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
nu_render (nu_platform_t *platform)
{
    // platform->_renderer.api.render(platform->_renderer.ctx,
    //                                &platform->_surface.glfw.viewport.extent,
    //                                &platform->_surface.glfw.viewport.viewport);
#ifdef NU_BUILD_GLFW
    nuglfw__swap_buffers(&platform->_surface.glfw);
#endif
    return NU_ERROR_NONE;
}
// const nu_texture_t *
// nu_surface_color_target (const nu_platform_t *platform)
// {
//     return &platform->_renderer.surface_color;
// }

nu_error_t
nuext_set_viewport_mode (nu_platform_t *platform, nuext_viewport_mode_t mode)
{
#ifdef NU_BUILD_GLFW
    nuglfw__swap_buffers(&platform->_surface.glfw);
#endif
    return NU_ERROR_NONE;
}

#endif

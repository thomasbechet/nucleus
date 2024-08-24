#ifndef NU_CONTEXT_IMPL_H
#define NU_CONTEXT_IMPL_H

#include <nucleus/platform/context.h>

#include <nucleus/platform/surface_impl.h>
#include <nucleus/platform/input_impl.h>
#ifdef NU_BUILD_GLFW
#include <nucleus/platform/glfw_impl.h>
#endif

nu_platform_info_t
nu_platform_info_default (void)
{
    nu_platform_info_t info;
    info.width  = 640;
    info.height = 400;
    info.logger = nu_logger_info_default();
    return info;
}

nu_error_t
nu_platform_init (const nu_platform_info_t *info, nu_platform_t *platform)
{
    nu_error_t error;

    // Initialize context
    platform->_allocator = info->allocator;
    nu_logger_init(&info->logger, &platform->_logger);
    platform->_surface.size    = nu_uvec2(info->width, info->height);
    platform->_close_requested = NU_FALSE;

    NU_INFO(&platform->_logger,
            "initialize platform context (%dx%d)",
            info->width,
            info->height);

    // Initialize surface (and inputs)
    error = nu__init_surface(platform);
    NU_ERROR_CHECK(error, return error);

    return NU_ERROR_NONE;
}
nu_error_t
nu_platform_free (nu_platform_t *platform)
{
    nu_error_t error;

    NU_INFO(&platform->_logger, "terminate platform context");

    // Terminate surface (and inputs)
#ifdef NU_BUILD_GLFW
    nuglfw__free();
#endif
    return NU_ERROR_NONE;
}
nu_error_t
nu_poll_events (nu_platform_t *platform)
{
#ifdef NU_BUILD_GLFW
    nuglfw__poll_events(&platform->_input.glfw,
                        &platform->_surface.glfw,
                        &platform->_close_requested);
#endif
    return NU_ERROR_NONE;
}
nu_bool_t
nu_exit_requested (const nu_platform_t *platform)
{
    return platform->_close_requested;
}

#endif

#ifndef NU_CONTEXT_IMPL_H
#define NU_CONTEXT_IMPL_H

#include <nucleus/platform/context.h>

#include <nucleus/platform/surface_impl.h>
#include <nucleus/platform/input_impl.h>
#ifdef NU_BUILD_GLFW
#include <nucleus/platform/glfw_impl.h>
#endif

nu_error_t
nu_platform_create (const nu_platform_info_t *info, nu_platform_t *platform)
{
    nu_error_t error;

    // Initialize context
    platform->_ptr
        = (nu__platform_t *)nu_alloc(info->allocator, sizeof(*platform->_ptr));
    platform->_ptr->surface.size    = nu_uvec2(info->width, info->height);
    platform->_ptr->close_requested = NU_FALSE;
    platform->_ptr->allocator       = info->allocator;
    platform->_ptr->logger          = info->logger;

    NU_INFO(platform->_ptr->logger,
            "initialize platform context (%dx%d)",
            info->width,
            info->height);

    // Initialize surface (and inputs)
    error = nu__init_surface(platform->_ptr);
    NU_ERROR_CHECK(error, return error);

    return NU_ERROR_NONE;
}
nu_error_t
nu_platform_delete (nu_platform_t platform)
{
    nu_error_t error;

    NU_INFO(platform._ptr->logger, "terminate platform context");

    // Terminate surface (and inputs)
#ifdef NU_BUILD_GLFW
    nuglfw__free();
#endif

    nu_free(platform._ptr->allocator, platform._ptr, sizeof(*platform._ptr));

    return NU_ERROR_NONE;
}
nu_error_t
nu_platform_poll_events (nu_platform_t platform)
{
#ifdef NU_BUILD_GLFW
    nuglfw__poll_events(&platform._ptr->input.glfw,
                        &platform._ptr->surface.glfw,
                        &platform._ptr->close_requested);
#endif
    return NU_ERROR_NONE;
}
nu_bool_t
nu_platform_exit_requested (nu_platform_t platform)
{
    return platform._ptr->close_requested;
}

#endif

#ifndef NU_PLATFORM_IMPL_H
#define NU_PLATFORM_IMPL_H

#include <nucleus/internal.h>

static nu_error_t
nu__platform_init (const nu_platform_info_t *info)
{
    nu_error_t error;

    // Initialize context
    nu_pool_init(10, &_ctx.input.entries);
    _ctx.surface.size             = nu_uvec2(info->width, info->height);
    _ctx.platform.close_requested = NU_FALSE;

    NU_INFO("initialize platform context (%dx%d)", info->width, info->height);

    // Initialize surface (and inputs)
#ifdef NU_BUILD_GLFW
    error = nuglfw__init();
    NU_ERROR_CHECK(error, return error);
#endif

    return NU_ERROR_NONE;
}
static nu_error_t
nu__platform_free (void)
{
    nu_error_t error;

    NU_INFO("terminate platform context");

    // Terminate surface (and inputs)
#ifdef NU_BUILD_GLFW
    nuglfw__free();
#endif

    nu_pool_free(&_ctx.input.entries);

    return NU_ERROR_NONE;
}
nu_error_t
nu_poll_events (void)
{
#ifdef NU_BUILD_GLFW
    nuglfw__poll_events();
#endif
    return NU_ERROR_NONE;
}
nu_bool_t
nu_exit_requested (void)
{
    return _ctx.platform.close_requested;
}

#endif

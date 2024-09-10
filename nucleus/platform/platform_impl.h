#ifndef NU_PLATFORM_IMPL_H
#define NU_PLATFORM_IMPL_H

#include <nucleus/internal.h>

#include <nucleus/platform/input_impl.h>
#include <nucleus/platform/surface_impl.h>
#ifdef NU_BUILD_GLFW
#include <nucleus/platform/glfw_impl.h>
#endif

static nu_error_t
nu__platform_init (void)
{
    nu_error_t error;

    // Initialize context
    nu_pool_init(10, &_ctx.platform.input.entries);
    _ctx.platform.surface.size
        = nu_uvec2(_ctx.config.platform.width, _ctx.config.platform.height);
    _ctx.platform.close_requested = NU_FALSE;

    nu_info("initialize platform context (%dx%d)",
            _ctx.config.platform.width,
            _ctx.config.platform.height);

    // Initialize surface (and inputs)
#ifdef NU_BUILD_GLFW
    error = nuglfw__init();
    nu_error_check(error, return error);
#endif

    return NU_ERROR_NONE;
}
static nu_error_t
nu__platform_free (void)
{
    nu_error_t error;

    nu_info("terminate platform context");

    // Terminate surface (and inputs)
#ifdef NU_BUILD_GLFW
    nuglfw__free();
#endif

    nu_pool_free(&_ctx.platform.input.entries);

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

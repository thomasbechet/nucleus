#ifndef NU_APP_IMPL_H
#define NU_APP_IMPL_H

#include <nucleus/internal.h>

void
nu_app_init_callback (nu_app_callback_t callback)
{
    _ctx.core.app.init = callback;
}
void
nu_app_free_callback (nu_app_callback_t callback)
{
    _ctx.core.app.free = callback;
}
void
nu_app_update_callback (nu_app_callback_t callback)
{
    _ctx.core.app.update = callback;
}

float
nu_deltatime (void)
{
    return _ctx.core.app.deltatime;
}

void nu_main(void);

int
main (int argc, char *argv[])
{
    nu_error_t error;

    // Initialise app configuration
    _ctx.core.app.init      = NU_NULL;
    _ctx.core.app.update    = NU_NULL;
    _ctx.core.app.deltatime = 0;
#ifdef NU_BUILD_PLATFORM
    _ctx.platform.size = nu_v2u(640, 400);
#endif

    // User app configuration
    nu_main();

    // Initialize modules

    error = nu__core_init();
    NU_ERROR_CHECK(error, return error);
#ifdef NU_BUILD_PLATFORM
    error = nu__platform_init();
    NU_ERROR_CHECK(error, return error);
#endif
#ifdef NU_BUILD_GRAPHICS
    error = nu__graphics_init();
    NU_ERROR_CHECK(error, return error);
#endif
#ifdef NU_BUILD_UTILS
    error = nu__utils_init();
    NU_ERROR_CHECK(error, return error);
#endif
#ifdef NU_BUILD_IMPORTER
    nu__importer_init();
#endif
#ifdef NU_BUILD_ASSET
    error = nu__asset_init();
    NU_ERROR_CHECK(error, return error);
#endif
#ifdef NU_BUILD_UI
    nu__ui_init();
#endif
#ifdef NU_BUILD_PHYSICS
    nu__physics_init();
#endif
#ifdef NU_BUILD_ECS
    nu__ecs_init();
#endif
#ifdef NU_BUILD_SERIA
    nu__seria_init();
#endif

    // Run app

    if (_ctx.core.app.init)
    {
        _ctx.core.app.init();
    }

    nu_timer_t timer;
    nu_timer_reset(&timer);
    nu_f32_t  delta      = 0.0f;
    nu_bool_t bool_state = NU_TRUE;

    if (_ctx.core.app.update)
    {
        while (!nu_exit_requested())
        {
            // Compute delta time
            _ctx.core.app.deltatime = nu_timer_elapsed(&timer);
            nu_timer_reset(&timer);

            // Poll events
#ifdef NU_BUILD_PLATFORM
            nu__platform_poll_events();
#endif

            // User update
            if (_ctx.core.app.update)
            {
                _ctx.core.app.update();
            }

            // Refresh surface
#ifdef NU_BUILD_PLATFORM
            nu__platform_swap_buffers();
#endif
        }
    }

    if (_ctx.core.app.free)
    {
        _ctx.core.app.free();
    }

    // Terminate modules

#ifdef NU_BUILD_SERIA
    nu__seria_init();
#endif
#ifdef NU_BUILD_ECS
    nu__ecs_free();
#endif
#ifdef NU_BUILD_PHYSICS
    nu__physics_free();
#endif
#ifdef NU_BUILD_UI
    nu__ui_free();
#endif
#ifdef NU_BUILD_ASSET
    nu__asset_free();
#endif
#ifdef NU_BUILD_IMPORTER
    nu__importer_free();
#endif
#ifdef NU_BUILD_UTILS
    nu__utils_free();
#endif
#ifdef NU_BUILD_GRAPHICS
    nu__graphics_free();
#endif
#ifdef NU_BUILD_PLATFORM
    nu__platform_free();
#endif
    nu__core_free();

    return 0;
}

#endif

#ifndef NU_CONTEXT_IMPL_H
#define NU_CONTEXT_IMPL_H

#include <nucleus/internal.h>

#define NU__INIT_MODULE(name)    \
    error = nu__##name##_init(); \
    NU_ERROR_CHECK(error, return error);

static nu__config_t *
nu__config (void)
{
    nu__config_t *cfg = &_ctx.config;
    if (!cfg->initialized)
    {
#ifdef NU_BUILD_PLATFORM
        cfg->platform.width  = 640;
        cfg->platform.height = 400;
#endif
        cfg->initialized = NU_TRUE;
    }
    return cfg;
}

nu_error_t
nu_init (void)
{
    nu_error_t error;

    // Ensure configuration has been initialized
    (void)nu__config();

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

    return NU_ERROR_NONE;
}
void
nu_terminate (void)
{
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
}

#endif

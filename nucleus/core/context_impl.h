#ifndef NU_CONTEXT_IMPL_H
#define NU_CONTEXT_IMPL_H

#include <nucleus/internal.h>

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
#ifdef NU_BUILD_GRAPHICS
        cfg->graphics.api = NU_RENDERER_GL;
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
    nu_error_check(error, return error);

#ifdef NU_BUILD_PLATFORM
    error = nu__platform_init();
    nu_error_check(error, return error);
#endif

#ifdef NU_BUILD_GRAPHICS
    error = nu__graphics_init();
    nu_error_check(error, return error);
#endif

#ifdef NU_BUILD_UTILS
    error = nu__utils_init();
    nu_error_check(error, return error);
#endif

#ifdef NU_BUILD_IMPORTER
    nu__importer_init();
#endif

#ifdef NU_BUILD_ASSET
    error = nu__asset_init();
    nu_error_check(error, return error);
#endif

    return NU_ERROR_NONE;
}
void
nu_terminate (void)
{
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

#ifndef NU_CONTEXT_IMPL_H
#define NU_CONTEXT_IMPL_H

#include <nucleus/internal.h>

nu_error_t
nu_initialize (void)
{
    nu_error_t error;

    memset(&_ctx, 0, sizeof(_ctx));

    error = nu__core_init();
    NU_ERROR_CHECK(error, return error);

#ifdef NU_BUILD_PLATFORM
    nu_platform_info_t pinfo;
    pinfo.width  = 640;
    pinfo.height = 400;
    error        = nu__platform_init(&pinfo);
    NU_ERROR_CHECK(error, return error);
#endif

#ifdef NU_BUILD_GRAPHICS
    nu_renderer_info_t rinfo;
    rinfo.api = NU_RENDERER_GL;
    error     = nu__graphics_init(&rinfo);
    NU_ERROR_CHECK(error, return error);
#endif

#ifdef NU_BUILD_UTILS
    error = nu__utils_init();
    NU_ERROR_CHECK(error, return error);
#endif

#ifdef NU_BUILD_ASSET
    error = nu__asset_init();
    NU_ERROR_CHECK(error, return error);
#endif

    return NU_ERROR_NONE;
}
nu_error_t
nu_terminate (void)
{
#ifdef NU_BUILD_ASSET
    nu__asset_free();
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
    return NU_ERROR_NONE;
}

#endif

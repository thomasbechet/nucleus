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
    nu_platform_info_t info;
    info.width  = 640;
    info.height = 400;
    error       = nu__platform_init(&info);
    NU_ERROR_CHECK(error, return error);
#endif

    return NU_ERROR_NONE;
}
nu_error_t
nu_terminate (void)
{
#ifdef NU_BUILD_PLATFORM
    nu__platform_free();
#endif
    nu__core_free();
    return NU_ERROR_NONE;
}

#endif

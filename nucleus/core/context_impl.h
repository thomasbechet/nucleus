#ifndef NU_CONTEXT_IMPL_H
#define NU_CONTEXT_IMPL_H

#include <nucleus/internal.h>

nu_error_t
nu_initialize (void)
{
    memset(&_ctx, 0, sizeof(_ctx));
    _ctx.core.allocator    = nu__allocator_stdlib();
    _ctx.core.logger.level = NU_LOG_DEBUG;

#ifdef NU_BUILD_PLATFORM
    nu_platform_info_t info;
    info.width       = 640;
    info.height      = 400;
    nu_error_t error = nu__platform_init(&info);
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
    return NU_ERROR_NONE;
}

#endif

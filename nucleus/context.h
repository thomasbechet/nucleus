#ifndef NU_CONTEXT_H
#define NU_CONTEXT_H

#include <nucleus/types.h>

NU_API void nu_context_info_default(nu_context_info_t *info);

NU_API nu_error_t nu_init(const nu_context_info_t *info,
                          nu_allocator_t          *alloc,
                          nu_context_t            *ctx);
NU_API nu_error_t nu_terminate(nu_context_t *ctx, nu_allocator_t *alloc);
NU_API nu_error_t nu_poll_events(nu_context_t *ctx);
NU_API nu_bool_t  nu_exit_requested(const nu_context_t *ctx);

#ifdef NU_IMPLEMENTATION

void
nu_context_info_default (nu_context_info_t *info)
{
    info->width  = 640;
    info->height = 400;
}

nu_error_t
nu_init (const nu_context_info_t *info,
         nu_allocator_t          *alloc,
         nu_context_t            *ctx)
{
    nu_error_t error;
    NU_UNUSED(alloc);
    if (info)
    {
        ctx->_info = *info;
    }
    else
    {
        nu_context_info_default(&ctx->_info);
    }
    ctx->_close_requested = NU_FALSE;

#ifdef NU_BUILD_GLFW
    error = nuglfw__init(ctx, &ctx->_info);
    NU_ERROR_CHECK(error, return error);
#endif

    return NU_ERROR_NONE;
}
nu_error_t
nu_terminate (nu_context_t *ctx, nu_allocator_t *alloc)
{
    NU_UNUSED(alloc);
#ifdef NU_BUILD_GLFW
    nuglfw__terminate(&ctx->_glfw);
#endif
    return NU_ERROR_NONE;
}
nu_error_t
nu_poll_events (nu_context_t *ctx)
{
#ifdef NU_BUILD_GLFW
    nuglfw__poll_events(ctx);
#endif
    return NU_ERROR_NONE;
}
nu_bool_t
nu_exit_requested (const nu_context_t *ctx)
{
    return ctx->_close_requested;
}

#endif

#endif

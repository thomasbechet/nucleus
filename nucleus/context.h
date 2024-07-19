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
    NU_UNUSED(ctx);
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

    // GLFW initialization
#ifdef NU_BUILD_GLFW
    if (!glfwInit())
    {
        return NU_ERROR_BACKEND;
    }
    ctx->_glfw_window = glfwCreateWindow(
        ctx->_info.width, ctx->_info.height, "", NU_NULL, NU_NULL);
    if (!ctx->_glfw_window)
    {
        return NU_ERROR_BACKEND;
    }
    glfwMakeContextCurrent(ctx->_glfw_window);
#endif

    return NU_ERROR_NONE;
}
nu_error_t
nu_terminate (nu_context_t *ctx, nu_allocator_t *alloc)
{
    NU_UNUSED(ctx);
    NU_UNUSED(alloc);
#ifdef NU_BUILD_GLFW
    glfwTerminate();
#endif
    return NU_ERROR_NONE;
}
nu_error_t
nu_poll_events (nu_context_t *ctx)
{
#ifdef NU_BUILD_GLFW
    if (ctx->_glfw_window)
    {
        glfwPollEvents();
        ctx->_close_requested = glfwWindowShouldClose(ctx->_glfw_window);
    }
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

#ifndef NU_BACKEND_GLFW_H
#define NU_BACKEND_GLFW_H

#include <nucleus/types.h>

#ifdef NU_IMPLEMENTATION

static nu_error_t
nuglfw__init (nu_context_t *ctx, const nu_context_info_t *info)
{
    NU_UNUSED(info);
    if (!glfwInit())
    {
        return NU_ERROR_BACKEND;
    }
    ctx->_glfw.win = glfwCreateWindow(
        ctx->_info.width, ctx->_info.height, "", NU_NULL, NU_NULL);
    if (!ctx->_glfw.win)
    {
        return NU_ERROR_BACKEND;
    }
    glfwMakeContextCurrent(ctx->_glfw.win);
    return NU_ERROR_NONE;
}

static nu_error_t
nuglfw__terminate (nu__glfw_backend_t *backend)
{
    NU_UNUSED(backend);
    glfwTerminate();
    return NU_ERROR_NONE;
}

static nu_error_t
nuglfw__poll_events (nu_context_t *ctx)
{
    if (ctx->_glfw.win)
    {
        glfwPollEvents();
        ctx->_close_requested = glfwWindowShouldClose(ctx->_glfw.win);
    }
    return NU_ERROR_NONE;
}

static nu_error_t
nuglfw__swap_buffers(nu_context_t *ctx)
{
    if (ctx->_glfw.win)
    {
        glfwSwapBuffers(ctx->_glfw.win);
    }
    return NU_ERROR_NONE;
}

#endif

#endif

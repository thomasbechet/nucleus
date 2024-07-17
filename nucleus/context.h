#ifndef NU_CONTEXT_H
#define NU_CONTEXT_H

#include <nucleus/types.h>

NU_API nu_error_t nu_init(const nu_context_info_t *info,
                          nu_allocator_t          *alloc,
                          nu_context_t            *ctx);
NU_API nu_error_t nu_terminate(nu_context_t *ctx, nu_allocator_t *alloc);
NU_API nu_bool_t  nu_exit_requested(nu_context_t *ctx);

#ifdef NU_IMPLEMENTATION

#ifdef NU_BUILD_GLFW
#include <nucleus/external/glfw.h>
#endif

nu_error_t
nu_init (const nu_context_info_t *info,
         nu_allocator_t          *alloc,
         nu_context_t            *ctx)
{
    ctx->_info = *info;

    // Surface API
    switch (info->surface_api)
    {
        case NU_SURFACE_API_NONE:
            break;
        case NU_SURFACE_API_GLFW: {
#ifdef NU_BUILD_GLFW
            if (!glfwInit())
            {
                return NU_ERROR_BACKEND;
            }
            ctx->_glfw_window = NU_NULL;
#else
            return NU_ERROR_UNSUPPORTED_API;
#endif
        }
        break;
    }

    // Input API
    switch (info->input_api)
    {
        case NU_INPUT_API_NONE:
            break;
        case NU_INPUT_API_GLFW: {
            if (info->surface_api != NU_SURFACE_API_GLFW)
            {
                return NU_ERROR_UNSUPPORTED_API;
            }
        }
        break;
    }

    // Graphics API
    switch (info->graphics_api)
    {
        case NU_GRAPHICS_API_NONE:
            break;
        case NU_GRAPHICS_API_OPENGL:
            break;
    }

    return NU_ERROR_NONE;
}
nu_error_t
nu_terminate (nu_context_t *ctx, nu_allocator_t *alloc)
{
    switch (ctx->_info.surface_api)
    {
        case NU_SURFACE_API_NONE:
            break;
        case NU_SURFACE_API_GLFW:
#ifdef NU_BUILD_GLFW
            glfwTerminate();
#endif
            break;
    }
    return NU_ERROR_NONE;
}
nu_bool_t
nu_exit_requested (nu_context_t *ctx)
{
    switch (ctx->_info.surface_api)
    {
        case NU_SURFACE_API_NONE:
            break;
        case NU_SURFACE_API_GLFW:
#ifdef NU_BUILD_GLFW
            if (ctx->_glfw_window)
            {
                glfwPollEvents(); 
                return glfwWindowShouldClose(ctx->_glfw_window);
            }
#endif
            break;
    }
    return NU_FALSE;
}

#endif

#endif

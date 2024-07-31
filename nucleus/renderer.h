#ifndef NU_RENDERER_H
#define NU_RENDERER_H

#include <nucleus/types.h>

NU_API nu_error_t nu_create_camera(nu_context_t *ctx, nu_camera_t *camera);
NU_API nu_error_t nu_delete_camera(nu_context_t *ctx, nu_camera_t *camera);
NU_API nu_error_t nu_update_camera(nu_context_t *ctx, nu_camera_t *camera);
NU_API nu_error_t nu_create_mesh(nu_context_t         *ctx,
                                 const nu_mesh_info_t *info,
                                 nu_mesh_t            *mesh);
NU_API nu_error_t nu_delete_mesh(nu_context_t *ctx, nu_mesh_t *mesh);
NU_API nu_error_t nu_create_renderpass(nu_context_t               *ctx,
                                       const nu_renderpass_info_t *info,
                                       nu_renderpass_t            *pass);
NU_API nu_error_t nu_delete_renderpass(nu_context_t    *ctx,
                                       nu_renderpass_t *pass);

NU_API void nu_submit_renderpass(nu_context_t                 *ctx,
                                 nu_renderpass_t              *pass,
                                 const nu_renderpass_submit_t *info);
NU_API void nu_draw(nu_context_t    *ctx,
                    nu_renderpass_t *renderpass,
                    const nu_mesh_t *mesh,
                    const float     *transform);
NU_API void nu_draw_instanced(nu_renderpass_t *renderpass,
                              const float     *transforms,
                              nu_u32_t         count);

#ifdef NU_IMPLEMENTATION

#ifdef NU_BUILD_RENDERER_GL
#include <nucleus/backend/gl.h>
#endif

static nu_error_t
nu__renderer_null_init (void *ctx, nu_iv2_t size)
{
    return NU_ERROR_NONE;
}
static nu_error_t
nu__renderer_null_render (void       *ctx,
                          nu_extent_t global_viewport,
                          nu_v4_t     viewport)
{
    NU_UNUSED(ctx);
    NU_UNUSED(viewport);
    return NU_ERROR_NONE;
}
static nu_error_t
nu__renderer_null_create_camera (void *ctx, nu_camera_t *camera)
{
    return NU_ERROR_NONE;
}
static nu_error_t
nu__renderer_null_delete_camera (void *ctx, nu_camera_t *camera)
{
    return NU_ERROR_NONE;
}
static nu_error_t
nu__renderer_null_update_camera (void *ctx, nu_camera_t *camera)
{
    return NU_ERROR_NONE;
}
static nu_error_t
nu__renderer_null_create_mesh (void                 *ctx,
                               const nu_mesh_info_t *info,
                               nu_mesh_t            *mesh)
{
    return NU_ERROR_NONE;
}
static nu_error_t
nu__renderer_null_delete_mesh (void *ctx, nu_mesh_t *mesh)
{
    return NU_ERROR_NONE;
}
static nu_error_t
nu__renderer_null_create_renderpass (void                       *ctx,
                                     const nu_renderpass_info_t *info,
                                     nu_renderpass_t            *pass)
{
    return NU_ERROR_NONE;
}
static nu_error_t
nu__renderer_null_delete_renderpass (void *ctx, nu_renderpass_t *pass)
{
    return NU_ERROR_NONE;
}
static void
nu__renderer_null_submit_renderpass (void                         *ctx,
                                     nu_renderpass_t              *pass,
                                     const nu_renderpass_submit_t *info)
{
}

static nu_error_t
nu__init_renderer (nu_context_t *ctx)
{
    switch (ctx->_renderer_backend)
    {
        case NU_RENDERER_NULL:
            ctx->_renderer.api.init   = nu__renderer_null_init;
            ctx->_renderer.api.render = nu__renderer_null_render;

            ctx->_renderer.api.create_camera = nu__renderer_null_create_camera;
            ctx->_renderer.api.delete_camera = nu__renderer_null_delete_camera;
            ctx->_renderer.api.update_camera = nu__renderer_null_update_camera;
            ctx->_renderer.api.create_mesh   = nu__renderer_null_create_mesh;
            ctx->_renderer.api.delete_mesh   = nu__renderer_null_delete_mesh;
            ctx->_renderer.api.create_renderpass
                = nu__renderer_null_create_renderpass;
            ctx->_renderer.api.delete_renderpass
                = nu__renderer_null_delete_renderpass;

            ctx->_renderer.api.submit_renderpass
                = nu__renderer_null_submit_renderpass;

            ctx->_renderer.ctx = NU_NULL;
            break;
        case NU_RENDERER_GL:
            ctx->_renderer.api.init   = nugl__init;
            ctx->_renderer.api.render = nugl__render;

            ctx->_renderer.api.create_camera     = nugl__create_camera;
            ctx->_renderer.api.delete_camera     = nugl__delete_camera;
            ctx->_renderer.api.update_camera     = nugl__update_camera;
            ctx->_renderer.api.create_mesh       = nugl__create_mesh;
            ctx->_renderer.api.delete_mesh       = nugl__delete_mesh;
            ctx->_renderer.api.create_renderpass = nugl__create_renderpass;
            ctx->_renderer.api.delete_renderpass = nugl__delete_renderpass;

            ctx->_renderer.api.submit_renderpass = nugl__submit_renderpass;

            ctx->_renderer.ctx = &ctx->_renderer.backend.gl;
            break;
        case NU_RENDERER_DX11:
            break;
        case NU_RENDERER_SOFTRAST:
            break;
    }

    // Initialize backend
    nu_error_t error
        = ctx->_renderer.api.init(ctx->_renderer.ctx, ctx->_surface_size);
    NU_ERROR_CHECK(error, return error);

    return NU_ERROR_NONE;
}
static nu_error_t
nu__terminate_renderer (nu_context_t *ctx)
{
    return NU_ERROR_NONE;
}

nu_error_t
nu_create_camera (nu_context_t *ctx, nu_camera_t *camera)
{
    camera->projection = NU_CAMERA_DEFAULT_PROJECTION;
    camera->fov        = NU_CAMERA_DEFAULT_FOV;
    camera->near       = NU_CAMERA_DEFAULT_NEAR;
    camera->far        = NU_CAMERA_DEFAULT_FAR;
    camera->eye        = NU_CAMERA_DEFAULT_EYE;
    camera->center     = NU_CAMERA_DEFAULT_CENTER;
    camera->up         = NU_CAMERA_DEFAULT_UP;
    return ctx->_renderer.api.create_camera(ctx->_renderer.ctx, camera);
}
nu_error_t
nu_delete_camera (nu_context_t *ctx, nu_camera_t *camera)
{
    return ctx->_renderer.api.delete_camera(ctx->_renderer.ctx, camera);
}
nu_error_t
nu_update_camera (nu_context_t *ctx, nu_camera_t *camera)
{
    return ctx->_renderer.api.update_camera(ctx->_renderer.ctx, camera);
}
nu_error_t
nu_create_mesh (nu_context_t *ctx, const nu_mesh_info_t *info, nu_mesh_t *mesh)
{
    return ctx->_renderer.api.create_mesh(ctx->_renderer.ctx, info, mesh);
}
nu_error_t
nu_delete_mesh (nu_context_t *ctx, nu_mesh_t *mesh)
{
    return ctx->_renderer.api.delete_mesh(ctx->_renderer.ctx, mesh);
}
nu_error_t
nu_create_renderpass (nu_context_t               *ctx,
                      const nu_renderpass_info_t *info,
                      nu_renderpass_t            *pass)
{
    return ctx->_renderer.api.create_renderpass(ctx->_renderer.ctx, info, pass);
}
nu_error_t
nu_delete_renderpass (nu_context_t *ctx, nu_renderpass_t *pass)
{
    return ctx->_renderer.api.delete_renderpass(ctx->_renderer.ctx, pass);
}

void
nu_submit_renderpass (nu_context_t                 *ctx,
                      nu_renderpass_t              *pass,
                      const nu_renderpass_submit_t *info)
{
}
void
nu_draw (nu_context_t    *ctx,
         nu_renderpass_t *renderpass,
         const nu_mesh_t *mesh,
         const float     *transform)
{
}

#endif

#endif

#ifndef NU_RENDERER_IMPL_H
#define NU_RENDERER_IMPL_H

#include <nucleus/graphics/renderer.h>

#ifdef NU_BUILD_RENDERER_GL
#include <nucleus/graphics/gl_impl.h>
#endif

static nu_error_t
nu__renderer_null_init (nu_renderer_t  *ctx,
                        nu_allocator_t *allocator,
                        nu_uvec2_t      size)
{
    return NU_ERROR_NONE;
}
static nu_error_t
nu__renderer_null_render (nu_renderer_t   *ctx,
                          const nu_rect_t *global_viewport,
                          const nu_rect_t *viewport)
{
    return NU_ERROR_NONE;
}
static nu_texture_handle_t
nu__renderer_null_create_surface_color (nu_renderer_t *ctx, nu_uvec2_t size)
{
    nu_texture_handle_t tex = { 0 };
    return tex;
}
static nu_error_t
nu__renderer_null_create_camera (nu_renderer_t          *ctx,
                                 const nu_camera_info_t *info,
                                 nu_camera_handle_t     *camera)
{
    return NU_ERROR_NONE;
}
static nu_error_t
nu__renderer_null_delete_camera (nu_renderer_t *ctx, nu_camera_handle_t camera)
{
    return NU_ERROR_NONE;
}
static nu_error_t
nu__renderer_null_update_camera (nu_renderer_t          *ctx,
                                 nu_camera_handle_t      camera,
                                 const nu_camera_info_t *info)
{
    return NU_ERROR_NONE;
}
static nu_error_t
nu__renderer_null_create_mesh (nu_renderer_t        *ctx,
                               const nu_mesh_info_t *info,
                               nu_mesh_handle_t     *mesh)
{
    return NU_ERROR_NONE;
}
static nu_error_t
nu__renderer_null_delete_mesh (nu_renderer_t *ctx, nu_mesh_handle_t mesh)
{
    return NU_ERROR_NONE;
}
static nu_error_t
nu__renderer_null_create_texture (nu_renderer_t           *ctx,
                                  const nu_texture_info_t *info,
                                  nu_texture_handle_t     *texture)
{
    return NU_ERROR_NONE;
}
static nu_error_t
nu__renderer_null_delete_texture (nu_renderer_t      *ctx,
                                  nu_texture_handle_t texture)
{
    return NU_ERROR_NONE;
}
static nu_error_t
nu__renderer_null_create_cubemap (nu_renderer_t           *ctx,
                                  const nu_cubemap_info_t *info,
                                  nu_cubemap_handle_t     *cubemap)
{
    return NU_ERROR_NONE;
}
static nu_error_t
nu__renderer_null_delete_cubemap (nu_renderer_t      *ctx,
                                  nu_cubemap_handle_t cubemap)
{
    return NU_ERROR_NONE;
}
static nu_error_t
nu__renderer_null_create_material (nu_renderer_t            *ctx,
                                   const nu_material_info_t *info,
                                   nu_material_handle_t     *material)
{
    return NU_ERROR_NONE;
}
static nu_error_t
nu__renderer_null_delete_material (nu_renderer_t       *ctx,
                                   nu_material_handle_t material)
{
    return NU_ERROR_NONE;
}
static nu_error_t
nu__renderer_null_update_material (nu_renderer_t            *ctx,
                                   nu_material_handle_t      material,
                                   const nu_material_info_t *info)
{
    return NU_ERROR_NONE;
}
static nu_error_t
nu__renderer_null_create_renderpass (nu_renderer_t              *ctx,
                                     const nu_renderpass_info_t *info,
                                     nu_renderpass_handle_t     *pass)
{
    return NU_ERROR_NONE;
}
static nu_error_t
nu__renderer_null_delete_renderpass (nu_renderer_t         *ctx,
                                     nu_renderpass_handle_t pass)
{
    return NU_ERROR_NONE;
}

static void
nu__renderer_null_draw (nu_renderer_t         *ctx,
                        nu_renderpass_handle_t pass,
                        nu_mesh_handle_t       mesh,
                        nu_material_handle_t   material,
                        nu_mat4_t              transform)
{
}
static void
nu__renderer_null_submit_renderpass (nu_renderer_t                *ctx,
                                     nu_renderpass_handle_t        pass,
                                     const nu_renderpass_submit_t *info)
{
}

nu_renderer_info_t
nu_renderer_info_default (void)
{
    nu_renderer_info_t info;
    info.logger = nu_logger_info_default();
    info.api    = NU_RENDERER_NULL;
    return info;
}

nu_error_t
nu_renderer_init (nu_platform_t            *platform,
                  const nu_renderer_info_t *info,
                  nu_renderer_t            *renderer)
{
    renderer->_allocator = info->allocator;

    switch (info->api)
    {
        case NU_RENDERER_NULL:
            renderer->_api.init   = nu__renderer_null_init;
            renderer->_api.render = nu__renderer_null_render;
            renderer->_api.create_surface_color
                = nu__renderer_null_create_surface_color;

            renderer->_api.create_camera   = nu__renderer_null_create_camera;
            renderer->_api.delete_camera   = nu__renderer_null_delete_camera;
            renderer->_api.update_camera   = nu__renderer_null_update_camera;
            renderer->_api.create_mesh     = nu__renderer_null_create_mesh;
            renderer->_api.delete_mesh     = nu__renderer_null_delete_mesh;
            renderer->_api.create_texture  = nu__renderer_null_create_texture;
            renderer->_api.delete_texture  = nu__renderer_null_delete_texture;
            renderer->_api.create_cubemap  = nu__renderer_null_create_cubemap;
            renderer->_api.delete_cubemap  = nu__renderer_null_delete_cubemap;
            renderer->_api.create_material = nu__renderer_null_create_material;
            renderer->_api.delete_material = nu__renderer_null_delete_material;
            renderer->_api.update_material = nu__renderer_null_update_material;
            renderer->_api.create_renderpass
                = nu__renderer_null_create_renderpass;
            renderer->_api.delete_renderpass
                = nu__renderer_null_delete_renderpass;

            renderer->_api.draw = nu__renderer_null_draw;
            renderer->_api.submit_renderpass
                = nu__renderer_null_submit_renderpass;
            break;
        case NU_RENDERER_GL:
            renderer->_api.init                 = nugl__init;
            renderer->_api.render               = nugl__render;
            renderer->_api.create_surface_color = nugl__create_surface_color;

            renderer->_api.create_camera     = nugl__create_camera;
            renderer->_api.delete_camera     = nugl__delete_camera;
            renderer->_api.update_camera     = nugl__update_camera;
            renderer->_api.create_mesh       = nugl__create_mesh;
            renderer->_api.delete_mesh       = nugl__delete_mesh;
            renderer->_api.create_texture    = nugl__create_texture;
            renderer->_api.delete_texture    = nugl__delete_texture;
            renderer->_api.create_cubemap    = nugl__create_cubemap;
            renderer->_api.delete_cubemap    = nugl__delete_cubemap;
            renderer->_api.create_material   = nugl__create_material;
            renderer->_api.delete_material   = nugl__delete_material;
            renderer->_api.update_material   = nugl__update_material;
            renderer->_api.create_renderpass = nugl__create_renderpass;
            renderer->_api.delete_renderpass = nugl__delete_renderpass;

            renderer->_api.draw              = nugl__draw;
            renderer->_api.submit_renderpass = nugl__submit_renderpass;
            break;
        case NU_RENDERER_DX11:
            break;
        case NU_RENDERER_SOFTRAST:
            break;
    }

    // Initialize logger
    nu_logger_init(&info->logger, &renderer->_logger);

    // Initialize backend
    NU_INFO(&renderer->_logger, "initialize renderer context");
    nu_error_t error = renderer->_api.init(
        renderer, &renderer->_allocator, platform->_surface.size);
    NU_ERROR_CHECK(error, return error);

    // Create surface texture
    renderer->_surface_color = renderer->_api.create_surface_color(
        renderer, platform->_surface.size);

    return NU_ERROR_NONE;
}
nu_error_t
nu_renderer_terminate (nu_renderer_t *renderer)
{
    NU_INFO(&renderer->_logger, "terminate renderer context");

    return NU_ERROR_NONE;
}
nu_error_t
nu_render (nu_platform_t *platform, nu_renderer_t *renderer)
{
    renderer->_api.render(renderer,
                          &platform->_surface.glfw.viewport.extent,
                          &platform->_surface.glfw.viewport.viewport);
    return NU_ERROR_NONE;
}
nu_texture_handle_t
nu_surface_color_target (const nu_platform_t *platform,
                         const nu_renderer_t *renderer)
{
    return renderer->_surface_color;
}

nu_camera_info_t
nu_camera_info_default (void)
{
    nu_camera_info_t info;
    info.projection = NU_PROJECTION_PERSPECTIVE;
    info.fov        = 80;
    info.near       = 0.01;
    info.far        = 100;
    info.eye        = NU_VEC3_ZERO;
    info.center     = NU_VEC3_FORWARD;
    info.up         = NU_VEC3_UP;
    return info;
}
nu_error_t
nu_camera_create (nu_renderer_t          *ctx,
                  const nu_camera_info_t *info,
                  nu_camera_handle_t     *camera)
{
    return ctx->_api.create_camera(ctx, info, camera);
}
nu_error_t
nu_camera_delete (nu_renderer_t *ctx, nu_camera_handle_t camera)
{
    return ctx->_api.delete_camera(ctx, camera);
}
nu_error_t
nu_camera_update (nu_renderer_t          *ctx,
                  nu_camera_handle_t      camera,
                  const nu_camera_info_t *info)
{
    return ctx->_api.update_camera(ctx, camera, info);
}

nu_error_t
nu_mesh_create (nu_renderer_t        *ctx,
                const nu_mesh_info_t *info,
                nu_mesh_handle_t     *mesh)
{
    return ctx->_api.create_mesh(ctx, info, mesh);
}
nu_error_t
nu_mesh_delete (nu_renderer_t *ctx, nu_mesh_handle_t mesh)
{
    return ctx->_api.delete_mesh(ctx, mesh);
}

nu_error_t
nu_texture_create (nu_renderer_t           *ctx,
                   const nu_texture_info_t *info,
                   nu_texture_handle_t     *texture)
{
    return ctx->_api.create_texture(ctx, info, texture);
}
nu_error_t
nu_texture_create_color (nu_renderer_t       *ctx,
                         nu_color_t           color,
                         nu_texture_handle_t *texture)
{
    nu_error_t        error;
    nu_texture_info_t info;
    info.size   = nu_uvec2(1, 1);
    info.usage  = NU_TEXTURE_USAGE_SAMPLE;
    info.format = NU_TEXTURE_FORMAT_COLOR;
    info.colors = &color;
    return nu_texture_create(ctx, &info, texture);
}
nu_error_t
nu_texture_delete (nu_renderer_t *ctx, nu_texture_handle_t texture)
{
    return ctx->_api.delete_texture(ctx, texture);
}

nu_material_info_t
nu_material_info_default (void)
{
    nu_material_info_t info;
    info.texture0     = NU_NULL;
    info.texture1     = NU_NULL;
    info.uv_transform = nu_mat3_identity();
    return info;
}
nu_error_t
nu_material_create (nu_renderer_t            *ctx,
                    const nu_material_info_t *info,
                    nu_material_handle_t     *material)
{
    return ctx->_api.create_material(ctx, info, material);
}
nu_error_t
nu_material_delete (nu_renderer_t *ctx, nu_material_handle_t material)
{
    return ctx->_api.delete_material(ctx, material);
}
nu_error_t
nu_material_update (nu_renderer_t            *ctx,
                    nu_material_handle_t      material,
                    const nu_material_info_t *info)
{
    return ctx->_api.update_material(ctx, material, info);
}

nu_error_t
nu_renderpass_create (nu_renderer_t              *ctx,
                      const nu_renderpass_info_t *info,
                      nu_renderpass_handle_t     *pass)
{
    return ctx->_api.create_renderpass(ctx, info, pass);
}
nu_error_t
nu_renderpass_delete (nu_renderer_t *ctx, nu_renderpass_handle_t pass)
{
    return ctx->_api.delete_renderpass(ctx, pass);
}

void
nu_renderpass_submit (nu_renderer_t                *ctx,
                      nu_renderpass_handle_t        pass,
                      const nu_renderpass_submit_t *info)
{
    ctx->_api.submit_renderpass(ctx, pass, info);
}
void
nu_draw (nu_renderer_t         *ctx,
         nu_renderpass_handle_t pass,
         nu_mesh_handle_t       mesh,
         nu_material_handle_t   material,
         nu_mat4_t              transform)
{
    ctx->_api.draw(ctx, pass, mesh, material, transform);
}

#endif

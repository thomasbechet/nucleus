#ifndef NU_RENDERER_IMPL_H
#define NU_RENDERER_IMPL_H

#include <nucleus/graphics/renderer.h>

#ifdef NU_BUILD_GL
#include <nucleus/gl.h>
#endif

#define NU_TRY_CALL(func) (!func) ? NU_ERROR_NONE : func

nu_error_t
nu_renderer_init (nu_platform_t            *platform,
                  const nu_renderer_info_t *info,
                  nu_renderer_t            *renderer)
{
    renderer->_allocator = info->allocator;

    switch (info->api)
    {
        case NU_RENDERER_NULL:
            nu_memset(&renderer->_api, 0, sizeof(renderer->_api));
            break;
        case NU_RENDERER_GL:
            renderer->_api.init                 = nugl__init;
            renderer->_api.free                 = nugl__free;
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

            renderer->_api.submit_renderpass = nugl__submit_renderpass;
            renderer->_api.reset_renderpass  = nugl__reset_renderpass;
            renderer->_api.draw              = nugl__draw;
            renderer->_api.blit              = nugl__blit;
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
    nu_error_t error
        = NU_TRY_CALL(renderer->_api.init)(renderer, platform->_surface.size);
    NU_ERROR_CHECK(error, return error);

    // Create surface texture
    if (renderer->_api.create_surface_color)
    {
        renderer->_surface_color = renderer->_api.create_surface_color(
            renderer, platform->_surface.size);
    }

    return NU_ERROR_NONE;
}
nu_error_t
nu_renderer_free (nu_renderer_t *renderer)
{
    NU_INFO(&renderer->_logger, "terminate renderer context");
    renderer->_api.free(renderer);

    return NU_ERROR_NONE;
}
nu_error_t
nu_render (nu_platform_t *platform, nu_renderer_t *renderer)
{
    return NU_TRY_CALL(renderer->_api.render)(
        renderer,
        &platform->_surface.glfw.viewport.extent,
        &platform->_surface.glfw.viewport.viewport);
}
nu_texture_t
nu_surface_color_target (const nu_platform_t *platform,
                         const nu_renderer_t *renderer)
{
    return renderer->_surface_color;
}

nu_error_t
nu_camera_create (nu_renderer_t          *ctx,
                  const nu_camera_info_t *info,
                  nu_camera_t            *camera)
{
    return NU_TRY_CALL(ctx->_api.create_camera)(ctx, info, camera);
}
nu_error_t
nu_camera_delete (nu_renderer_t *ctx, nu_camera_t camera)
{
    return NU_TRY_CALL(ctx->_api.delete_camera)(ctx, camera);
}
nu_error_t
nu_camera_update (nu_renderer_t          *ctx,
                  nu_camera_t             camera,
                  const nu_camera_info_t *info)
{
    return NU_TRY_CALL(ctx->_api.update_camera)(ctx, camera, info);
}

nu_error_t
nu_mesh_create (nu_renderer_t *ctx, const nu_mesh_info_t *info, nu_mesh_t *mesh)
{
    return NU_TRY_CALL(ctx->_api.create_mesh)(ctx, info, mesh);
}
nu_error_t
nu_mesh_delete (nu_renderer_t *ctx, nu_mesh_t mesh)
{
    return NU_TRY_CALL(ctx->_api.delete_mesh)(ctx, mesh);
}

nu_error_t
nu_texture_create (nu_renderer_t           *ctx,
                   const nu_texture_info_t *info,
                   nu_texture_t            *texture)
{
    return NU_TRY_CALL(ctx->_api.create_texture)(ctx, info, texture);
}
nu_error_t
nu_texture_create_color (nu_renderer_t *ctx,
                         nu_color_t     color,
                         nu_texture_t  *texture)
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
nu_texture_delete (nu_renderer_t *ctx, nu_texture_t texture)
{
    return NU_TRY_CALL(ctx->_api.delete_texture)(ctx, texture);
}

nu_error_t
nu_cubemap_create (nu_renderer_t           *ctx,
                   const nu_cubemap_info_t *info,
                   nu_cubemap_t            *cubemap)
{
    return NU_TRY_CALL(ctx->_api.create_cubemap)(ctx, info, cubemap);
}
nu_error_t
nu_cubemap_delete (nu_renderer_t *ctx, nu_cubemap_t cubemap)
{
    return NU_TRY_CALL(ctx->_api.delete_cubemap)(ctx, cubemap);
}

nu_error_t
nu_material_create (nu_renderer_t            *ctx,
                    const nu_material_info_t *info,
                    nu_material_t            *material)
{
    return NU_TRY_CALL(ctx->_api.create_material)(ctx, info, material);
}
nu_error_t
nu_material_delete (nu_renderer_t *ctx, nu_material_t material)
{
    return NU_TRY_CALL(ctx->_api.delete_material)(ctx, material);
}
nu_error_t
nu_material_update (nu_renderer_t            *ctx,
                    nu_material_t             material,
                    const nu_material_info_t *info)
{
    return NU_TRY_CALL(ctx->_api.update_material)(ctx, material, info);
}

nu_error_t
nu_renderpass_create (nu_renderer_t              *ctx,
                      const nu_renderpass_info_t *info,
                      nu_renderpass_t            *pass)
{
    return NU_TRY_CALL(ctx->_api.create_renderpass)(ctx, info, pass);
}
nu_error_t
nu_renderpass_delete (nu_renderer_t *ctx, nu_renderpass_t pass)
{
    return NU_TRY_CALL(ctx->_api.delete_renderpass)(ctx, pass);
}

void
nu_renderpass_submit (nu_renderer_t                *ctx,
                      nu_renderpass_t               pass,
                      const nu_renderpass_submit_t *info)
{
    if (ctx->_api.submit_renderpass)
    {
        ctx->_api.submit_renderpass(ctx, pass, info);
    }
}
void
nu_renderpass_reset (nu_renderer_t *ctx, nu_renderpass_t pass)
{
    if (ctx->_api.reset_renderpass)
    {
        ctx->_api.reset_renderpass(ctx, pass);
    }
}
void
nu_draw (nu_renderer_t  *ctx,
         nu_renderpass_t pass,
         nu_material_t   material,
         nu_mesh_t       mesh,
         nu_mat4_t       transform)
{
    if (ctx->_api.draw)
    {
        ctx->_api.draw(ctx, pass, material, mesh, transform);
    }
}
void
nu_blit (nu_renderer_t  *ctx,
         nu_renderpass_t pass,
         nu_material_t   material,
         nu_rect_t       extent,
         nu_rect_t       tex_extent)
{
    if (ctx->_api.blit)
    {
        ctx->_api.blit(ctx, pass, material, extent, tex_extent);
    }
}

#endif

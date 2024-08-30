#ifndef NU_RENDERER_IMPL_H
#define NU_RENDERER_IMPL_H

#include <nucleus/graphics/renderer.h>

#ifdef NU_BUILD_GL
#include <nucleus/gl.h>
#endif

#define NU_TRY_CALL(func) (!func) ? NU_ERROR_NONE : func

nu_error_t
nu_renderer_create (nu_platform_t            *platform,
                    const nu_renderer_info_t *info,
                    nu_renderer_t            *renderer)
{
    renderer->ptr = nu_alloc(info->allocator, sizeof(*renderer->ptr));
    renderer->ptr->_allocator = info->allocator;

    switch (info->api)
    {
        case NU_RENDERER_NULL:
            nu_memset(&renderer->ptr->_api, 0, sizeof(renderer->ptr->_api));
            break;
        case NU_RENDERER_GL:
            renderer->ptr->_api.init   = nugl__init;
            renderer->ptr->_api.free   = nugl__free;
            renderer->ptr->_api.render = nugl__render;
            renderer->ptr->_api.create_surface_color
                = nugl__create_surface_color;

            renderer->ptr->_api.create_camera     = nugl__create_camera;
            renderer->ptr->_api.delete_camera     = nugl__delete_camera;
            renderer->ptr->_api.update_camera     = nugl__update_camera;
            renderer->ptr->_api.create_mesh       = nugl__create_mesh;
            renderer->ptr->_api.delete_mesh       = nugl__delete_mesh;
            renderer->ptr->_api.create_texture    = nugl__create_texture;
            renderer->ptr->_api.delete_texture    = nugl__delete_texture;
            renderer->ptr->_api.create_cubemap    = nugl__create_cubemap;
            renderer->ptr->_api.delete_cubemap    = nugl__delete_cubemap;
            renderer->ptr->_api.create_material   = nugl__create_material;
            renderer->ptr->_api.delete_material   = nugl__delete_material;
            renderer->ptr->_api.update_material   = nugl__update_material;
            renderer->ptr->_api.create_renderpass = nugl__create_renderpass;
            renderer->ptr->_api.delete_renderpass = nugl__delete_renderpass;

            renderer->ptr->_api.submit_renderpass = nugl__submit_renderpass;
            renderer->ptr->_api.reset_renderpass  = nugl__reset_renderpass;
            renderer->ptr->_api.draw_mesh         = nugl__draw_mesh;
            renderer->ptr->_api.draw_blit         = nugl__draw_blit;
            break;
        case NU_RENDERER_DX11:
            break;
        case NU_RENDERER_SOFTRAST:
            break;
    }

    // Initialize logger
    nu_logger_init(&info->logger, &renderer->ptr->_logger);

    // Initialize backend
    NU_INFO(&renderer->ptr->_logger, "initialize renderer context");
    nu_error_t error = NU_TRY_CALL(renderer->ptr->_api.init)(
        renderer->ptr, platform->_surface.size);
    NU_ERROR_CHECK(error, return error);

    // Create surface texture
    if (renderer->ptr->_api.create_surface_color)
    {
        renderer->ptr->_surface_color
            = renderer->ptr->_api.create_surface_color(renderer->ptr,
                                                       platform->_surface.size);
    }

    return NU_ERROR_NONE;
}
nu_error_t
nu_renderer_delete (nu_platform_t *platform, nu_renderer_t renderer)
{
    NU_INFO(&renderer.ptr->_logger, "terminate renderer context");
    renderer.ptr->_api.free(renderer.ptr);
    nu_free(renderer.ptr->_allocator, renderer.ptr, sizeof(*renderer.ptr));

    return NU_ERROR_NONE;
}
nu_error_t
nu_renderer_render (nu_platform_t *platform, nu_renderer_t renderer)
{
    return NU_TRY_CALL(renderer.ptr->_api.render)(
        renderer.ptr,
        &platform->_surface.glfw.viewport.extent,
        &platform->_surface.glfw.viewport.viewport);
}
nu_texture_handle_t
nu_surface_color_target (const nu_platform_t *platform, nu_renderer_t renderer)
{
    return renderer.ptr->_surface_color;
}

nu_error_t
nu_camera_create (nu_renderer_t           renderer,
                  const nu_camera_info_t *info,
                  nu_camera_handle_t     *camera)
{
    return NU_TRY_CALL(renderer.ptr->_api.create_camera)(
        renderer.ptr, info, camera);
}
nu_error_t
nu_camera_delete (nu_renderer_t renderer, nu_camera_handle_t camera)
{
    return NU_TRY_CALL(renderer.ptr->_api.delete_camera)(renderer.ptr, camera);
}
nu_error_t
nu_camera_update (nu_renderer_t           renderer,
                  nu_camera_handle_t      camera,
                  const nu_camera_info_t *info)
{
    return NU_TRY_CALL(renderer.ptr->_api.update_camera)(
        renderer.ptr, camera, info);
}

nu_error_t
nu_mesh_create (nu_renderer_t         renderer,
                const nu_mesh_info_t *info,
                nu_mesh_handle_t     *mesh)
{
    return NU_TRY_CALL(renderer.ptr->_api.create_mesh)(
        renderer.ptr, info, mesh);
}
nu_error_t
nu_mesh_delete (nu_renderer_t renderer, nu_mesh_handle_t mesh)
{
    return NU_TRY_CALL(renderer.ptr->_api.delete_mesh)(renderer.ptr, mesh);
}

nu_error_t
nu_texture_create (nu_renderer_t            renderer,
                   const nu_texture_info_t *info,
                   nu_texture_handle_t     *texture)
{
    return NU_TRY_CALL(renderer.ptr->_api.create_texture)(
        renderer.ptr, info, texture);
}
nu_error_t
nu_texture_create_color (nu_renderer_t        renderer,
                         nu_color_t           color,
                         nu_texture_handle_t *texture)
{
    nu_error_t        error;
    nu_texture_info_t info;
    info.size   = nu_uvec2(1, 1);
    info.usage  = NU_TEXTURE_USAGE_SAMPLE;
    info.format = NU_TEXTURE_FORMAT_COLOR;
    info.colors = &color;
    return nu_texture_create(renderer, &info, texture);
}
nu_error_t
nu_texture_delete (nu_renderer_t renderer, nu_texture_handle_t texture)
{
    return NU_TRY_CALL(renderer.ptr->_api.delete_texture)(renderer.ptr,
                                                          texture);
}

nu_error_t
nu_cubemap_create (nu_renderer_t            renderer,
                   const nu_cubemap_info_t *info,
                   nu_cubemap_handle_t     *cubemap)
{
    return NU_TRY_CALL(renderer.ptr->_api.create_cubemap)(
        renderer.ptr, info, cubemap);
}
nu_error_t
nu_cubemap_delete (nu_renderer_t renderer, nu_cubemap_handle_t cubemap)
{
    return NU_TRY_CALL(renderer.ptr->_api.delete_cubemap)(renderer.ptr,
                                                          cubemap);
}

nu_error_t
nu_material_create (nu_renderer_t             renderer,
                    const nu_material_info_t *info,
                    nu_material_handle_t     *material)
{
    return NU_TRY_CALL(renderer.ptr->_api.create_material)(
        renderer.ptr, info, material);
}
nu_error_t
nu_material_delete (nu_renderer_t renderer, nu_material_handle_t material)
{
    return NU_TRY_CALL(renderer.ptr->_api.delete_material)(renderer.ptr,
                                                           material);
}
nu_error_t
nu_material_update (nu_renderer_t             renderer,
                    nu_material_handle_t      material,
                    const nu_material_info_t *info)
{
    return NU_TRY_CALL(renderer.ptr->_api.update_material)(
        renderer.ptr, material, info);
}

nu_error_t
nu_renderpass_create (nu_renderer_t               renderer,
                      const nu_renderpass_info_t *info,
                      nu_renderpass_handle_t     *pass)
{
    return NU_TRY_CALL(renderer.ptr->_api.create_renderpass)(
        renderer.ptr, info, pass);
}
nu_error_t
nu_renderpass_delete (nu_renderer_t renderer, nu_renderpass_handle_t pass)
{
    return NU_TRY_CALL(renderer.ptr->_api.delete_renderpass)(renderer.ptr,
                                                             pass);
}

void
nu_renderpass_submit (nu_renderer_t                 renderer,
                      nu_renderpass_handle_t        pass,
                      const nu_renderpass_submit_t *info)
{
    if (renderer.ptr->_api.submit_renderpass)
    {
        renderer.ptr->_api.submit_renderpass(renderer.ptr, pass, info);
    }
}
void
nu_renderpass_reset (nu_renderer_t renderer, nu_renderpass_handle_t pass)
{
    if (renderer.ptr->_api.reset_renderpass)
    {
        renderer.ptr->_api.reset_renderpass(renderer.ptr, pass);
    }
}
void
nu_draw_mesh (nu_renderer_t          renderer,
              nu_renderpass_handle_t pass,
              nu_material_handle_t   material,
              nu_mesh_handle_t       mesh,
              nu_mat4_t              transform)
{
    if (renderer.ptr->_api.draw_mesh)
    {
        renderer.ptr->_api.draw_mesh(
            renderer.ptr, pass, material, mesh, transform);
    }
}
void
nu_draw_blit (nu_renderer_t          renderer,
              nu_renderpass_handle_t pass,
              nu_material_handle_t   material,
              nu_rect_t              extent,
              nu_rect_t              tex_extent)
{
    if (renderer.ptr->_api.draw_blit)
    {
        renderer.ptr->_api.draw_blit(
            renderer.ptr, pass, material, extent, tex_extent);
    }
}

#endif

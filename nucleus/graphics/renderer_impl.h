#ifndef NU_RENDERER_IMPL_H
#define NU_RENDERER_IMPL_H

#include <nucleus/graphics/renderer.h>

#ifdef NU_BUILD_GL
#include <nucleus/gl.h>
#endif

#define NU_TRY_CALL(func) (!func) ? NU_ERROR_NONE : func

nu_error_t
nu_renderer_create (const nu_renderer_info_t *info, nu_renderer_t *renderer)
{
    renderer->_ptr
        = (nu__renderer_t *)nu_alloc(info->allocator, sizeof(*renderer->_ptr));
    renderer->_ptr->allocator = info->allocator;
    renderer->_ptr->platform  = info->platform;
    renderer->_ptr->logger    = info->logger;

    switch (info->api)
    {
        case NU_RENDERER_NULL:
            nu_memset(&renderer->_ptr->api, 0, sizeof(renderer->_ptr->api));
            break;
        case NU_RENDERER_GL:
            renderer->_ptr->api.init   = nugl__init;
            renderer->_ptr->api.free   = nugl__free;
            renderer->_ptr->api.render = nugl__render;
            renderer->_ptr->api.create_surface_color
                = nugl__create_surface_color;

            renderer->_ptr->api.create_camera     = nugl__create_camera;
            renderer->_ptr->api.delete_camera     = nugl__delete_camera;
            renderer->_ptr->api.update_camera     = nugl__update_camera;
            renderer->_ptr->api.create_mesh       = nugl__create_mesh;
            renderer->_ptr->api.delete_mesh       = nugl__delete_mesh;
            renderer->_ptr->api.create_texture    = nugl__create_texture;
            renderer->_ptr->api.delete_texture    = nugl__delete_texture;
            renderer->_ptr->api.create_cubemap    = nugl__create_cubemap;
            renderer->_ptr->api.delete_cubemap    = nugl__delete_cubemap;
            renderer->_ptr->api.create_material   = nugl__create_material;
            renderer->_ptr->api.delete_material   = nugl__delete_material;
            renderer->_ptr->api.update_material   = nugl__update_material;
            renderer->_ptr->api.create_renderpass = nugl__create_renderpass;
            renderer->_ptr->api.delete_renderpass = nugl__delete_renderpass;

            renderer->_ptr->api.submit_renderpass = nugl__submit_renderpass;
            renderer->_ptr->api.reset_renderpass  = nugl__reset_renderpass;
            renderer->_ptr->api.draw_mesh         = nugl__draw_mesh;
            renderer->_ptr->api.draw_blit         = nugl__draw_blit;
            break;
        case NU_RENDERER_DX11:
            break;
        case NU_RENDERER_SOFTRAST:
            break;
    }

    // Initialize backend
    NU_INFO(renderer->_ptr->logger, "initialize renderer context");
    nu_error_t error = NU_TRY_CALL(renderer->_ptr->api.init)(
        renderer->_ptr, renderer->_ptr->platform._ptr->surface.size);
    NU_ERROR_CHECK(error, return error);

    // Create surface texture
    if (renderer->_ptr->api.create_surface_color)
    {
        renderer->_ptr->surface_color
            = renderer->_ptr->api.create_surface_color(
                renderer->_ptr, renderer->_ptr->platform._ptr->surface.size);
    }

    return NU_ERROR_NONE;
}
nu_error_t
nu_renderer_delete (nu_renderer_t renderer)
{
    NU_INFO(renderer._ptr->logger, "terminate renderer context");
    renderer._ptr->api.free(renderer._ptr);
    nu_free(renderer._ptr->allocator, renderer._ptr, sizeof(*renderer._ptr));

    return NU_ERROR_NONE;
}
nu_error_t
nu_renderer_render (nu_renderer_t renderer)
{
    return NU_TRY_CALL(renderer._ptr->api.render)(
        renderer._ptr,
        &renderer._ptr->platform._ptr->surface.glfw.viewport.extent,
        &renderer._ptr->platform._ptr->surface.glfw.viewport.viewport);
}
nu_texture_t
nu_surface_color_target (nu_renderer_t renderer)
{
    return renderer._ptr->surface_color;
}

nu_camera_info_t
nu_camera_info (void)
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
nu_camera_create (nu_renderer_t           renderer,
                  const nu_camera_info_t *info,
                  nu_camera_t            *camera)
{
    return NU_TRY_CALL(renderer._ptr->api.create_camera)(
        renderer._ptr, info, camera);
}
nu_error_t
nu_camera_delete (nu_renderer_t renderer, nu_camera_t camera)
{
    return NU_TRY_CALL(renderer._ptr->api.delete_camera)(renderer._ptr, camera);
}
nu_error_t
nu_camera_update (nu_renderer_t           renderer,
                  nu_camera_t             camera,
                  const nu_camera_info_t *info)
{
    return NU_TRY_CALL(renderer._ptr->api.update_camera)(
        renderer._ptr, camera, info);
}

nu_error_t
nu_mesh_create (nu_renderer_t         renderer,
                const nu_mesh_info_t *info,
                nu_mesh_t            *mesh)
{
    return NU_TRY_CALL(renderer._ptr->api.create_mesh)(
        renderer._ptr, info, mesh);
}
nu_error_t
nu_mesh_delete (nu_renderer_t renderer, nu_mesh_t mesh)
{
    return NU_TRY_CALL(renderer._ptr->api.delete_mesh)(renderer._ptr, mesh);
}

nu_error_t
nu_texture_create (nu_renderer_t            renderer,
                   const nu_texture_info_t *info,
                   nu_texture_t            *texture)
{
    return NU_TRY_CALL(renderer._ptr->api.create_texture)(
        renderer._ptr, info, texture);
}
nu_error_t
nu_texture_create_color (nu_renderer_t renderer,
                         nu_color_t    color,
                         nu_texture_t *texture)
{
    nu_texture_info_t info;
    info.size   = nu_uvec2(1, 1);
    info.usage  = NU_TEXTURE_USAGE_SAMPLE;
    info.format = NU_TEXTURE_FORMAT_COLOR;
    info.colors = &color;
    return nu_texture_create(renderer, &info, texture);
}
nu_error_t
nu_texture_create_image (nu_renderer_t renderer,
                         nu_image_t    image,
                         nu_texture_t *texture)
{
    nu_texture_info_t info = nu_image_texture_info(image);
    return nu_texture_create(renderer, &info, texture);
}
nu_error_t
nu_texture_delete (nu_renderer_t renderer, nu_texture_t texture)
{
    return NU_TRY_CALL(renderer._ptr->api.delete_texture)(renderer._ptr,
                                                          texture);
}

nu_error_t
nu_cubemap_create (nu_renderer_t            renderer,
                   const nu_cubemap_info_t *info,
                   nu_cubemap_t            *cubemap)
{
    return NU_TRY_CALL(renderer._ptr->api.create_cubemap)(
        renderer._ptr, info, cubemap);
}
nu_error_t
nu_cubemap_delete (nu_renderer_t renderer, nu_cubemap_t cubemap)
{
    return NU_TRY_CALL(renderer._ptr->api.delete_cubemap)(renderer._ptr,
                                                          cubemap);
}

nu_material_info_t
nu_material_info_default (nu_material_type_t type)
{
    nu_material_info_t info;
    info.type = type;
    switch (info.type)
    {
        case NU_MATERIAL_MESH:
            info.mesh.color0       = NU_NULL;
            info.mesh.color1       = NU_NULL;
            info.mesh.uv_transform = nu_mat3_identity();
            break;
        case NU_MATERIAL_CANVAS:
            info.canvas.color0    = NU_NULL;
            info.canvas.wrap_mode = NU_TEXTURE_WRAP_CLAMP;
            break;
    }
    return info;
}
nu_error_t
nu_material_create (nu_renderer_t             renderer,
                    const nu_material_info_t *info,
                    nu_material_t            *material)
{
    return NU_TRY_CALL(renderer._ptr->api.create_material)(
        renderer._ptr, info, material);
}
nu_error_t
nu_material_delete (nu_renderer_t renderer, nu_material_t material)
{
    return NU_TRY_CALL(renderer._ptr->api.delete_material)(renderer._ptr,
                                                           material);
}
nu_error_t
nu_material_update (nu_renderer_t             renderer,
                    nu_material_t             material,
                    const nu_material_info_t *info)
{
    return NU_TRY_CALL(renderer._ptr->api.update_material)(
        renderer._ptr, material, info);
}

nu_error_t
nu_renderpass_create (nu_renderer_t               renderer,
                      const nu_renderpass_info_t *info,
                      nu_renderpass_t            *pass)
{
    return NU_TRY_CALL(renderer._ptr->api.create_renderpass)(
        renderer._ptr, info, pass);
}
nu_error_t
nu_renderpass_delete (nu_renderer_t renderer, nu_renderpass_t pass)
{
    return NU_TRY_CALL(renderer._ptr->api.delete_renderpass)(renderer._ptr,
                                                             pass);
}

void
nu_renderpass_submit (nu_renderer_t                 renderer,
                      nu_renderpass_t               pass,
                      const nu_renderpass_submit_t *info)
{
    if (renderer._ptr->api.submit_renderpass)
    {
        renderer._ptr->api.submit_renderpass(renderer._ptr, pass, info);
    }
}
void
nu_renderpass_reset (nu_renderer_t renderer, nu_renderpass_t pass)
{
    if (renderer._ptr->api.reset_renderpass)
    {
        renderer._ptr->api.reset_renderpass(renderer._ptr, pass);
    }
}
void
nu_draw_mesh (nu_renderer_t   renderer,
              nu_renderpass_t pass,
              nu_material_t   material,
              nu_mesh_t       mesh,
              nu_mat4_t       transform)
{
    if (renderer._ptr->api.draw_mesh)
    {
        renderer._ptr->api.draw_mesh(
            renderer._ptr, pass, material, mesh, transform);
    }
}
void
nu_draw_blit (nu_renderer_t   renderer,
              nu_renderpass_t pass,
              nu_material_t   material,
              nu_rect_t       extent,
              nu_rect_t       tex_extent)
{
    if (renderer._ptr->api.draw_blit)
    {
        renderer._ptr->api.draw_blit(
            renderer._ptr, pass, material, extent, tex_extent);
    }
}

#endif

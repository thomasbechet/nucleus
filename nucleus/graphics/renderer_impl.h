#ifndef NU_RENDERER_IMPL_H
#define NU_RENDERER_IMPL_H

#include <nucleus/internal.h>

#define NU_TRY_CALL(func) (!func) ? NU_ERROR_NONE : func

nu_error_t
nu_renderer_render (void)
{
    return NU_TRY_CALL(_ctx.graphics.renderer.api.render)(
        &_ctx.platform.surface.glfw.viewport.extent,
        &_ctx.platform.surface.glfw.viewport.viewport);
}
nu_texture_t
nu_surface_color_target (void)
{
    return _ctx.graphics.renderer.surface_color;
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
nu_camera_create (const nu_camera_info_t *info, nu_camera_t *camera)
{
    return NU_TRY_CALL(_ctx.graphics.renderer.api.create_camera)(info, camera);
}
nu_error_t
nu_camera_delete (nu_camera_t camera)
{
    return NU_TRY_CALL(_ctx.graphics.renderer.api.delete_camera)(camera);
}
nu_error_t
nu_camera_update (nu_camera_t camera, const nu_camera_info_t *info)
{
    return NU_TRY_CALL(_ctx.graphics.renderer.api.update_camera)(camera, info);
}

nu_error_t
nu_mesh_create (const nu_mesh_info_t *info, nu_mesh_t *mesh)
{
    return NU_TRY_CALL(_ctx.graphics.renderer.api.create_mesh)(info, mesh);
}
nu_error_t
nu_mesh_delete (nu_mesh_t mesh)
{
    return NU_TRY_CALL(_ctx.graphics.renderer.api.delete_mesh)(mesh);
}

nu_error_t
nu_texture_create (const nu_texture_info_t *info, nu_texture_t *texture)
{
    return NU_TRY_CALL(_ctx.graphics.renderer.api.create_texture)(info,
                                                                  texture);
}
nu_error_t
nu_texture_create_color (nu_color_t color, nu_texture_t *texture)
{
    nu_texture_info_t info;
    info.size   = nu_uvec2(1, 1);
    info.usage  = NU_TEXTURE_USAGE_SAMPLE;
    info.format = NU_TEXTURE_FORMAT_COLOR;
    info.colors = &color;
    return nu_texture_create(&info, texture);
}
nu_error_t
nu_texture_create_image (nu_image_t image, nu_texture_t *texture)
{
    nu_texture_info_t info = nu_image_texture_info(image);
    return nu_texture_create(&info, texture);
}
nu_error_t
nu_texture_delete (nu_texture_t texture)
{
    return NU_TRY_CALL(_ctx.graphics.renderer.api.delete_texture)(texture);
}

nu_error_t
nu_cubemap_create (const nu_cubemap_info_t *info, nu_cubemap_t *cubemap)
{
    return NU_TRY_CALL(_ctx.graphics.renderer.api.create_cubemap)(info,
                                                                  cubemap);
}
nu_error_t
nu_cubemap_delete (nu_cubemap_t cubemap)
{
    return NU_TRY_CALL(_ctx.graphics.renderer.api.delete_cubemap)(cubemap);
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
nu_material_create (const nu_material_info_t *info, nu_material_t *material)
{
    return NU_TRY_CALL(_ctx.graphics.renderer.api.create_material)(info,
                                                                   material);
}
nu_error_t
nu_material_delete (nu_material_t material)
{
    return NU_TRY_CALL(_ctx.graphics.renderer.api.delete_material)(material);
}
nu_error_t
nu_material_update (nu_material_t material, const nu_material_info_t *info)
{
    return NU_TRY_CALL(_ctx.graphics.renderer.api.update_material)(material,
                                                                   info);
}

nu_error_t
nu_renderpass_create (const nu_renderpass_info_t *info, nu_renderpass_t *pass)
{
    return NU_TRY_CALL(_ctx.graphics.renderer.api.create_renderpass)(info,
                                                                     pass);
}
nu_error_t
nu_renderpass_delete (nu_renderpass_t pass)
{
    return NU_TRY_CALL(_ctx.graphics.renderer.api.delete_renderpass)(pass);
}

void
nu_renderpass_submit (nu_renderpass_t pass, const nu_renderpass_submit_t *info)
{
    if (_ctx.graphics.renderer.api.submit_renderpass)
    {
        _ctx.graphics.renderer.api.submit_renderpass(pass, info);
    }
}
void
nu_renderpass_reset (nu_renderpass_t pass)
{
    if (_ctx.graphics.renderer.api.reset_renderpass)
    {
        _ctx.graphics.renderer.api.reset_renderpass(pass);
    }
}
void
nu_draw_mesh (nu_renderpass_t pass,
              nu_material_t   material,
              nu_mesh_t       mesh,
              nu_mat4_t       transform)
{
    if (_ctx.graphics.renderer.api.draw_mesh)
    {
        _ctx.graphics.renderer.api.draw_mesh(pass, material, mesh, transform);
    }
}
void
nu_draw_blit (nu_renderpass_t pass,
              nu_material_t   material,
              nu_rect_t       extent,
              nu_rect_t       tex_extent)
{
    if (_ctx.graphics.renderer.api.draw_blit)
    {
        _ctx.graphics.renderer.api.draw_blit(
            pass, material, extent, tex_extent);
    }
}

#endif

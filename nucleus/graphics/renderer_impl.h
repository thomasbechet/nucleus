#ifndef NU_RENDERER_IMPL_H
#define NU_RENDERER_IMPL_H

#include <nucleus/internal.h>

#define CHECK_NULL_API_HANDLE(type)      \
    if (_ctx.graphics.renderer.null_api) \
    {                                    \
        return NU_HANDLE_NULL(type);     \
    }
#define CHECK_NULL_API_ERROR             \
    if (_ctx.graphics.renderer.null_api) \
    {                                    \
        return NU_ERROR_NONE;            \
    }
#define CHECK_NULL_API_VOID              \
    if (_ctx.graphics.renderer.null_api) \
    {                                    \
        return;                          \
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
nu_camera_t
nu_camera_create (const nu_camera_info_t *info)
{
    CHECK_NULL_API_HANDLE(nu_camera_t)
    return _ctx.graphics.renderer.api.create_camera(info);
}
nu_error_t
nu_camera_delete (nu_camera_t camera)
{
    CHECK_NULL_API_ERROR
    return _ctx.graphics.renderer.api.delete_camera(camera);
}
nu_error_t
nu_camera_update (nu_camera_t camera, const nu_camera_info_t *info)
{
    CHECK_NULL_API_ERROR
    return _ctx.graphics.renderer.api.update_camera(camera, info);
}

nu_mesh_t
nu_mesh_create (const nu_mesh_info_t *info)
{
    CHECK_NULL_API_HANDLE(nu_mesh_t)
    return _ctx.graphics.renderer.api.create_mesh(info);
}
nu_error_t
nu_mesh_delete (nu_mesh_t mesh)
{
    CHECK_NULL_API_ERROR
    return _ctx.graphics.renderer.api.delete_mesh(mesh);
}

nu_texture_t
nu_texture_create (const nu_texture_info_t *info)
{
    CHECK_NULL_API_HANDLE(nu_texture_t)
    return _ctx.graphics.renderer.api.create_texture(info);
}
nu_texture_t
nu_texture_create_color (nu_color_t color)
{
    nu_texture_info_t info;
    info.size   = nu_uvec2(1, 1);
    info.usage  = NU_TEXTURE_USAGE_SAMPLE;
    info.format = NU_TEXTURE_FORMAT_COLOR;
    info.colors = &color;
    return nu_texture_create(&info);
}
nu_texture_t
nu_texture_create_image (nu_image_t image)
{
    nu_texture_info_t info = nu_image_texture_info(image);
    return nu_texture_create(&info);
}
nu_error_t
nu_texture_delete (nu_texture_t texture)
{
    CHECK_NULL_API_ERROR
    return _ctx.graphics.renderer.api.delete_texture(texture);
}

nu_cubemap_t
nu_cubemap_create (const nu_cubemap_info_t *info)
{
    CHECK_NULL_API_HANDLE(nu_cubemap_t)
    return _ctx.graphics.renderer.api.create_cubemap(info);
}
nu_error_t
nu_cubemap_delete (nu_cubemap_t cubemap)
{
    CHECK_NULL_API_ERROR
    return _ctx.graphics.renderer.api.delete_cubemap(cubemap);
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
nu_material_t
nu_material_create (const nu_material_info_t *info)
{
    CHECK_NULL_API_HANDLE(nu_material_t)
    return _ctx.graphics.renderer.api.create_material(info);
}
nu_error_t
nu_material_delete (nu_material_t material)
{
    CHECK_NULL_API_ERROR
    return _ctx.graphics.renderer.api.delete_material(material);
}
nu_error_t
nu_material_update (nu_material_t material, const nu_material_info_t *info)
{
    CHECK_NULL_API_ERROR
    return _ctx.graphics.renderer.api.update_material(material, info);
}

nu_renderpass_t
nu_renderpass_create (const nu_renderpass_info_t *info)
{
    CHECK_NULL_API_HANDLE(nu_renderpass_t)
    return _ctx.graphics.renderer.api.create_renderpass(info);
}
nu_error_t
nu_renderpass_delete (nu_renderpass_t pass)
{
    CHECK_NULL_API_ERROR
    return _ctx.graphics.renderer.api.delete_renderpass(pass);
}

void
nu_renderpass_submit (nu_renderpass_t pass, const nu_renderpass_submit_t *info)
{
    CHECK_NULL_API_VOID
    _ctx.graphics.renderer.api.submit_renderpass(pass, info);
}
void
nu_renderpass_reset (nu_renderpass_t pass)
{
    CHECK_NULL_API_VOID
    _ctx.graphics.renderer.api.reset_renderpass(pass);
}
void
nu_draw_mesh (nu_renderpass_t pass,
              nu_material_t   material,
              nu_mesh_t       mesh,
              nu_mat4_t       transform)
{
    CHECK_NULL_API_VOID
    _ctx.graphics.renderer.api.draw_mesh(pass, material, mesh, transform);
}
void
nu_draw_blit (nu_renderpass_t pass,
              nu_material_t   material,
              nu_rect_t       extent,
              nu_rect_t       tex_extent)
{
    CHECK_NULL_API_VOID
    _ctx.graphics.renderer.api.draw_blit(pass, material, extent, tex_extent);
}

#undef CHECK_NULL_API_HANDLE
#undef CHECK_NULL_API_ERROR
#undef CHECK_NULL_API_VOID

#endif

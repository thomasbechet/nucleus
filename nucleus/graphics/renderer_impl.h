#ifndef NU_RENDERER_IMPL_H
#define NU_RENDERER_IMPL_H

#include <nucleus/internal.h>

#define CHECK_NULL_API_HANDLE            \
    if (_ctx.graphics.renderer.null_api) \
    {                                    \
        return NU_NULL;                  \
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

void
nu_config_renderer_api (nu_renderer_api_t api)
{
    nu__config()->graphics.api = api;
}

nu_texture_t
nu_surface_color_target (void)
{
    return _ctx.graphics.renderer.surface_color;
}

nu_camera_t
nu_camera_create (void)
{
    CHECK_NULL_API_HANDLE
    nu_camera_t handle = _ctx.graphics.renderer.api.camera_create();
    NU_CHECK(handle, return handle);
    nu_camera_view(handle,
                   nu_lookat(NU_VEC3_UP, NU_VEC3_FORWARD, NU_VEC3_ZERO));
    float aspect = (float)_ctx.platform.size.x / (float)_ctx.platform.size.y;
    nu_camera_proj(handle, nu_perspective(nu_radian(70), aspect, 0.01, 100));
    return handle;
}
void
nu_camera_delete (nu_camera_t camera)
{
    CHECK_NULL_API_VOID
    _ctx.graphics.renderer.api.camera_delete(camera);
}
void
nu_camera_view (nu_camera_t camera, nu_mat4_t view)
{
    CHECK_NULL_API_VOID
    _ctx.graphics.renderer.api.camera_view(camera, view);
}
void
nu_camera_proj (nu_camera_t camera, nu_mat4_t proj)
{
    CHECK_NULL_API_VOID
    _ctx.graphics.renderer.api.camera_proj(camera, proj);
}

nu_mesh_t
nu_mesh_create (nu_size_t count)
{
    CHECK_NULL_API_HANDLE
    return _ctx.graphics.renderer.api.mesh_create(count);
}
void
nu_mesh_delete (nu_mesh_t mesh)
{
    CHECK_NULL_API_VOID
    _ctx.graphics.renderer.api.mesh_delete(mesh);
}
void
nu_mesh_update (nu_mesh_t        mesh,
                const nu_vec3_t *positions,
                const nu_vec2_t *uvs,
                const nu_vec3_t *normals)
{
    CHECK_NULL_API_VOID
    _ctx.graphics.renderer.api.mesh_update(mesh, positions, uvs, normals);
}
void
nu_mesh_uvs (nu_mesh_t mesh, const nu_vec2_t *uvs)
{
    CHECK_NULL_API_VOID
    _ctx.graphics.renderer.api.mesh_update(mesh, NU_NULL, uvs, NU_NULL);
}
void
nu_mesh_normals (nu_mesh_t mesh, const nu_vec3_t *normals)
{
    CHECK_NULL_API_VOID
    _ctx.graphics.renderer.api.mesh_update(mesh, NU_NULL, NU_NULL, normals);
}

nu_texture_t
nu_texture_create (nu_uvec2_t          size,
                   nu_texture_format_t format,
                   nu_texture_usage_t  usage,
                   const nu_color_t   *colors)
{
    CHECK_NULL_API_HANDLE
    return _ctx.graphics.renderer.api.texture_create(
        size, format, usage, colors);
}
nu_texture_t
nu_texture_create_color (nu_color_t color)
{
    return nu_texture_create(nu_uvec2(1, 1),
                             NU_TEXTURE_FORMAT_COLOR,
                             NU_TEXTURE_USAGE_SAMPLE,
                             &color);
}
nu_texture_t
nu_texture_create_image (nu_image_t image)
{
    nu__image_t *ima = &_ctx.graphics.images.data[NU_HANDLE_INDEX(image)];
    return nu_texture_create(ima->size,
                             NU_TEXTURE_FORMAT_COLOR,
                             NU_TEXTURE_USAGE_SAMPLE,
                             ima->colors);
}
void
nu_texture_delete (nu_texture_t texture)
{
    CHECK_NULL_API_VOID
    _ctx.graphics.renderer.api.texture_delete(texture);
}

nu_cubemap_t
nu_cubemap_create (const nu_cubemap_info_t *info)
{
    CHECK_NULL_API_HANDLE
    return _ctx.graphics.renderer.api.cubemap_create(info);
}
void
nu_cubemap_delete (nu_cubemap_t cubemap)
{
    CHECK_NULL_API_VOID
    _ctx.graphics.renderer.api.cubemap_delete(cubemap);
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
    CHECK_NULL_API_HANDLE
    return _ctx.graphics.renderer.api.material_create(info);
}
void
nu_material_delete (nu_material_t material)
{
    CHECK_NULL_API_VOID
    _ctx.graphics.renderer.api.material_delete(material);
}
nu_error_t
nu_material_update (nu_material_t material, const nu_material_info_t *info)
{
    CHECK_NULL_API_ERROR
    return _ctx.graphics.renderer.api.material_update(material, info);
}

nu_renderpass_t
nu_renderpass_create (nu_renderpass_type_t type, nu_bool_t reset_after_submit)
{
    CHECK_NULL_API_HANDLE
    return _ctx.graphics.renderer.api.renderpass_create(type,
                                                        reset_after_submit);
}
void
nu_renderpass_delete (nu_renderpass_t pass)
{
    CHECK_NULL_API_VOID
    _ctx.graphics.renderer.api.renderpass_delete(pass);
}
void
nu_renderpass_clear_color (nu_renderpass_t pass, nu_color_t *color)
{
    CHECK_NULL_API_VOID
    _ctx.graphics.renderer.api.renderpass_clear_color(pass, color);
}
void
nu_renderpass_camera (nu_renderpass_t pass, nu_camera_t camera)
{
    CHECK_NULL_API_VOID
    _ctx.graphics.renderer.api.renderpass_camera(pass, camera);
}
void
nu_renderpass_skybox_cubemap (nu_renderpass_t pass, nu_cubemap_t cubemap)
{
    CHECK_NULL_API_VOID
    _ctx.graphics.renderer.api.renderpass_skybox_cubemap(pass, cubemap);
}
void
nu_renderpass_skybox_rotation (nu_renderpass_t pass, nu_quat_t rot)
{
    CHECK_NULL_API_VOID
    _ctx.graphics.renderer.api.renderpass_skybox_rotation(pass, rot);
}
void
nu_renderpass_target_color (nu_renderpass_t pass, nu_texture_t color)
{
    CHECK_NULL_API_VOID
    _ctx.graphics.renderer.api.renderpass_target_color(pass, color);
}
void
nu_renderpass_target_depth (nu_renderpass_t pass, nu_texture_t depth)
{
    CHECK_NULL_API_VOID
    _ctx.graphics.renderer.api.renderpass_target_depth(pass, depth);
}
void
nu_renderpass_reset (nu_renderpass_t pass)
{
    CHECK_NULL_API_VOID
    _ctx.graphics.renderer.api.renderpass_reset(pass);
}
void
nu_renderpass_submit (nu_renderpass_t pass)
{
    CHECK_NULL_API_VOID
    _ctx.graphics.renderer.api.renderpass_submit(pass);
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

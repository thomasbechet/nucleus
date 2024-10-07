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
    nu_camera_set_view(handle,
                       nu_lookat(NU_VEC3_UP, NU_VEC3_FORWARD, NU_VEC3_ZEROS));
    float aspect = (float)_ctx.platform.size.x / (float)_ctx.platform.size.y;
    nu_camera_set_proj(handle,
                       nu_perspective(nu_radian(70), aspect, 0.01, 100));
    return handle;
}
void
nu_camera_delete (nu_camera_t camera)
{
    CHECK_NULL_API_VOID
    _ctx.graphics.renderer.api.camera_delete(camera);
}
void
nu_camera_set_view (nu_camera_t camera, nu_mat4_t view)
{
    CHECK_NULL_API_VOID
    _ctx.graphics.renderer.api.camera_set_view(camera, view);
}
void
nu_camera_set_proj (nu_camera_t camera, nu_mat4_t proj)
{
    CHECK_NULL_API_VOID
    _ctx.graphics.renderer.api.camera_set_proj(camera, proj);
}

nu_mesh_t
nu_mesh_create (nu_primitive_t primitive, nu_size_t count)
{
    CHECK_NULL_API_HANDLE
    return _ctx.graphics.renderer.api.mesh_create(primitive, count);
}
void
nu_mesh_delete (nu_mesh_t mesh)
{
    CHECK_NULL_API_VOID
    _ctx.graphics.renderer.api.mesh_delete(mesh);
}
void
nu_mesh_write_uvs (nu_mesh_t        mesh,
                   nu_size_t        first,
                   nu_size_t        count,
                   const nu_vec2_t *data)
{
    CHECK_NULL_API_VOID
    _ctx.graphics.renderer.api.mesh_write_uvs(mesh, first, count, data);
}
void
nu_mesh_write_positions (nu_mesh_t        mesh,
                         nu_size_t        first,
                         nu_size_t        count,
                         const nu_vec3_t *data)
{
    CHECK_NULL_API_VOID
    _ctx.graphics.renderer.api.mesh_write_positions(mesh, first, count, data);
}
void
nu_mesh_write_colors (nu_mesh_t         mesh,
                      nu_size_t         first,
                      nu_size_t         count,
                      const nu_color_t *data)
{
}

nu_texture_t
nu_texture_create (nu_vec2u_t size, nu_texture_type_t usage)
{
    CHECK_NULL_API_HANDLE
    return _ctx.graphics.renderer.api.texture_create(size, usage);
}
nu_texture_t
nu_texture_create_color (nu_color_t color)
{
    nu_texture_t tex = nu_texture_create(NU_VEC2U_ONES, NU_TEXTURE_COLOR);
    NU_CHECK(tex, return tex);
    nu_texture_write_colors(tex, &color);
    return tex;
}
nu_texture_t
nu_image_create_texture (nu_image_t image)
{
    nu__image_t *ima = &_ctx.graphics.images.data[NU_HANDLE_INDEX(image)];
    nu_texture_t tex = nu_texture_create(ima->size, NU_TEXTURE_COLOR);
    NU_CHECK(tex, return tex);
    nu_texture_write_colors(tex, ima->colors);
    return tex;
}
void
nu_texture_delete (nu_texture_t texture)
{
    CHECK_NULL_API_VOID
    _ctx.graphics.renderer.api.texture_delete(texture);
}
void
nu_texture_write_colors (nu_texture_t texture, const nu_color_t *colors)
{
    CHECK_NULL_API_VOID
    _ctx.graphics.renderer.api.texture_write_colors(texture, colors);
}

NU_API nu_cubemap_t
nu_cubemap_create (nu_u32_t size)
{
    CHECK_NULL_API_HANDLE
    return _ctx.graphics.renderer.api.cubemap_create(size);
}
void
nu_cubemap_delete (nu_cubemap_t cubemap)
{
    CHECK_NULL_API_VOID
    _ctx.graphics.renderer.api.cubemap_delete(cubemap);
}
void
nu_cubemap_write_colors (nu_cubemap_t      cubemap,
                         nu_cubemap_face_t face,
                         const nu_color_t *colors)
{
    CHECK_NULL_API_VOID
    _ctx.graphics.renderer.api.cubemap_write_colors(cubemap, face, colors);
}

nu_material_t
nu_material_create (nu_material_type_t type)
{
    CHECK_NULL_API_HANDLE
    return _ctx.graphics.renderer.api.material_create(type);
}
nu_material_t
nu_material_create_color (nu_material_type_t type, nu_color_t color)
{
    nu_material_t mat = nu_material_create(type);
    nu_material_set_color(mat, color);
    return mat;
}
void
nu_material_delete (nu_material_t material)
{
    CHECK_NULL_API_VOID
    _ctx.graphics.renderer.api.material_delete(material);
}
void
nu_material_set_color (nu_material_t material, nu_color_t color)
{
    CHECK_NULL_API_VOID
    _ctx.graphics.renderer.api.material_set_color(material, color);
}
void
nu_material_set_texture (nu_material_t material, nu_texture_t texture)
{
    CHECK_NULL_API_VOID
    _ctx.graphics.renderer.api.material_set_texture(material, texture);
}
void
nu_material_set_uv_transform (nu_material_t material, nu_mat3_t mat)
{
    CHECK_NULL_API_VOID
    _ctx.graphics.renderer.api.material_set_uv_transform(material, mat);
}
void
nu_material_set_wrap_mode (nu_material_t material, nu_texture_wrap_mode_t mode)
{
    CHECK_NULL_API_VOID
    _ctx.graphics.renderer.api.material_set_wrap_mode(material, mode);
}

nu_renderpass_t
nu_renderpass_create (nu_renderpass_type_t type)
{
    CHECK_NULL_API_HANDLE
    return _ctx.graphics.renderer.api.renderpass_create(type);
}
void
nu_renderpass_delete (nu_renderpass_t pass)
{
    CHECK_NULL_API_VOID
    _ctx.graphics.renderer.api.renderpass_delete(pass);
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
#ifdef NU_BUILD_UTILS
    _ctx.utils.stats.renderer_current.renderpass_count += 1;
#endif
}

void
nu_renderpass_set_reset_after_submit (nu_renderpass_t pass, nu_bool_t bool)
{
    CHECK_NULL_API_VOID
    _ctx.graphics.renderer.api.renderpass_set_reset_after_submit(pass, bool);
}
void
nu_renderpass_set_clear_color (nu_renderpass_t pass, nu_color_t *color)
{
    CHECK_NULL_API_VOID
    _ctx.graphics.renderer.api.renderpass_set_clear_color(pass, color);
}
void
nu_renderpass_set_camera (nu_renderpass_t pass, nu_camera_t camera)
{
    CHECK_NULL_API_VOID
    _ctx.graphics.renderer.api.renderpass_set_camera(pass, camera);
}
void
nu_renderpass_set_skybox (nu_renderpass_t pass,
                          nu_cubemap_t    cubemap,
                          nu_quat_t       rotation)
{
    CHECK_NULL_API_VOID
    _ctx.graphics.renderer.api.renderpass_set_skybox(pass, cubemap, rotation);
}
void
nu_renderpass_set_color_target (nu_renderpass_t pass, nu_texture_t texture)
{
    CHECK_NULL_API_VOID
    _ctx.graphics.renderer.api.renderpass_set_color_target(pass, texture);
}
void
nu_renderpass_set_depth_target (nu_renderpass_t pass, nu_texture_t texture)
{
    CHECK_NULL_API_VOID
    _ctx.graphics.renderer.api.renderpass_set_depth_target(pass, texture);
}
void
nu_renderpass_set_shadow_target (nu_renderpass_t pass, nu_texture_t texture)
{
    CHECK_NULL_API_VOID
    _ctx.graphics.renderer.api.renderpass_set_shadow_target(pass, texture);
}

void
nu_bind_material (nu_renderpass_t pass, nu_material_t material)
{
    CHECK_NULL_API_VOID
    _ctx.graphics.renderer.api.bind_material(pass, material);
}
void
nu_draw_submesh_instanced (nu_renderpass_t  pass,
                           nu_mesh_t        mesh,
                           nu_size_t        first,
                           nu_size_t        count,
                           nu_size_t        instance_count,
                           const nu_mat4_t *transforms)
{
    CHECK_NULL_API_VOID
    _ctx.graphics.renderer.api.draw_submesh_instanced(
        pass, mesh, first, count, instance_count, transforms);
#ifdef NU_BUILD_UTILS
    nu_primitive_t primitive = _ctx.graphics.renderer.api.mesh_primitive(mesh);
    switch (primitive)
    {
        case NU_PRIMITIVE_POINTS:
            _ctx.utils.stats.renderer_current.point_count += count;
            break;
        case NU_PRIMITIVE_LINES:
            _ctx.utils.stats.renderer_current.line_count += count;
            break;
        case NU_PRIMITIVE_LINES_STRIP:
            _ctx.utils.stats.renderer_current.line_count += count;
            break;
        case NU_PRIMITIVE_TRIANGLES:
            _ctx.utils.stats.renderer_current.triangle_count += count;
            break;
    }
#endif
}
void
nu_draw_blit (nu_renderpass_t pass, nu_box2i_t extent, nu_box2i_t tex_extent)
{
    CHECK_NULL_API_VOID
    _ctx.graphics.renderer.api.draw_blit(pass, extent, tex_extent);
}

void
nu_draw_blit_sliced (nu_renderpass_t pass,
                     nu_box2i_t      extent,
                     nu_box2i_t      tex_extent,
                     nu_box2i_t      inner)
{
    nu_u32_t margin_left   = inner.min.x - tex_extent.min.x;
    nu_u32_t margin_right  = tex_extent.max.x - inner.max.x;
    nu_u32_t margin_top    = inner.min.y - tex_extent.min.y;
    nu_u32_t margin_bottom = tex_extent.max.y - inner.max.y;

    nu_u32_t tex_mid_width
        = nu_box2i_size(tex_extent).x - margin_left - margin_right;
    nu_u32_t tex_mid_height
        = nu_box2i_size(tex_extent).y - margin_top - margin_bottom;
    nu_u32_t ext_mid_width
        = nu_box2i_size(extent).x - margin_left - margin_right;
    nu_u32_t ext_mid_height
        = nu_box2i_size(extent).y - margin_top - margin_bottom;

    // Top-Left
    if (margin_top && margin_left)
    {
        nu_draw_blit(
            pass,
            nu_box2i_xywh(extent.min.x, extent.min.y, margin_left, margin_top),
            nu_box2i_xywh(
                tex_extent.min.x, tex_extent.min.y, margin_left, margin_top));
    }

    // Top-Mid
    if (margin_top)
    {
        nu_draw_blit(pass,
                     nu_box2i_xywh(extent.min.x + margin_left,
                                   extent.min.y,
                                   ext_mid_width,
                                   margin_top),
                     nu_box2i_xywh(tex_extent.min.x + margin_left,
                                   tex_extent.min.y,
                                   tex_mid_width,
                                   margin_top));
    }

    // Top-Right
    if (margin_top && margin_right)
    {
        nu_draw_blit(pass,
                     nu_box2i_xywh(extent.max.x - margin_right + 1,
                                   extent.min.y,
                                   margin_right,
                                   margin_top),
                     nu_box2i_xywh(tex_extent.max.x - margin_right + 1,
                                   tex_extent.min.y,
                                   margin_right,
                                   margin_top));
    }

    // Mid-Left
    if (margin_left)
    {
        nu_draw_blit(pass,
                     nu_box2i_xywh(extent.min.x,
                                   extent.min.y + margin_top,
                                   margin_right,
                                   ext_mid_height),
                     nu_box2i_xywh(tex_extent.min.x,
                                   tex_extent.min.y + margin_top,
                                   margin_right,
                                   tex_mid_height));
    }

    // Mid-Mid
    nu_draw_blit(pass,
                 nu_box2i_xywh(extent.min.x + margin_left,
                               extent.min.y + margin_top,
                               ext_mid_width,
                               ext_mid_height),
                 nu_box2i_xywh(tex_extent.min.x + margin_left,
                               tex_extent.min.y + margin_top,
                               tex_mid_width,
                               tex_mid_height));

    // Mid-Right
    if (margin_right)
    {
        nu_draw_blit(pass,
                     nu_box2i_xywh(extent.max.x - margin_right + 1,
                                   extent.min.y + margin_top,
                                   margin_right,
                                   ext_mid_height),
                     nu_box2i_xywh(tex_extent.max.x - margin_right + 1,
                                   tex_extent.min.y + margin_top,
                                   margin_right,
                                   tex_mid_height));
    }

    // Bottom-Left
    if (margin_bottom && margin_left)
    {
        nu_draw_blit(pass,
                     nu_box2i_xywh(extent.min.x,
                                   extent.max.y - margin_bottom + 1,
                                   margin_left,
                                   margin_bottom),
                     nu_box2i_xywh(tex_extent.min.x,
                                   tex_extent.max.y - margin_bottom + 1,
                                   margin_right,
                                   margin_bottom));
    }

    // Bottom-Mid
    if (margin_bottom)
    {
        nu_draw_blit(pass,
                     nu_box2i_xywh(extent.min.x + margin_left,
                                   extent.max.y - margin_bottom + 1,
                                   ext_mid_width,
                                   margin_bottom),
                     nu_box2i_xywh(tex_extent.min.x + margin_left,
                                   tex_extent.max.y - margin_bottom + 1,
                                   tex_mid_width,
                                   margin_bottom));
    }

    // Bottom-Right
    if (margin_bottom && margin_right)
    {
        nu_draw_blit(pass,
                     nu_box2i_xywh(extent.max.x - margin_right + 1,
                                   extent.max.y - margin_bottom + 1,
                                   margin_right,
                                   margin_bottom),
                     nu_box2i_xywh(tex_extent.max.x - margin_right + 1,
                                   tex_extent.max.y - margin_bottom + 1,
                                   margin_right,
                                   margin_bottom));
    }
}
void
nu_draw_mesh (nu_renderpass_t pass, nu_mesh_t mesh, nu_mat4_t transform)
{
    nu_draw_mesh_instanced(pass, mesh, 1, &transform);
}
void
nu_draw_submesh (nu_renderpass_t pass,
                 nu_mesh_t       mesh,
                 nu_size_t       first,
                 nu_size_t       count,
                 nu_mat4_t       transform)
{
    nu_draw_submesh_instanced(pass, mesh, first, count, 1, &transform);
}
void
nu_draw_mesh_instanced (nu_renderpass_t  pass,
                        nu_mesh_t        mesh,
                        nu_size_t        instance_count,
                        const nu_mat4_t *transforms)
{
    CHECK_NULL_API_VOID
    nu_size_t capacity = _ctx.graphics.renderer.api.mesh_capacity(mesh);
    nu_draw_submesh_instanced(
        pass, mesh, 0, capacity, instance_count, transforms);
}

#undef CHECK_NULL_API_HANDLE
#undef CHECK_NULL_API_ERROR
#undef CHECK_NULL_API_VOID

#endif

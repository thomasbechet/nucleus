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
                   nu_lookat(NU_VEC3_UP, NU_VEC3_FORWARD, NU_VEC3_ZEROS));
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
nu_mesh_vec2 (nu_mesh_t        mesh,
              nu_mesh_buffer_t buffer,
              nu_size_t        first,
              nu_size_t        count,
              const nu_vec2_t *data)
{
    CHECK_NULL_API_VOID
    _ctx.graphics.renderer.api.mesh_vec2(mesh, buffer, first, count, data);
}
void
nu_mesh_vec3 (nu_mesh_t        mesh,
              nu_mesh_buffer_t buffer,
              nu_size_t        first,
              nu_size_t        count,
              const nu_vec3_t *data)
{
    CHECK_NULL_API_VOID
    _ctx.graphics.renderer.api.mesh_vec3(mesh, buffer, first, count, data);
}
void
nu_mesh_color (nu_mesh_t         mesh,
               nu_mesh_buffer_t  buffer,
               nu_size_t         first,
               nu_size_t         count,
               const nu_color_t *data)
{
}

nu_texture_t
nu_texture_create (nu_vec2u_t          size,
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
    return nu_texture_create(nu_vec2u(1, 1),
                             NU_TEXTURE_FORMAT_COLOR,
                             NU_TEXTURE_USAGE_SAMPLE,
                             &color);
}
nu_texture_t
nu_image_create_texture (nu_image_t image)
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

NU_API nu_cubemap_t
nu_cubemap_create (nu_u32_t           size,
                   nu_texture_usage_t usage,
                   const nu_color_t **colors)
{
    CHECK_NULL_API_HANDLE
    return _ctx.graphics.renderer.api.cubemap_create(size, usage, colors);
}
void
nu_cubemap_delete (nu_cubemap_t cubemap)
{
    CHECK_NULL_API_VOID
    _ctx.graphics.renderer.api.cubemap_delete(cubemap);
}

nu_material_t
nu_material_create (nu_material_type_t type)
{
    CHECK_NULL_API_HANDLE
    return _ctx.graphics.renderer.api.material_create(type);
}
void
nu_material_delete (nu_material_t material)
{
    CHECK_NULL_API_VOID
    _ctx.graphics.renderer.api.material_delete(material);
}
void
nu_material_color (nu_material_t          material,
                   nu_material_property_t prop,
                   nu_color_t             color)
{
    CHECK_NULL_API_VOID
    _ctx.graphics.renderer.api.material_color(material, prop, color);
}
void
nu_material_texture (nu_material_t          material,
                     nu_material_property_t prop,
                     nu_texture_t           texture)
{
    CHECK_NULL_API_VOID
    _ctx.graphics.renderer.api.material_texture(material, prop, texture);
}
void
nu_material_mat3 (nu_material_t          material,
                  nu_material_property_t prop,
                  nu_mat3_t              mat)
{
    CHECK_NULL_API_VOID
    _ctx.graphics.renderer.api.material_mat3(material, prop, mat);
}
void
nu_material_wrap_mode (nu_material_t          material,
                       nu_material_property_t prop,
                       nu_texture_wrap_mode_t mode)
{
    CHECK_NULL_API_VOID
    _ctx.graphics.renderer.api.material_wrap_mode(material, prop, mode);
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
nu_renderpass_color (nu_renderpass_t          pass,
                     nu_renderpass_property_t prop,
                     nu_color_t              *color)
{
    CHECK_NULL_API_VOID
    _ctx.graphics.renderer.api.renderpass_color(pass, prop, color);
}
void
nu_renderpass_camera (nu_renderpass_t          pass,
                      nu_renderpass_property_t prop,
                      nu_camera_t              camera)
{
    CHECK_NULL_API_VOID
    _ctx.graphics.renderer.api.renderpass_camera(pass, prop, camera);
}
void
nu_renderpass_cubemap (nu_renderpass_t          pass,
                       nu_renderpass_property_t prop,
                       nu_cubemap_t             cubemap)
{
    CHECK_NULL_API_VOID
    _ctx.graphics.renderer.api.renderpass_cubemap(pass, prop, cubemap);
}
void
nu_renderpass_quat (nu_renderpass_t          pass,
                    nu_renderpass_property_t prop,
                    nu_quat_t                rot)
{
    CHECK_NULL_API_VOID
    _ctx.graphics.renderer.api.renderpass_quat(pass, prop, rot);
}
void
nu_renderpass_texture (nu_renderpass_t          pass,
                       nu_renderpass_property_t prop,
                       nu_texture_t             texture)
{
    CHECK_NULL_API_VOID
    _ctx.graphics.renderer.api.renderpass_texture(pass, prop, texture);
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
    nu_u32_t margin_left = inner.p.x - tex_extent.p.x;
    nu_u32_t margin_right
        = (tex_extent.p.x + tex_extent.s.x) - (inner.p.x + inner.s.x);
    nu_u32_t margin_top = inner.p.y - tex_extent.p.y;
    nu_u32_t margin_bottom
        = (tex_extent.p.y + tex_extent.s.y) - (inner.p.y + inner.s.y);

    nu_u32_t tex_mid_width  = tex_extent.s.x - margin_left - margin_right;
    nu_u32_t tex_mid_height = tex_extent.s.y - margin_top - margin_bottom;
    nu_u32_t ext_mid_width  = extent.s.x - margin_left - margin_right;
    nu_u32_t ext_mid_height = extent.s.y - margin_top - margin_bottom;

    // Top-Left
    if (margin_top && margin_left)
    {
        nu_draw_blit(
            pass,
            nu_box2i(extent.p.x, extent.p.y, margin_left, margin_top),
            nu_box2i(tex_extent.p.x, tex_extent.p.y, margin_left, margin_top));
    }

    // Top-Mid
    if (margin_top)
    {
        nu_draw_blit(pass,
                     nu_box2i(extent.p.x + margin_left,
                              extent.p.y,
                              ext_mid_width,
                              margin_top),
                     nu_box2i(tex_extent.p.x + margin_left,
                              tex_extent.p.y,
                              tex_mid_width,
                              margin_top));
    }

    // Top-Right
    if (margin_top && margin_right)
    {
        nu_draw_blit(pass,
                     nu_box2i(extent.p.x + extent.s.x - margin_right,
                              extent.p.y,
                              margin_right,
                              margin_top),
                     nu_box2i(tex_extent.p.x + tex_extent.s.x - margin_right,
                              tex_extent.p.y,
                              margin_right,
                              margin_top));
    }

    // Mid-Left
    if (margin_left)
    {
        nu_draw_blit(pass,
                     nu_box2i(extent.p.x,
                              extent.p.y + margin_top,
                              margin_right,
                              ext_mid_height),
                     nu_box2i(tex_extent.p.x,
                              tex_extent.p.y + margin_top,
                              margin_right,
                              tex_mid_height));
    }

    // Mid-Mid
    nu_draw_blit(pass,
                 nu_box2i(extent.p.x + margin_left,
                          extent.p.y + margin_top,
                          ext_mid_width,
                          ext_mid_height),
                 nu_box2i(tex_extent.p.x + margin_left,
                          tex_extent.p.y + margin_top,
                          tex_mid_width,
                          tex_mid_height));

    // Mid-Right
    if (margin_right)
    {
        nu_draw_blit(pass,
                     nu_box2i(extent.p.x + extent.s.x - margin_right,
                              extent.p.y + margin_top,
                              margin_right,
                              ext_mid_height),
                     nu_box2i(tex_extent.p.x + tex_extent.s.x - margin_right,
                              tex_extent.p.y + margin_top,
                              margin_right,
                              tex_mid_height));
    }

    // Bottom-Left
    if (margin_bottom && margin_left)
    {
        nu_draw_blit(pass,
                     nu_box2i(extent.p.x,
                              extent.p.y + extent.s.y - margin_bottom,
                              margin_left,
                              margin_bottom),
                     nu_box2i(tex_extent.p.x,
                              tex_extent.p.y + tex_extent.s.y - margin_bottom,
                              margin_right,
                              margin_bottom));
    }

    // Bottom-Mid
    if (margin_bottom)
    {
        nu_draw_blit(pass,
                     nu_box2i(extent.p.x + margin_left,
                              extent.p.y + extent.s.y - margin_bottom,
                              ext_mid_width,
                              margin_bottom),
                     nu_box2i(tex_extent.p.x + margin_left,
                              tex_extent.p.y + tex_extent.s.y - margin_bottom,
                              tex_mid_width,
                              margin_bottom));
    }

    // Bottom-Right
    if (margin_bottom && margin_right)
    {
        nu_draw_blit(pass,
                     nu_box2i(extent.p.x + extent.s.x - margin_right,
                              extent.p.y + extent.s.y - margin_bottom,
                              margin_right,
                              margin_bottom),
                     nu_box2i(tex_extent.p.x + tex_extent.s.x - margin_right,
                              tex_extent.p.y + tex_extent.s.y - margin_bottom,
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

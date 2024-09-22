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

nu_buffer_t
nu_buffer_create (nu_buffer_type_t type,
                  nu_primitive_t   primitive,
                  nu_size_t        size)
{
    CHECK_NULL_API_HANDLE
    return _ctx.graphics.renderer.api.buffer_create(type, primitive, size);
}
void
nu_buffer_delete (nu_buffer_t buffer)
{
    CHECK_NULL_API_VOID
    _ctx.graphics.renderer.api.buffer_delete(buffer);
}
void
nu_buffer_vec2 (nu_buffer_t      buffer,
                nu_size_t        first,
                nu_size_t        count,
                const nu_vec2_t *p)
{
    CHECK_NULL_API_VOID
    _ctx.graphics.renderer.api.buffer_vec2(buffer, first, count, p);
}
void
nu_buffer_vec3 (nu_buffer_t      buffer,
                nu_size_t        first,
                nu_size_t        count,
                const nu_vec3_t *p)
{
    CHECK_NULL_API_VOID
    _ctx.graphics.renderer.api.buffer_vec3(buffer, first, count, p);
}
void
nu_buffer_mat4 (nu_buffer_t      buffer,
                nu_size_t        first,
                nu_size_t        count,
                const nu_mat4_t *p)
{
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
nu_mesh_buffer (nu_mesh_t mesh, nu_buffer_t buffer, nu_size_t first)
{
    CHECK_NULL_API_VOID
    _ctx.graphics.renderer.api.mesh_buffer(mesh, buffer, first);
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
nu_renderpass_color_target (nu_renderpass_t pass, nu_texture_t color)
{
    CHECK_NULL_API_VOID
    _ctx.graphics.renderer.api.renderpass_target_color(pass, color);
}
void
nu_renderpass_depth_target (nu_renderpass_t pass, nu_texture_t depth)
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
nu_bind_material (nu_renderpass_t pass, nu_material_t material)
{
    CHECK_NULL_API_VOID
    _ctx.graphics.renderer.api.bind_material(pass, material);
}
void
nu_draw_meshes (nu_renderpass_t  pass,
                nu_mesh_t        mesh,
                const nu_mat4_t *transforms,
                nu_size_t        count)
{
    CHECK_NULL_API_VOID
    _ctx.graphics.renderer.api.draw_meshes(pass, mesh, transforms, count);
}
void
nu_draw_blit (nu_renderpass_t pass, nu_rect_t extent, nu_rect_t tex_extent)
{
    CHECK_NULL_API_VOID
    _ctx.graphics.renderer.api.draw_blit(pass, extent, tex_extent);
}

void
nu_draw_mesh (nu_renderpass_t pass, nu_mesh_t mesh, nu_mat4_t transform)
{
    nu_draw_meshes(pass, mesh, &transform, 1);
}

#undef CHECK_NULL_API_HANDLE
#undef CHECK_NULL_API_ERROR
#undef CHECK_NULL_API_VOID

#endif

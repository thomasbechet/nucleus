#ifndef NU_BACKEND_IMPL_H
#define NU_BACKEND_IMPL_H

#include <nucleus/internal.h>

#ifdef NU_BUILD_GL
#include <nucleus/gl/gl_impl.h>
#endif

static void
nu__renderer_init (void)
{
#ifdef NU_BUILD_GL
    nugl__init();
#endif
}
static void
nu__renderer_free (void)
{
#ifdef NU_BUILD_GL
    nugl__free();
#endif
}
static nu_texture_t
nu__renderer_create_surface_color (nu_vec2u_t size)
{
#ifdef NU_BUILD_GL
    return nugl__create_surface_color(size);
#endif
}
static void
nu__renderer_render (nu_box2i_t global_viewport, nu_box2i_t viewport)
{
#ifdef NU_BUILD_GL
    nugl__render(global_viewport, viewport);
#endif
}

nu_texture_t
nu_surface_color_target (void)
{
    return _ctx.graphics.surface_color;
}

nu_camera_t
nu_camera_create (void)
{
#ifdef NU_BUILD_GL
    nu_camera_t handle = nugl__camera_create();
    NU_CHECK(handle, return handle);
    nu_camera_set_view(handle,
                       nu_lookat(NU_VEC3_UP, NU_VEC3_FORWARD, NU_VEC3_ZEROS));
    float aspect = (float)_ctx.platform.size.x / (float)_ctx.platform.size.y;
    nu_camera_set_proj(handle,
                       nu_perspective(nu_radian(70), aspect, 0.01, 100));
    return handle;
#endif
}
void
nu_camera_delete (nu_camera_t camera)
{
#ifdef NU_BUILD_GL
    nugl__camera_delete(camera);
#endif
}
void
nu_camera_set_view (nu_camera_t camera, nu_mat4_t view)
{
#ifdef NU_BUILD_GL
    nugl__camera_set_view(camera, view);
#endif
}
void
nu_camera_set_proj (nu_camera_t camera, nu_mat4_t proj)
{
#ifdef NU_BUILD_GL
    nugl__camera_set_proj(camera, proj);
#endif
}

nu_mesh_t
nu_mesh_create (nu_primitive_t primitive, nu_size_t count)
{
#ifdef NU_BUILD_GL
    return nugl__mesh_create(primitive, count);
#endif
}
void
nu_mesh_delete (nu_mesh_t mesh)
{
#ifdef NU_BUILD_GL
    nugl__mesh_delete(mesh);
#endif
}
void
nu_mesh_write_uvs (nu_mesh_t        mesh,
                   nu_size_t        first,
                   nu_size_t        count,
                   const nu_vec2_t *data)
{
#ifdef NU_BUILD_GL
    nugl__mesh_write_uvs(mesh, first, count, data);
#endif
}
void
nu_mesh_write_positions (nu_mesh_t        mesh,
                         nu_size_t        first,
                         nu_size_t        count,
                         const nu_vec3_t *data)
{
#ifdef NU_BUILD_GL
    nugl__mesh_write_positions(mesh, first, count, data);
#endif
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
#ifdef NU_BUILD_GL
    return nugl__texture_create(size, usage);
#endif
}
void
nu_texture_delete (nu_texture_t texture)
{
#ifdef NU_BUILD_GL
    nugl__texture_delete(texture);
#endif
}
void
nu_texture_write_colors (nu_texture_t texture, const nu_color_t *colors)
{
#ifdef NU_BUILD_GL
    nugl__texture_write_colors(texture, colors);
#endif
}

NU_API nu_cubemap_t
nu_cubemap_create (nu_u32_t size)
{
#ifdef NU_BUILD_GL
    return nugl__cubemap_create(size);
#endif
}
void
nu_cubemap_delete (nu_cubemap_t cubemap)
{
#ifdef NU_BUILD_GL
    nugl__cubemap_delete(cubemap);
#endif
}
void
nu_cubemap_write_colors (nu_cubemap_t      cubemap,
                         nu_cubemap_face_t face,
                         const nu_color_t *colors)
{
#ifdef NU_BUILD_GL
    nugl__cubemap_write_colors(cubemap, face, colors);
#endif
}

nu_material_t
nu_material_create (nu_material_type_t type)
{
#ifdef NU_BUILD_GL
    return nugl__material_create(type);
#endif
}
void
nu_material_delete (nu_material_t material)
{
#ifdef NU_BUILD_GL
    nugl__material_delete(material);
#endif
}
void
nu_material_set_color (nu_material_t material, nu_color_t color)
{
#ifdef NU_BUILD_GL
    nugl__material_set_color(material, color);
}
#endif
void
nu_material_set_texture (nu_material_t material, nu_texture_t texture)
{
#ifdef NU_BUILD_GL
    nugl__material_set_texture(material, texture);
#endif
}
void
nu_material_set_uv_transform (nu_material_t material, nu_mat3_t mat)
{
#ifdef NU_BUILD_GL
    nugl__material_set_uv_transform(material, mat);
#endif
}
void
nu_material_set_wrap_mode (nu_material_t material, nu_texture_wrap_mode_t mode)
{
#ifdef NU_BUILD_GL
    nugl__material_set_wrap_mode(material, mode);
#endif
}

nu_light_t
nu_light_create (nu_light_type_t type)
{
#ifdef NU_BUILD_GL
    return nugl__light_create(type);
#endif
}
void
nu_light_delete (nu_light_t light)
{
#ifdef NU_BUILD_GL
    nugl__light_delete(light);
#endif
}
void
nu_light_set_position (nu_light_t light, nu_vec3_t v)
{
#ifdef NU_BUILD_GL
    nugl__light_set_position(light, v);
#endif
}
void
nu_light_set_rotation (nu_light_t light, nu_quat_t q)
{
#ifdef NU_BUILD_GL
    nugl__light_set_rotation(light, q);
#endif
}
void
nu_light_set_color (nu_light_t light, nu_color_t v)
{
}

nu_renderpass_t
nu_renderpass_create (nu_renderpass_type_t type)
{
#ifdef NU_BUILD_GL
    return nugl__renderpass_create(type);
#endif
}
void
nu_renderpass_delete (nu_renderpass_t pass)
{
#ifdef NU_BUILD_GL
    nugl__renderpass_delete(pass);
#endif
}
void
nu_renderpass_reset (nu_renderpass_t pass)
{
#ifdef NU_BUILD_GL
    nugl__renderpass_reset(pass);
#endif
}
void
nu_renderpass_submit (nu_renderpass_t pass)
{
#ifdef NU_BUILD_GL
    nugl__renderpass_submit(pass);
#endif
#ifdef NU_BUILD_UTILS
    _ctx.utils.stats.graphics_current.renderpass_count += 1;
#endif
}

void
nu_renderpass_set_reset_after_submit (nu_renderpass_t pass, nu_bool_t bool)
{
#ifdef NU_BUILD_GL
    nugl__renderpass_set_reset_after_submit(pass, bool);
#endif
}
void
nu_renderpass_set_clear_color (nu_renderpass_t pass, nu_color_t *color)
{
#ifdef NU_BUILD_GL
    nugl__renderpass_set_clear_color(pass, color);
#endif
}
void
nu_renderpass_set_camera (nu_renderpass_t pass, nu_camera_t camera)
{
#ifdef NU_BUILD_GL
    nugl__renderpass_set_camera(pass, camera);
#endif
}
void
nu_renderpass_set_skybox (nu_renderpass_t pass,
                          nu_cubemap_t    cubemap,
                          nu_quat_t       rotation)
{
#ifdef NU_BUILD_GL
    nugl__renderpass_set_skybox(pass, cubemap, rotation);
#endif
}
void
nu_renderpass_set_color_target (nu_renderpass_t pass, nu_texture_t texture)
{
#ifdef NU_BUILD_GL
    nugl__renderpass_set_color_target(pass, texture);
#endif
}
void
nu_renderpass_set_depth_target (nu_renderpass_t pass, nu_texture_t texture)
{
#ifdef NU_BUILD_GL
    nugl__renderpass_set_depth_target(pass, texture);
#endif
}
void
nu_renderpass_set_lightenv (nu_renderpass_t pass, nu_lightenv_t env)
{
}

void
nu_draw_submesh_instanced (nu_renderpass_t  pass,
                           nu_mesh_t        mesh,
                           nu_size_t        first,
                           nu_size_t        count,
                           nu_material_t    material,
                           const nu_mat4_t *transforms,
                           nu_size_t        instance_count)
{
#ifdef NU_BUILD_GL
    nugl__draw_submesh_instanced(
        pass, mesh, first, count, material, transforms, instance_count);
#endif
#ifdef NU_BUILD_UTILS
    nu_primitive_t primitive = nugl__mesh_primitive(mesh);
    switch (primitive)
    {
        case NU_PRIMITIVE_POINTS:
            _ctx.utils.stats.graphics_current.point_count += count;
            break;
        case NU_PRIMITIVE_LINES:
            _ctx.utils.stats.graphics_current.line_count += count;
            break;
        case NU_PRIMITIVE_LINES_STRIP:
            _ctx.utils.stats.graphics_current.line_count += count;
            break;
        case NU_PRIMITIVE_TRIANGLES:
            _ctx.utils.stats.graphics_current.triangle_count += count;
            break;
    }
#endif
}
void
nu_draw_blit (nu_renderpass_t pass,
              nu_box2i_t      extent,
              nu_box2i_t      tex_extent,
              nu_material_t   material)
{
#ifdef NU_BUILD_GL
    nugl__draw_blit(pass, extent, tex_extent, material);
#endif
}

#endif

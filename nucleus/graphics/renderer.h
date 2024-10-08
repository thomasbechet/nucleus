#ifndef NU_RENDERER_H
#define NU_RENDERER_H

#include <nucleus/graphics/api.h>

typedef struct
{
    nu_size_t triangle_count;
    nu_size_t line_count;
    nu_size_t point_count;
    nu_size_t renderpass_count;
} nu__renderer_stats_t;

typedef struct
{
    nu_error_t (*init)(void);
    nu_error_t (*free)(void);
    nu_error_t (*render)(const nu_box2i_t *global_viewport,
                         const nu_box2i_t *viewport);
    nu_texture_t (*create_surface_color)(nu_vec2u_t size);

    nu_camera_t (*camera_create)(void);
    void (*camera_delete)(nu_camera_t camera);
    void (*camera_set_view)(nu_camera_t camera, nu_mat4_t view);
    void (*camera_set_proj)(nu_camera_t camera, nu_mat4_t proj);

    nu_mesh_t (*mesh_create)(nu_primitive_t primitive, nu_size_t capacity);
    void (*mesh_delete)(nu_mesh_t mesh);
    nu_size_t (*mesh_capacity)(nu_mesh_t mesh);
    nu_primitive_t (*mesh_primitive)(nu_mesh_t mesh);
    void (*mesh_write_uvs)(nu_mesh_t        mesh,
                           nu_size_t        first,
                           nu_size_t        count,
                           const nu_vec2_t *data);
    void (*mesh_write_positions)(nu_mesh_t        mesh,
                                 nu_size_t        first,
                                 nu_size_t        count,
                                 const nu_vec3_t *data);
    void (*mesh_write_colors)(nu_mesh_t         mesh,
                              nu_size_t         first,
                              nu_size_t         count,
                              const nu_color_t *data);

    nu_texture_t (*texture_create)(nu_vec2u_t size, nu_texture_type_t usage);
    void (*texture_delete)(nu_texture_t texture);
    void (*texture_write_colors)(nu_texture_t      texture,
                                 const nu_color_t *colors);

    nu_cubemap_t (*cubemap_create)(nu_u32_t size);
    void (*cubemap_delete)(nu_cubemap_t cubemap);
    void (*cubemap_write_colors)(nu_cubemap_t      cubemap,
                                 nu_cubemap_face_t face,
                                 const nu_color_t *colors);

    nu_material_t (*material_create)(nu_material_type_t type);
    void (*material_delete)(nu_material_t material);
    void (*material_set_color)(nu_material_t material, nu_color_t color);
    void (*material_set_texture)(nu_material_t material, nu_texture_t texture);
    void (*material_set_uv_transform)(nu_material_t material, nu_mat3_t mat);
    void (*material_set_wrap_mode)(nu_material_t          material,
                                   nu_texture_wrap_mode_t mode);

    nu_light_t (*light_create)(nu_light_type_t type);
    void (*light_delete)(nu_light_t light);
    void (*light_set_position)(nu_light_t light, nu_vec3_t v);
    void (*light_set_rotation)(nu_light_t light, nu_quat_t q);
    void (*light_set_color)(nu_light_t light, nu_color_t v);

    nu_renderpass_t (*renderpass_create)(nu_renderpass_type_t type);
    void (*renderpass_delete)(nu_renderpass_t pass);
    void (*renderpass_reset)(nu_renderpass_t pass);
    void (*renderpass_submit)(nu_renderpass_t pass);

    void (*renderpass_set_reset_after_submit)(nu_renderpass_t pass,
                                              nu_bool_t bool);
    void (*renderpass_set_clear_color)(nu_renderpass_t pass, nu_color_t *color);
    void (*renderpass_set_camera)(nu_renderpass_t pass, nu_camera_t camera);
    void (*renderpass_set_skybox)(nu_renderpass_t pass,
                                  nu_cubemap_t    cubemap,
                                  nu_quat_t       rotation);
    void (*renderpass_set_color_target)(nu_renderpass_t pass,
                                        nu_texture_t    texture);
    void (*renderpass_set_depth_target)(nu_renderpass_t pass,
                                        nu_texture_t    texture);
    void (*renderpass_set_lightenv)(nu_renderpass_t pass, nu_lightenv_t env);

    void (*bind_material)(nu_renderpass_t pass, nu_material_t material);
    void (*draw_submesh_instanced)(nu_renderpass_t  pass,
                                   nu_mesh_t        meshes,
                                   nu_size_t        first,
                                   nu_size_t        count,
                                   nu_size_t        instance_count,
                                   const nu_mat4_t *transforms);
    void (*draw_blit)(nu_renderpass_t pass,
                      nu_box2i_t      extent,
                      nu_box2i_t      tex_extent);
} nu__renderer_api_t;

typedef struct
{
    nu__renderer_api_t api;
    nu_bool_t          null_api;
    nu_texture_t       surface_color;
} nu__renderer_t;

static nu_size_t
nu__primitive_vertex_count (nu_primitive_t primitive, nu_size_t capacity)
{
    switch (primitive)
    {
        case NU_PRIMITIVE_POINTS:
            return capacity;
        case NU_PRIMITIVE_LINES:
            return capacity * 2;
        case NU_PRIMITIVE_LINES_STRIP:
            return capacity + 1;
        case NU_PRIMITIVE_TRIANGLES:
            return capacity * 3;
    }
    return 0;
}
static nu_size_t
nu__primitive_vertex_first (nu_primitive_t primitive, nu_size_t first)
{
    switch (primitive)
    {
        case NU_PRIMITIVE_POINTS:
            return first;
        case NU_PRIMITIVE_LINES:
            return first * 2;
        case NU_PRIMITIVE_LINES_STRIP:
            return first;
        case NU_PRIMITIVE_TRIANGLES:
            return first * 3;
    }
    return 0;
}

#endif

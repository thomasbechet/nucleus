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
    void (*camera_view)(nu_camera_t camera, nu_mat4_t view);
    void (*camera_proj)(nu_camera_t camera, nu_mat4_t proj);

    nu_mesh_t (*mesh_create)(nu_primitive_t primitive, nu_size_t capacity);
    void (*mesh_delete)(nu_mesh_t mesh);
    nu_size_t (*mesh_capacity)(nu_mesh_t mesh);
    nu_primitive_t (*mesh_primitive)(nu_mesh_t mesh);
    void (*mesh_vec2)(nu_mesh_t        mesh,
                      nu_mesh_buffer_t buffer,
                      nu_size_t        first,
                      nu_size_t        count,
                      const nu_vec2_t *data);
    void (*mesh_vec3)(nu_mesh_t        mesh,
                      nu_mesh_buffer_t buffer,
                      nu_size_t        first,
                      nu_size_t        count,
                      const nu_vec3_t *data);

    nu_texture_t (*texture_create)(nu_vec2u_t          size,
                                   nu_texture_format_t format,
                                   nu_texture_usage_t  usage,
                                   const nu_color_t   *colors);
    void (*texture_delete)(nu_texture_t texture);

    nu_cubemap_t (*cubemap_create)(nu_u32_t           size,
                                   nu_texture_usage_t usage,
                                   const nu_color_t **colors);
    void (*cubemap_delete)(nu_cubemap_t cubemap);

    nu_material_t (*material_create)(nu_material_type_t type);
    void (*material_delete)(nu_material_t material);
    void (*material_color)(nu_material_t          material,
                           nu_material_property_t prop,
                           nu_color_t             color);
    void (*material_texture)(nu_material_t          material,
                             nu_material_property_t prop,
                             nu_texture_t           texture);
    void (*material_mat3)(nu_material_t          material,
                          nu_material_property_t prop,
                          nu_mat3_t              mat);
    void (*material_wrap_mode)(nu_material_t          material,
                               nu_material_property_t prop,
                               nu_texture_wrap_mode_t mode);

    nu_renderpass_t (*renderpass_create)(nu_renderpass_type_t type,
                                         nu_bool_t reset_after_submit);
    void (*renderpass_delete)(nu_renderpass_t pass);
    void (*renderpass_reset)(nu_renderpass_t pass);
    void (*renderpass_submit)(nu_renderpass_t pass);

    void (*renderpass_color)(nu_renderpass_t          pass,
                             nu_renderpass_property_t prop,
                             nu_color_t              *color);
    void (*renderpass_camera)(nu_renderpass_t          pass,
                              nu_renderpass_property_t prop,
                              nu_camera_t              camera);
    void (*renderpass_cubemap)(nu_renderpass_t          pass,
                               nu_renderpass_property_t prop,
                               nu_cubemap_t             cubemap);
    void (*renderpass_quat)(nu_renderpass_t          pass,
                            nu_renderpass_property_t prop,
                            nu_quat_t                rot);
    void (*renderpass_texture)(nu_renderpass_t          pass,
                               nu_renderpass_property_t prop,
                               nu_texture_t             texture);

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

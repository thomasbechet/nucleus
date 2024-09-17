#ifndef NU_RENDERER_H
#define NU_RENDERER_H

#include <nucleus/graphics/api.h>

typedef struct
{
    nu_error_t (*init)(void);
    nu_error_t (*free)(void);
    nu_error_t (*render)(const nu_rect_t *global_viewport,
                         const nu_rect_t *viewport);
    nu_texture_t (*create_surface_color)(nu_uvec2_t size);

    nu_camera_t (*camera_create)(void);
    void (*camera_delete)(nu_camera_t camera);
    void (*camera_view)(nu_camera_t camera, nu_mat4_t view);
    void (*camera_proj)(nu_camera_t camera, nu_mat4_t proj);

    nu_mesh_t (*mesh_create)(nu_size_t count);
    void (*mesh_delete)(nu_mesh_t mesh);
    void (*mesh_update)(nu_mesh_t        mesh,
                        const nu_vec3_t *positions,
                        const nu_vec2_t *uvs,
                        const nu_vec3_t *normals);

    nu_texture_t (*texture_create)(nu_uvec2_t          size,
                                   nu_texture_format_t format,
                                   nu_texture_usage_t  usage,
                                   const nu_color_t   *colors);
    void (*texture_delete)(nu_texture_t texture);

    nu_cubemap_t (*cubemap_create)(const nu_cubemap_info_t *info);
    void (*cubemap_delete)(nu_cubemap_t cubemap);

    nu_material_t (*material_create)(nu_material_type_t type);
    void (*material_delete)(nu_material_t material);
    void (*material_color0)(nu_material_t material, nu_texture_t color0);
    void (*material_color1)(nu_material_t material, nu_texture_t color1);
    void (*material_uv_transform)(nu_material_t material, nu_mat3_t transform);
    void (*material_wrap_mode)(nu_material_t          material,
                               nu_texture_wrap_mode_t mode);

    nu_renderpass_t (*renderpass_create)(nu_renderpass_type_t type,
                                         nu_bool_t reset_after_submit);
    void (*renderpass_delete)(nu_renderpass_t pass);
    void (*renderpass_clear_color)(nu_renderpass_t pass, nu_color_t *color);
    void (*renderpass_camera)(nu_renderpass_t pass, nu_camera_t camera);
    void (*renderpass_skybox_cubemap)(nu_renderpass_t pass,
                                      nu_cubemap_t    cubemap);
    void (*renderpass_skybox_rotation)(nu_renderpass_t pass, nu_quat_t rot);
    void (*renderpass_target_color)(nu_renderpass_t pass, nu_texture_t color);
    void (*renderpass_target_depth)(nu_renderpass_t pass, nu_texture_t depth);
    void (*renderpass_reset)(nu_renderpass_t pass);
    void (*renderpass_submit)(nu_renderpass_t pass);

    void (*draw_mesh)(nu_renderpass_t pass,
                      nu_material_t   material,
                      nu_mesh_t       mesh,
                      nu_mat4_t       transform);
    void (*draw_blit)(nu_renderpass_t pass,
                      nu_material_t   material,
                      nu_rect_t       extent,
                      nu_rect_t       tex_extent);
} nu__renderer_api_t;

typedef struct
{
    nu__renderer_api_t api;
    nu_bool_t          null_api;
    nu_texture_t       surface_color;
} nu__renderer_t;

#endif

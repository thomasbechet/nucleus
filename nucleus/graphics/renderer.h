#ifndef NU_RENDERER_H
#define NU_RENDERER_H

#include <nucleus/graphics/api.h>

typedef struct
{
    // Engine API
    nu_error_t (*init)(void);
    nu_error_t (*free)(void);
    nu_error_t (*render)(const nu_rect_t *global_viewport,
                         const nu_rect_t *viewport);
    nu_texture_t (*create_surface_color)(nu_uvec2_t size);

    // Resources API
    nu_camera_t (*create_camera)(void);
    void (*delete_camera)(nu_camera_t camera);
    void (*update_camera_view)(nu_camera_t camera, nu_mat4_t view);
    void (*update_camera_proj)(nu_camera_t camera, nu_mat4_t proj);

    nu_mesh_t (*create_mesh)(nu_size_t count);
    void (*delete_mesh)(nu_mesh_t mesh);
    void (*update_mesh)(nu_mesh_t        mesh,
                        const nu_vec3_t *positions,
                        const nu_vec2_t *uvs,
                        const nu_vec3_t *normals);

    nu_texture_t (*create_texture)(nu_uvec2_t          size,
                                   nu_texture_format_t format,
                                   nu_texture_usage_t  usage,
                                   const nu_color_t   *colors);
    nu_error_t (*delete_texture)(nu_texture_t texture);

    nu_cubemap_t (*create_cubemap)(const nu_cubemap_info_t *info);
    nu_error_t (*delete_cubemap)(nu_cubemap_t cubemap);

    nu_material_t (*create_material)(const nu_material_info_t *info);
    nu_error_t (*delete_material)(nu_material_t material);
    nu_error_t (*update_material)(nu_material_t             material,
                                  const nu_material_info_t *info);

    nu_renderpass_t (*create_renderpass)(const nu_renderpass_info_t *info);
    void (*delete_renderpass)(nu_renderpass_t pass);

    // Commands API
    void (*submit_renderpass)(nu_renderpass_t               pass,
                              const nu_renderpass_submit_t *info);
    void (*reset_renderpass)(nu_renderpass_t pass);
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

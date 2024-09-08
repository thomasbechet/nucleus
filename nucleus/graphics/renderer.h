#ifndef NU_RENDERER_H
#define NU_RENDERER_H

#include <nucleus/graphics/texture.h>
#include <nucleus/graphics/camera.h>
#include <nucleus/graphics/mesh.h>
#include <nucleus/graphics/cubemap.h>
#include <nucleus/graphics/renderpass.h>
#include <nucleus/graphics/material.h>
#include <nucleus/graphics/image.h>

typedef enum
{
    NU_RENDERER_NULL,
    NU_RENDERER_GL,
    NU_RENDERER_DX11,
    NU_RENDERER_SOFTRAST,
} nu_renderer_backend_t;

typedef struct
{
    nu_renderer_backend_t api;
} nu_renderer_info_t;

NU_API nu_texture_t nu_surface_color_target(void);

NU_API nu_camera_info_t nu_camera_info(void);
NU_API nu_camera_t      nu_camera_create(const nu_camera_info_t *info);
NU_API nu_error_t       nu_camera_delete(nu_camera_t camera);
NU_API nu_error_t       nu_camera_update(nu_camera_t             camera,
                                         const nu_camera_info_t *info);

NU_API nu_mesh_t  nu_mesh_create(const nu_mesh_info_t *info);
NU_API nu_error_t nu_mesh_delete(nu_mesh_t mesh);

NU_API nu_texture_t nu_texture_create(const nu_texture_info_t *info);
NU_API nu_texture_t nu_texture_create_color(nu_color_t color);
NU_API nu_texture_t nu_texture_create_image(nu_image_t image);
NU_API nu_error_t   nu_texture_delete(nu_texture_t texture);

NU_API nu_cubemap_t nu_cubemap_create(const nu_cubemap_info_t *info);
NU_API nu_error_t   nu_cubemap_delete(nu_cubemap_t cubemap);

NU_API nu_material_info_t nu_material_info_default(nu_material_type_t type);
NU_API nu_material_t      nu_material_create(const nu_material_info_t *info);
NU_API nu_error_t         nu_material_delete(nu_material_t material);
NU_API nu_error_t         nu_material_update(nu_material_t             material,
                                             const nu_material_info_t *info);

NU_API nu_renderpass_t nu_renderpass_create(const nu_renderpass_info_t *info);
NU_API nu_error_t      nu_renderpass_delete(nu_renderpass_t pass);

NU_API void nu_renderpass_submit(nu_renderpass_t               pass,
                                 const nu_renderpass_submit_t *info);
NU_API void nu_renderpass_reset(nu_renderpass_t pass);
NU_API void nu_draw_mesh(nu_renderpass_t pass,
                         nu_material_t   material,
                         nu_mesh_t       mesh,
                         nu_mat4_t       transform);
NU_API void nu_draw_blit(nu_renderpass_t pass,
                         nu_material_t   material,
                         nu_rect_t       extent,
                         nu_rect_t       tex_extent);

#endif

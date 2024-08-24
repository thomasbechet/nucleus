#ifndef NU_RENDERER_H
#define NU_RENDERER_H

#include <nucleus/platform.h>
#include <nucleus/graphics/texture.h>
#include <nucleus/graphics/camera.h>
#include <nucleus/graphics/mesh.h>
#include <nucleus/graphics/cubemap.h>
#include <nucleus/graphics/renderpass.h>
#include <nucleus/graphics/material.h>

typedef enum
{
    NU_RENDERER_NULL,
    NU_RENDERER_GL,
    NU_RENDERER_DX11,
    NU_RENDERER_SOFTRAST,
} nu_renderer_backend_t;

struct nu_renderer;

typedef struct
{
    // Engine API
    nu_error_t (*init)(struct nu_renderer *ctx, nu_uvec2_t size);
    nu_error_t (*free)(struct nu_renderer *ctx);
    nu_error_t (*render)(struct nu_renderer *ctx,
                         const nu_rect_t    *global_viewport,
                         const nu_rect_t    *viewport);
    nu_texture_handle_t (*create_surface_color)(struct nu_renderer *ctx,
                                                nu_uvec2_t          size);

    // Resources API
    nu_error_t (*create_camera)(struct nu_renderer     *ctx,
                                const nu_camera_info_t *info,
                                nu_camera_handle_t     *camera);
    nu_error_t (*delete_camera)(struct nu_renderer *ctx,
                                nu_camera_handle_t  camera);
    nu_error_t (*update_camera)(struct nu_renderer     *ctx,
                                nu_camera_handle_t      camera,
                                const nu_camera_info_t *info);

    nu_error_t (*create_mesh)(struct nu_renderer   *ctx,
                              const nu_mesh_info_t *info,
                              nu_mesh_handle_t     *mesh);
    nu_error_t (*delete_mesh)(struct nu_renderer *ctx, nu_mesh_handle_t mesh);

    nu_error_t (*create_texture)(struct nu_renderer      *ctx,
                                 const nu_texture_info_t *info,
                                 nu_texture_handle_t     *texture);
    nu_error_t (*delete_texture)(struct nu_renderer *ctx,
                                 nu_texture_handle_t texture);

    nu_error_t (*create_cubemap)(struct nu_renderer      *ctx,
                                 const nu_cubemap_info_t *info,
                                 nu_cubemap_handle_t     *cubemap);
    nu_error_t (*delete_cubemap)(struct nu_renderer *ctx,
                                 nu_cubemap_handle_t cubemap);

    nu_error_t (*create_material)(struct nu_renderer       *ctx,
                                  const nu_material_info_t *info,
                                  nu_material_handle_t     *material);
    nu_error_t (*delete_material)(struct nu_renderer  *ctx,
                                  nu_material_handle_t material);
    nu_error_t (*update_material)(struct nu_renderer       *ctx,
                                  nu_material_handle_t      material,
                                  const nu_material_info_t *info);

    nu_error_t (*create_renderpass)(struct nu_renderer         *ctx,
                                    const nu_renderpass_info_t *info,
                                    nu_renderpass_handle_t     *pass);
    nu_error_t (*delete_renderpass)(struct nu_renderer    *ctx,
                                    nu_renderpass_handle_t pass);

    // Commands API
    void (*submit_renderpass)(struct nu_renderer           *ctx,
                              nu_renderpass_handle_t        pass,
                              const nu_renderpass_submit_t *info);
    void (*reset_renderpass)(struct nu_renderer    *ctx,
                             nu_renderpass_handle_t pass);
    void (*draw)(struct nu_renderer    *ctx,
                 nu_renderpass_handle_t pass,
                 nu_material_handle_t   material,
                 nu_mesh_handle_t       mesh,
                 nu_mat4_t              transform);
    void (*blit)(struct nu_renderer    *ctx,
                 nu_renderpass_handle_t pass,
                 nu_material_handle_t   material,
                 nu_rect_t              extent,
                 nu_rect_t              tex_extent);
} nu_renderer_api_t;

typedef struct
{
    nu_renderer_backend_t api;
    nu_logger_info_t      logger;
    nu_allocator_t        allocator;
} nu_renderer_info_t;

typedef struct nu_renderer
{
    nu_logger_t         _logger;
    nu_allocator_t      _allocator;
    nu_renderer_api_t   _api;
    void               *_backend;
    nu_texture_handle_t _surface_color;
} nu_renderer_t;

NU_API nu_renderer_info_t nu_renderer_info_default(void);

NU_API nu_error_t nu_renderer_init(nu_platform_t            *platform,
                                   const nu_renderer_info_t *info,
                                   nu_renderer_t            *renderer);
NU_API nu_error_t nu_renderer_terminate(nu_renderer_t *renderer);
NU_API nu_error_t nu_render(nu_platform_t *platform, nu_renderer_t *renderer);
NU_API nu_texture_handle_t nu_surface_color_target(
    const nu_platform_t *platform, const nu_renderer_t *renderer);

NU_API nu_camera_info_t nu_camera_info_default(void);
NU_API nu_error_t       nu_camera_create(nu_renderer_t          *ctx,
                                         const nu_camera_info_t *info,
                                         nu_camera_handle_t     *camera);
NU_API nu_error_t       nu_camera_delete(nu_renderer_t     *ctx,
                                         nu_camera_handle_t camera);
NU_API nu_error_t       nu_camera_update(nu_renderer_t          *ctx,
                                         nu_camera_handle_t      camera,
                                         const nu_camera_info_t *info);

NU_API nu_error_t nu_mesh_create(nu_renderer_t        *ctx,
                                 const nu_mesh_info_t *info,
                                 nu_mesh_handle_t     *mesh);
NU_API nu_error_t nu_mesh_delete(nu_renderer_t *ctx, nu_mesh_handle_t mesh);

NU_API nu_error_t nu_texture_create(nu_renderer_t           *ctx,
                                    const nu_texture_info_t *info,
                                    nu_texture_handle_t     *texture);
NU_API nu_error_t nu_texture_create_color(nu_renderer_t       *ctx,
                                          nu_color_t           color,
                                          nu_texture_handle_t *texture);
NU_API nu_error_t nu_texture_delete(nu_renderer_t      *ctx,
                                    nu_texture_handle_t texture);

NU_API nu_cubemap_info_t nu_cubemap_info_default(void);
NU_API nu_error_t        nu_cubemap_create(nu_renderer_t           *ctx,
                                           const nu_cubemap_info_t *info,
                                           nu_cubemap_handle_t     *cubemap);
NU_API nu_error_t        nu_cubemap_delete(nu_renderer_t      *ctx,
                                           nu_cubemap_handle_t cubemap);

NU_API nu_material_info_t nu_material_info_default(nu_material_type_t type);
NU_API nu_error_t         nu_material_create(nu_renderer_t            *ctx,
                                             const nu_material_info_t *info,
                                             nu_material_handle_t     *material);
NU_API nu_error_t         nu_material_delete(nu_renderer_t       *ctx,
                                             nu_material_handle_t material);
NU_API nu_error_t         nu_material_update(nu_renderer_t            *ctx,
                                             nu_material_handle_t      material,
                                             const nu_material_info_t *info);

NU_API nu_error_t nu_renderpass_create(nu_renderer_t              *ctx,
                                       const nu_renderpass_info_t *info,
                                       nu_renderpass_handle_t     *pass);
NU_API nu_error_t nu_renderpass_delete(nu_renderer_t         *ctx,
                                       nu_renderpass_handle_t pass);

NU_API void nu_renderpass_submit(nu_renderer_t                *ctx,
                                 nu_renderpass_handle_t        pass,
                                 const nu_renderpass_submit_t *info);
NU_API void nu_renderpass_reset(nu_renderer_t         *ctx,
                                nu_renderpass_handle_t pass);
NU_API void nu_draw(nu_renderer_t         *ctx,
                    nu_renderpass_handle_t pass,
                    nu_material_handle_t   material,
                    nu_mesh_handle_t       mesh,
                    nu_mat4_t              transform);
NU_API void nu_blit(nu_renderer_t         *ctx,
                    nu_renderpass_handle_t pass,
                    nu_material_handle_t   material,
                    nu_rect_t              extent,
                    nu_rect_t              tex_extent);

#endif

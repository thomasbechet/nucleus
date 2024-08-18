#ifndef NU_RENDERER_H
#define NU_RENDERER_H

#include <nucleus/platform.h>
#include <nucleus/graphics/shared_types.h>

#ifdef NU_BUILD_RENDERER_GL
#include <nucleus/graphics/gl.h>
#endif

typedef enum
{
    NU_RENDERER_NULL,
    NU_RENDERER_GL,
    NU_RENDERER_DX11,
    NU_RENDERER_SOFTRAST,
} nu_renderer_backend_t;

typedef enum
{
    NU_PROJECTION_PERSPECTIVE,
    NU_PROJECTION_ORTHOGRAPHIC,
} nu_projection_t;

typedef struct
{
    nu_projection_t projection;
    float           fov;
    float           near;
    float           far;
    nu_vec3_t       up;
    nu_vec3_t       center;
    nu_vec3_t       eye;
} nu_camera_info_t;

typedef struct
{
#ifdef NU_BUILD_RENDERER_GL
    nugl__handle_t _gl;
#endif
} nu_camera_handle_t;

typedef struct
{
    const nu_vec3_t *positions;
    const nu_vec2_t *uvs;
    const nu_vec3_t *normals;
    nu_size_t        count;
} nu_mesh_info_t;

typedef union
{
#ifdef NU_BUILD_RENDERER_GL
    nugl__handle_t _gl;
#endif
} nu_mesh_handle_t;

typedef enum
{
    NU_TEXTURE_FORMAT_COLOR,
    NU_TEXTURE_FORMAT_DEPTH
} nu_texture_format_t;

typedef enum
{
    NU_TEXTURE_USAGE_TARGET,
    NU_TEXTURE_USAGE_SAMPLE
} nu_texture_usage_t;

typedef struct
{
    nu_uvec2_t          size;
    nu_texture_format_t format;
    nu_texture_usage_t  usage;
    const nu_color_t   *colors;
} nu_texture_info_t;

typedef struct
{
#ifdef NU_BUILD_RENDERER_GL
    nugl__handle_t _gl;
#endif
} nu_texture_handle_t;

typedef struct
{
    nu_u32_t           size;
    nu_texture_usage_t usage;
    const nu_color_t  *colors_posx;
    const nu_color_t  *colors_negx;
    const nu_color_t  *colors_posy;
    const nu_color_t  *colors_negy;
    const nu_color_t  *colors_posz;
    const nu_color_t  *colors_negz;
} nu_cubemap_info_t;

typedef struct
{
#ifdef NU_BUILD_RENDERER_GL
    nugl__handle_t _gl;
#endif
} nu_cubemap_handle_t;

typedef struct
{
    const nu_texture_handle_t *color0;
    const nu_texture_handle_t *color1;
    nu_mat3_t                  uv_transform;
} nu_material_mesh_t;

typedef struct
{
    const nu_texture_handle_t *color0;
    nu_texture_wrap_mode_t     wrap_mode;
} nu_material_canvas_t;

typedef struct
{
    nu_material_type_t type;
    union
    {
        nu_material_mesh_t   mesh;
        nu_material_canvas_t canvas;
    };
} nu_material_info_t;

typedef struct
{
#ifdef NU_BUILD_RENDERER_GL
    nugl__handle_t _gl;
#endif
} nu_material_handle_t;

typedef union
{
#ifdef NU_BUILD_RENDERER_GL
    nugl__context_t gl;
#endif
} nu__renderer_backend_t;

typedef struct
{
    int todo;
} nu_renderpass_unlit_info_t;

typedef struct
{
    int todo;
} nu_renderpass_flat_info_t;

typedef struct
{
    int todo;
} nu_renderpass_transparent_info_t;

typedef struct
{
    int todo;
} nu_renderpass_skybox_info_t;

typedef struct
{
    nu_renderpass_type_t type;
    nu_bool_t            reset_after_submit;
    union
    {
        nu_renderpass_unlit_info_t       unlit;
        nu_renderpass_flat_info_t        flat;
        nu_renderpass_transparent_info_t transparent;
    };
} nu_renderpass_info_t;

typedef struct
{
    nu_camera_handle_t         camera;
    const nu_texture_handle_t *color_target;
    const nu_texture_handle_t *depth_target;
    nu_color_t                *clear_color;
    nu_cubemap_handle_t       *clear_skybox;
} nu_renderpass_submit_unlit_t;

typedef struct
{
    nu_camera_handle_t         camera;
    const nu_texture_handle_t *color_target;
    const nu_texture_handle_t *depth_target;
    nu_color_t                *clear_color;
} nu_renderpass_submit_flat_t;

typedef struct
{
    nu_camera_handle_t         camera;
    const nu_texture_handle_t *color_target;
    const nu_texture_handle_t *depth_target;
    nu_cubemap_handle_t        cubemap;
    nu_quat_t                  rotation;
} nu_renderpass_submit_skybox_t;

typedef struct
{
    const nu_texture_handle_t *color_target;
} nu_renderpass_submit_canvas_t;

typedef union
{
    nu_renderpass_submit_unlit_t  unlit;
    nu_renderpass_submit_flat_t   flat;
    nu_renderpass_submit_skybox_t skybox;
    nu_renderpass_submit_canvas_t canvas;
} nu_renderpass_submit_t;

typedef union
{
#ifdef NU_BUILD_RENDERER_GL
    nugl__handle_t _gl;
#endif
} nu_renderpass_handle_t;

struct nu_renderer;

typedef struct
{
    // Engine API
    nu_error_t (*init)(struct nu_renderer *ctx,
                       nu_allocator_t     *allocator,
                       nu_uvec2_t          size);
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
    nu_logger_t            _logger;
    nu_allocator_t         _allocator;
    nu_renderer_api_t      _api;
    nu__renderer_backend_t _backend;
    nu_texture_handle_t    _surface_color;
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

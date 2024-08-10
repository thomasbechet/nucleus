#ifndef NU_RENDERER_H
#define NU_RENDERER_H

#include <nucleus/platform.h>
#include <nucleus/graphics/shared_types.h>

#ifdef NU_BUILD_RENDERER_GL
#include <nucleus/graphics/gl.h>
#endif

#define NU_CAMERA_DEFAULT_PROJECTION NU_PROJECTION_PERSPECTIVE
#define NU_CAMERA_DEFAULT_FOV        80.0
#define NU_CAMERA_DEFAULT_NEAR       0.01
#define NU_CAMERA_DEFAULT_FAR        100.0
#define NU_CAMERA_DEFAULT_EYE        NU_VEC3_ZERO
#define NU_CAMERA_DEFAULT_CENTER     NU_VEC3_FORWARD
#define NU_CAMERA_DEFAULT_UP         NU_VEC3_UP

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
    nugl__camera_t gl;
#endif
} nu_camera_t;

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
    nugl__mesh_t gl;
#endif
} nu_mesh_t;

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
    nu_uvec2_t _size;
#ifdef NU_BUILD_RENDERER_GL
    nugl__texture_t _gl;
#endif
} nu_texture_t;

typedef struct
{
    const nu_texture_t *texture0;
    const nu_texture_t *texture1;
    nu_mat3_t           uv_transform;
} nu_material_info_t;

typedef struct
{
#ifdef NU_BUILD_RENDERER_GL
    nugl__material_t gl;
#endif
} nu_material_t;

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
    nu_renderpass_type_t type;
    union
    {
        nu_renderpass_unlit_info_t       unlit;
        nu_renderpass_flat_info_t        flat;
        nu_renderpass_transparent_info_t transparent;
    };
} nu_renderpass_info_t;

typedef struct
{
    const nu_camera_t  *camera;
    const nu_texture_t *color_target;
    const nu_texture_t *depth_target;
    nu_color_t          clear_color;
} nu_renderpass_submit_unlit_t;

typedef struct
{
    const nu_camera_t  *camera;
    const nu_texture_t *color_target;
    const nu_texture_t *depth_target;
    nu_color_t          clear_color;
} nu_renderpass_submit_flat_t;

typedef struct
{
    nu_bool_t reset;
    union
    {
        nu_renderpass_submit_unlit_t unlit;
        nu_renderpass_submit_flat_t  flat;
    };
} nu_renderpass_submit_t;

typedef union
{
#ifdef NU_BUILD_RENDERER_GL
    nugl__renderpass_t gl;
#endif
} nu_renderpass_t;

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
    nu_texture_t (*create_surface_color)(struct nu_renderer *ctx);

    // Resources API
    nu_error_t (*create_camera)(struct nu_renderer     *ctx,
                                const nu_camera_info_t *info,
                                nu_camera_t            *camera);
    nu_error_t (*delete_camera)(struct nu_renderer *ctx, nu_camera_t *camera);
    nu_error_t (*update_camera)(struct nu_renderer     *ctx,
                                nu_camera_t            *camera,
                                const nu_camera_info_t *info);

    nu_error_t (*create_mesh)(struct nu_renderer   *ctx,
                              const nu_mesh_info_t *info,
                              nu_mesh_t            *mesh);
    nu_error_t (*delete_mesh)(struct nu_renderer *ctx, nu_mesh_t *mesh);

    nu_error_t (*create_texture)(struct nu_renderer      *ctx,
                                 const nu_texture_info_t *info,
                                 nu_texture_t            *texture);
    nu_error_t (*delete_texture)(struct nu_renderer *ctx,
                                 nu_texture_t       *texture);

    nu_error_t (*create_material)(struct nu_renderer       *ctx,
                                  const nu_material_info_t *info,
                                  nu_material_t            *material);
    nu_error_t (*delete_material)(struct nu_renderer *ctx,
                                  nu_material_t      *material);
    nu_error_t (*update_material)(struct nu_renderer       *ctx,
                                  nu_material_t            *material,
                                  const nu_material_info_t *info);

    nu_error_t (*create_renderpass)(struct nu_renderer         *ctx,
                                    const nu_renderpass_info_t *info,
                                    nu_renderpass_t            *pass);
    nu_error_t (*delete_renderpass)(struct nu_renderer *ctx,
                                    nu_renderpass_t    *pass);

    // Commands API
    void (*draw)(struct nu_renderer  *ctx,
                 nu_renderpass_t     *pass,
                 const nu_mesh_t     *mesh,
                 const nu_material_t *material,
                 const nu_mat4_t     *transform);
    void (*submit_renderpass)(struct nu_renderer           *ctx,
                              nu_renderpass_t              *pass,
                              const nu_renderpass_submit_t *info);
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
    nu_texture_t           _surface_color;
} nu_renderer_t;

typedef struct
{
    nu_vec3_t _position;
    nu_quat_t _rotation;
    nu_vec3_t _velocity;
    float     _yaw;
    float     _pitch;

    float     fov;
    nu_bool_t free_mode;
    float     speed;
    float     view_pitch_neg;
    float     view_pitch_pos;
    float     view_yaw_neg;
    float     view_yaw_pos;
    float     view_roll_neg;
    float     view_roll_pos;
    float     move_up;
    float     move_down;
    float     move_left;
    float     move_right;
    float     move_forward;
    float     move_backward;
} nu_camera_controller_t;

NU_API nu_renderer_info_t nu_renderer_info_default(void);

NU_API nu_error_t nu_renderer_init(nu_platform_t            *platform,
                                   const nu_renderer_info_t *info,
                                   nu_renderer_t            *renderer);
NU_API nu_error_t nu_renderer_terminate(nu_renderer_t *renderer);
NU_API nu_error_t nu_render(nu_platform_t *platform, nu_renderer_t *renderer);
NU_API const nu_texture_t *nu_surface_color_target(
    const nu_platform_t *platform, const nu_renderer_t *renderer);

NU_API nu_camera_info_t nu_camera_info_default(void);
NU_API nu_error_t       nu_camera_create(nu_renderer_t          *ctx,
                                         const nu_camera_info_t *info,
                                         nu_camera_t            *camera);
NU_API nu_error_t nu_camera_delete(nu_renderer_t *ctx, nu_camera_t *camera);
NU_API nu_error_t nu_camera_update(nu_renderer_t          *ctx,
                                   nu_camera_t            *camera,
                                   const nu_camera_info_t *info);

NU_API nu_error_t nu_mesh_create(nu_renderer_t        *ctx,
                                 const nu_mesh_info_t *info,
                                 nu_mesh_t            *mesh);
NU_API nu_error_t nu_mesh_delete(nu_renderer_t *ctx, nu_mesh_t *mesh);

NU_API nu_error_t nu_texture_create(nu_renderer_t           *ctx,
                                    const nu_texture_info_t *info,
                                    nu_texture_t            *texture);
NU_API nu_error_t nu_texture_create_color(nu_renderer_t *ctx,
                                          nu_color_t     color,
                                          nu_texture_t  *texture);
NU_API nu_error_t nu_texture_delete(nu_renderer_t *ctx, nu_texture_t *texture);

NU_API nu_material_info_t nu_material_info_default(void);
NU_API nu_error_t         nu_material_create(nu_renderer_t            *ctx,
                                             const nu_material_info_t *info,
                                             nu_material_t            *material);
NU_API nu_error_t         nu_material_delete(nu_renderer_t *ctx,
                                             nu_material_t *material);
NU_API nu_error_t         nu_material_update(nu_renderer_t            *ctx,
                                             nu_material_t            *material,
                                             const nu_material_info_t *info);

NU_API nu_error_t nu_renderpass_create(nu_renderer_t              *ctx,
                                       const nu_renderpass_info_t *info,
                                       nu_renderpass_t            *pass);
NU_API nu_error_t nu_renderpass_delete(nu_renderer_t   *ctx,
                                       nu_renderpass_t *pass);

NU_API void nu_renderpass_submit(nu_renderer_t                *ctx,
                                 nu_renderpass_t              *pass,
                                 const nu_renderpass_submit_t *info);
NU_API void nu_draw(nu_renderer_t       *ctx,
                    nu_renderpass_t     *renderpass,
                    const nu_mesh_t     *mesh,
                    const nu_material_t *material,
                    const nu_mat4_t     *transform);

NU_API void nu_camera_controller_init(nu_camera_controller_t *controller);
NU_API void nu_camera_controller_update(nu_camera_controller_t *controller,
                                        float                   dt,
                                        nu_camera_info_t       *info);

#endif

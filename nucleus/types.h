#ifndef NU_TYPES_H
#define NU_TYPES_H

#include <nucleus/config.h>

#ifdef NU_BUILD_GLFW
#define GLFW_INCLUDE_NONE
#define GLAD_GL_IMPLEMENTATION
#include <nucleus/external/glad/gl.h>
#include <nucleus/external/glfw-3.4/include/GLFW/glfw3.h>
#endif

//////////////////////////////////////////////////////////////////////////
//////                          Math Types                          //////
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//////                          Time Types                          //////
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//////                        Memory Types                          //////
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//////                        Logger Types                          //////
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//////                        Surface Types                         //////
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//////                         Input Types                          //////
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//////                       Renderer Types                         //////
//////////////////////////////////////////////////////////////////////////

#define NU_CAMERA_DEFAULT_PROJECTION NU_PROJECTION_PERSPECTIVE
#define NU_CAMERA_DEFAULT_FOV        80.0
#define NU_CAMERA_DEFAULT_NEAR       0.01
#define NU_CAMERA_DEFAULT_FAR        100.0
#define NU_CAMERA_DEFAULT_EYE        NU_VEC3_ZERO
#define NU_CAMERA_DEFAULT_CENTER     NU_VEC3_FORWARD
#define NU_CAMERA_DEFAULT_UP         NU_VEC3_UP

#define NU_COLOR_WHITE nu_color(255, 255, 255, 0)
#define NU_COLOR_BLACK nu_color(0, 0, 0, 0)
#define NU_COLOR_RED   nu_color(255, 0, 0, 0)

typedef enum
{
    NU_RENDERPASS_UNLIT,
    NU_RENDERPASS_FLAT,
    NU_RENDERPASS_TRANSPARENT,
} nu_renderpass_type_t;

#ifdef NU_BUILD_RENDERER_GL

#define NUGL__VERTEX_SIZE            (3 + 2)
#define NUGL__MAX_COMMAND_COUNT      128
#define NUGL__MAX_RENDERPASS_COUNT   32
#define NUGL__MAX_RENDERTARGET_COUNT 32

typedef struct
{
    GLuint    ubo;
    nu_mat4_t vp;
    nu_mat4_t ivp;
} nugl__camera_t;

typedef struct
{
    GLuint    vao;
    GLuint    vbo;
    nu_size_t vertex_count;
} nugl__mesh_t;

typedef struct
{
    GLuint    texture0;
    GLuint    texture1;
    nu_mat3_t uv_transform;
} nugl__material_t;

typedef struct
{
    GLuint     texture;
    nu_uvec2_t size;
} nugl__texture_t;

typedef enum
{
    NUGL__DRAW,
    NUGL__DRAW_INSTANCED,
} nugl__command_type_t;

typedef struct
{
    nugl__command_type_t type;
    GLuint               vao;
    GLuint               vbo;
    nu_size_t            vcount;
    nu_mat4_t            transform; // TODO: remove me
    GLuint               texture0;
    GLuint               texture1;
    nu_mat3_t            uv_transform;
} nugl__command_t;

typedef struct
{
    nugl__command_t *commands;
    nu_size_t        count;
} nugl__command_buffer_t;

typedef struct
{
    nu_renderpass_type_t   type;
    nugl__command_buffer_t cmds;
    nu_mat4_t              vp;
    nu_mat4_t              ivp;
    nu_color_t             clear_color;
    GLuint                 depth_target;
    GLuint                 color_target;
    GLuint                 fbo;
    nu_uvec2_t             fbo_size;
    nu_bool_t              reset;
} nugl__renderpass_data_t;

typedef struct
{
    nu_u32_t id;
} nugl__renderpass_t;

typedef struct
{
    GLuint color;
    GLuint depth;
    GLuint fbo;
} nugl__rendertarget_t;

typedef struct
{
    nu_allocator_t allocator;
    nu_uvec2_t     surface_size;
    GLuint         surface_color;

    GLuint blit_program;
    GLuint flat_program;
    GLuint nearest_sampler;

    nugl__rendertarget_t targets[NUGL__MAX_RENDERTARGET_COUNT];
    nu_size_t            target_count;

    nugl__renderpass_data_t passes[NUGL__MAX_RENDERPASS_COUNT];
    nu_u32_t                pass_count;
    nu_u32_t                pass_order[NUGL__MAX_RENDERPASS_COUNT];
    nu_u32_t                pass_order_count;
} nugl__context_t;

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
} nu_texture_info_t;

typedef struct
{
#ifdef NU_BUILD_RENDERER_GL
    nugl__texture_t gl;
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

typedef struct
{
    // Engine API
    nu_error_t (*init)(void *ctx, nu_allocator_t allocator, nu_uvec2_t size);
    nu_error_t (*render)(void            *ctx,
                         const nu_rect_t *global_viewport,
                         const nu_rect_t *viewport);
    nu_texture_t (*create_surface_color)(void *ctx);

    // Resources API
    nu_error_t (*create_camera)(void                   *ctx,
                                const nu_camera_info_t *info,
                                nu_camera_t            *camera);
    nu_error_t (*delete_camera)(void *ctx, nu_camera_t *camera);
    nu_error_t (*update_camera)(void                   *ctx,
                                nu_camera_t            *camera,
                                const nu_camera_info_t *info);

    nu_error_t (*create_mesh)(void                 *ctx,
                              const nu_mesh_info_t *info,
                              nu_mesh_t            *mesh);
    nu_error_t (*delete_mesh)(void *ctx, nu_mesh_t *mesh);

    nu_error_t (*create_texture)(void                    *ctx,
                                 const nu_texture_info_t *info,
                                 nu_texture_t            *texture);
    nu_error_t (*delete_texture)(void *ctx, nu_texture_t *texture);
    nu_error_t (*write_texture)(void             *ctx,
                                nu_texture_t     *texture,
                                nu_rect_t         rect,
                                const nu_color_t *colors);

    nu_error_t (*create_material)(void                     *ctx,
                                  const nu_material_info_t *info,
                                  nu_material_t            *material);
    nu_error_t (*delete_material)(void *ctx, nu_material_t *material);
    nu_error_t (*update_material)(void                     *ctx,
                                  nu_material_t            *material,
                                  const nu_material_info_t *info);

    nu_error_t (*create_renderpass)(void                       *ctx,
                                    const nu_renderpass_info_t *info,
                                    nu_renderpass_t            *pass);
    nu_error_t (*delete_renderpass)(void *ctx, nu_renderpass_t *pass);

    // Commands API
    void (*draw)(void                *ctx,
                 nu_renderpass_t     *pass,
                 const nu_mesh_t     *mesh,
                 const nu_material_t *material,
                 const nu_mat4_t     *transform);
    void (*submit_renderpass)(void                         *ctx,
                              nu_renderpass_t              *pass,
                              const nu_renderpass_submit_t *info);
} nu_renderer_api_t;

typedef struct
{
    nu_logger_t            logger;
    nu_renderer_api_t      api;
    void                  *ctx;
    nu__renderer_backend_t backend;
    nu_texture_t           surface_color;
} nu__renderer_t;

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

//////////////////////////////////////////////////////////////////////////
//////                        Context Types                         //////
//////////////////////////////////////////////////////////////////////////

#endif

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
//////                          Error Types                         //////
//////////////////////////////////////////////////////////////////////////

typedef enum
{
    NU_ERROR_NONE               = 0,
    NU_ERROR_UNSUPPORTED_API    = 1,
    NU_ERROR_BACKEND            = 2,
    NU_ERROR_DUPLICATED         = 3,
    NU_ERROR_OUT_OF_RESOURCE    = 4,
    NU_ERROR_SHADER_COMPILATION = 5,
} nu_error_t;

//////////////////////////////////////////////////////////////////////////
//////                          Basic Types                         //////
//////////////////////////////////////////////////////////////////////////

// TODO: use stdint types
typedef unsigned char  nu_u8_t;
typedef signed char    nu_i8_t;
typedef unsigned short nu_u16_t;
typedef signed short   nu_i16_t;
typedef unsigned int   nu_u32_t;
typedef signed int     nu_i32_t;
typedef unsigned long  nu_u64_t;
typedef signed long    nu_i64_t;

typedef char          nu_char_t;
typedef int           nu_bool_t;
typedef nu_i32_t      nu_int_t;
typedef unsigned long nu_size_t;
typedef int           nu_word_t;

#define NU_TRUE  1
#define NU_FALSE 0
#define NU_NULL  0
#define NU_NOOP

//////////////////////////////////////////////////////////////////////////
//////                          Math Types                          //////
//////////////////////////////////////////////////////////////////////////

#define NU_PI 3.14159265359

#define NU_MIN(a, b) (((a) < (b)) ? (a) : (b))
#define NU_MAX(a, b) (((a) > (b)) ? (a) : (b))

#define NU_VEC2_SIZE 2
#define NU_VEC3_SIZE 3
#define NU_VEC4_SIZE 4
#define NU_MAT3_SIZE 9
#define NU_MAT4_SIZE 16

#define NU_VEC2_ZERO nu_vec2(0, 0)

#define NU_VEC3_ZERO     nu_vec3(0, 0, 0)
#define NU_VEC3_UP       nu_vec3(0, 1, 0)
#define NU_VEC3_DOWN     nu_vec3(0, -1, 0)
#define NU_VEC3_FORWARD  nu_vec3(0, 0, -1)
#define NU_VEC3_BACKWARD nu_vec3(0, 0, 1)
#define NU_VEC3_LEFT     nu_vec3(-1, 0, 0)
#define NU_VEC3_RIGHT    nu_vec3(1, 0, 0)

#define NU_VEC4_ZERO nu_vec4(0, 0, 0, 0)

typedef union
{
    struct
    {
        float x;
        float y;
    };
    float xy[NU_VEC2_SIZE];
} nu_vec2_t;

typedef union
{
    struct
    {
        float x;
        float y;
        float z;
    };
    float xyz[NU_VEC3_SIZE];
} nu_vec3_t;

typedef union
{
    struct
    {
        float x;
        float y;
        float z;
        float w;
    };
    float xyzw[NU_VEC4_SIZE];
} nu_vec4_t;

typedef union
{
    struct
    {
        nu_u32_t x;
        nu_u32_t y;
    };
    nu_u32_t xy[NU_VEC2_SIZE];
} nu_uvec2_t;

typedef union
{
    struct
    {
        nu_u32_t x;
        nu_u32_t y;
        nu_u32_t z;
        nu_u32_t w;
    };
    nu_u32_t xyzw[NU_VEC4_SIZE];
} nu_uvec4_t;

typedef union
{
    struct
    {
        float x1;
        float x2;
        float x3;
        float x4;
        float y1;
        float y2;
        float y3;
        float y4;
        float z1;
        float z2;
        float z3;
        float z4;
        float w1;
        float w2;
        float w3;
        float w4;
    };
    float data[NU_MAT4_SIZE];
} nu_mat4_t;

//////////////////////////////////////////////////////////////////////////
//////                          Time Types                          //////
//////////////////////////////////////////////////////////////////////////

typedef struct
{
#if defined(NU_PLATFORM_WINDOWS)
    LARGE_INTEGER value;
#elif defined(NU_PLATFORM_UNIX)
    time_t value;
#endif
} nu_time_t;

typedef struct
{
#if defined(NU_PLATFORM_WINDOWS)
    LARGE_INTEGER t0;
#elif defined(NU_PLATFORM_UNIX)
    struct timespec start;
#endif
} nu_timer_t;

typedef struct
{
    nu_bool_t active;
    nu_u32_t  id;
    float     timestep;
    float     _acc;
} nu_fixed_loop_t;

//////////////////////////////////////////////////////////////////////////
//////                        Memory Types                          //////
//////////////////////////////////////////////////////////////////////////

typedef struct
{
    void *(*callback)(void     *p, // pointer (NULL if malloc)
                      nu_size_t s, // current size (0 if malloc)
                      nu_size_t n, // new size (0 if free)
                      nu_size_t a, // alignment
                      void     *u);    // userdata
    void *userdata;
} nu_allocator_t;

//////////////////////////////////////////////////////////////////////////
//////                        Logger Types                          //////
//////////////////////////////////////////////////////////////////////////

typedef struct
{
    nu_u32_t todo;
} nu__logger_t;

typedef enum
{
    NU_LOG_DEBUG   = 5,
    NU_LOG_INFO    = 4,
    NU_LOG_WARNING = 3,
    NU_LOG_ERROR   = 2,
    NU_LOG_FATAL   = 1
} nu_log_level_t;

typedef struct
{
    nu_log_level_t level;
} nu_logger_t;

//////////////////////////////////////////////////////////////////////////
//////                        Surface Types                         //////
//////////////////////////////////////////////////////////////////////////

#ifdef NU_BUILD_GLFW

#define NUGLFW_ID_NONE     0xFFFFFFFF
#define NUGLFW_MAX_BINDING 128
#define NUGLFW_MAX_INPUT   128

#define NUGLFW_WINDOW_WIDTH  1280
#define NUGLFW_WINDOW_HEIGHT 720

typedef enum
{
    NUEXT_VIEWPORT_FIXED,
    NUEXT_VIEWPORT_FIXED_BEST_FIT,
    NUEXT_VIEWPORT_STRETCH_KEEP_ASPECT,
    NUEXT_VIEWPORT_STRETCH,
} nuext_viewport_mode_t;

typedef struct
{
    nuext_viewport_mode_t mode;
    float                 scale_factor;
    nu_uvec2_t            screen;
    nu_uvec4_t            extent;
    nu_vec4_t             viewport;
} nuglfw__viewport_t;

typedef struct
{
    GLFWwindow        *win;
    nuglfw__viewport_t viewport;
} nuglfw__surface_t;

#endif

//////////////////////////////////////////////////////////////////////////
//////                         Input Types                          //////
//////////////////////////////////////////////////////////////////////////

#define NU_INPUT_PRESSED       1.0f
#define NU_INPUT_RELEASED      0.0f
#define NU_INPUT_IS_PRESSED(x) (x > 0.5f)

typedef struct
{
    float value;
    float previous;
} nu__input_state_t;

typedef struct
{
#ifdef NU_BUILD_GLFW
    nu_u32_t _glfwid;
#endif
} nu_input_t;

typedef enum
{
    // Keys
    NUEXT_BUTTON_A = 0,
    NUEXT_BUTTON_B = 1,
    NUEXT_BUTTON_C = 2,
    NUEXT_BUTTON_D = 3,
    NUEXT_BUTTON_E = 4,
    NUEXT_BUTTON_F = 5,
    NUEXT_BUTTON_G = 6,
    NUEXT_BUTTON_H = 7,
    NUEXT_BUTTON_I = 8,
    NUEXT_BUTTON_J = 9,
    NUEXT_BUTTON_K = 10,
    NUEXT_BUTTON_L = 11,
    NUEXT_BUTTON_M = 12,
    NUEXT_BUTTON_N = 13,
    NUEXT_BUTTON_O = 14,
    NUEXT_BUTTON_P = 15,
    NUEXT_BUTTON_Q = 16,
    NUEXT_BUTTON_R = 17,
    NUEXT_BUTTON_S = 18,
    NUEXT_BUTTON_T = 19,
    NUEXT_BUTTON_U = 20,
    NUEXT_BUTTON_V = 21,
    NUEXT_BUTTON_W = 22,
    NUEXT_BUTTON_X = 23,
    NUEXT_BUTTON_Y = 24,
    NUEXT_BUTTON_Z = 25,

    NUEXT_BUTTON_F1  = 26,
    NUEXT_BUTTON_F2  = 27,
    NUEXT_BUTTON_F3  = 28,
    NUEXT_BUTTON_F4  = 29,
    NUEXT_BUTTON_F5  = 30,
    NUEXT_BUTTON_F6  = 31,
    NUEXT_BUTTON_F7  = 32,
    NUEXT_BUTTON_F8  = 33,
    NUEXT_BUTTON_F9  = 34,
    NUEXT_BUTTON_F10 = 35,
    NUEXT_BUTTON_F11 = 36,
    NUEXT_BUTTON_F12 = 37,

    NUEXT_BUTTON_0 = 38,
    NUEXT_BUTTON_1 = 39,
    NUEXT_BUTTON_2 = 40,
    NUEXT_BUTTON_3 = 41,
    NUEXT_BUTTON_4 = 42,
    NUEXT_BUTTON_5 = 43,
    NUEXT_BUTTON_6 = 44,
    NUEXT_BUTTON_7 = 45,
    NUEXT_BUTTON_8 = 46,
    NUEXT_BUTTON_9 = 47,

    NUEXT_BUTTON_ESCAPE       = 48,
    NUEXT_BUTTON_SPACE        = 49,
    NUEXT_BUTTON_ENTER        = 50,
    NUEXT_BUTTON_TAB          = 51,
    NUEXT_BUTTON_BACKSPACE    = 52,
    NUEXT_BUTTON_LEFT_SHIFT   = 53,
    NUEXT_BUTTON_LEFT_CONTROL = 54,
    NUEXT_BUTTON_LEFT         = 55,
    NUEXT_BUTTON_RIGHT        = 56,
    NUEXT_BUTTON_UP           = 57,
    NUEXT_BUTTON_DOWN         = 58,

    // Mouse
    NUEXT_BUTTON_MOUSE_LEFT   = 59,
    NUEXT_BUTTON_MOUSE_RIGHT  = 60,
    NUEXT_BUTTON_MOUSE_MIDDLE = 61,
    NUEXT_BUTTON_MOUSE_1      = 62,
    NUEXT_BUTTON_MOUSE_2      = 63,
    NUEXT_BUTTON_MOUSE_3      = 64,
    NUEXT_BUTTON_MOUSE_4      = 65,
    NUEXT_BUTTON_MOUSE_5      = 66,
} nuext_button_t;

typedef enum
{
    NUEXT_AXIS_MOUSE_X,
    NUEXT_AXIS_MOUSE_Y,
} nuext_axis_t;

#ifdef NU_BUILD_GLFW

typedef struct
{
    nu_u32_t id;
    nu_u32_t next;
} nuglfw__binding_t;

typedef union
{
    nu__input_state_t state;
    nu_u32_t          free;
} nuglfw__input_state_t;

typedef struct
{
    nu_u32_t              free_binding;
    nu_u32_t              free_input;
    nuglfw__binding_t     bindings[NUGLFW_MAX_BINDING];
    nuglfw__input_state_t inputs[NUGLFW_MAX_INPUT];
    nu_u32_t              input_count;
    nu_u32_t              key_to_first_binding[GLFW_KEY_LAST];
    nu_u32_t              mouse_button_to_first_binding[GLFW_MOUSE_BUTTON_LAST];
    nu_vec2_t             mouse_position;
    nu_vec2_t             mouse_old_position;
    nu_vec2_t             mouse_scroll;
    nu_vec2_t             mouse_motion;
} nuglfw__input_t;

#endif

//////////////////////////////////////////////////////////////////////////
//////                       Renderer Types                         //////
//////////////////////////////////////////////////////////////////////////

#define NU_CAMERA_DEFAULT_PROJECTION NU_PROJECTION_PERSPECTIVE
#define NU_CAMERA_DEFAULT_FOV        90.0f
#define NU_CAMERA_DEFAULT_NEAR       0.01f
#define NU_CAMERA_DEFAULT_FAR        100.0f
#define NU_CAMERA_DEFAULT_EYE        NU_VEC3_ZERO
#define NU_CAMERA_DEFAULT_CENTER     NU_VEC3_FORWARD
#define NU_CAMERA_DEFAULT_UP         NU_VEC3_UP

typedef enum
{
    NU_RENDERPASS_FLAT,
    NU_RENDERPASS_TRANSPARENT,
} nu_renderpass_type_t;

#ifdef NU_BUILD_RENDERER_GL

#define NUGL__VERTEX_SIZE          (3 + 2)
#define NUGL__MAX_COMMAND_COUNT    128
#define NUGL__MAX_RENDERPASS_COUNT 32

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
    GLuint diffuse;
} nugl__material_t;

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
    nu_bool_t              reset;
} nugl__renderpass_data_t;

typedef struct
{
    nu_u32_t id;
} nugl__renderpass_t;

typedef struct
{
    nu_allocator_t allocator;

    GLuint blit_program;
    GLuint flat_program;
    GLuint nearest_sampler;

    GLuint     surface_fbo;
    GLuint     surface_texture;
    nu_uvec2_t surface_size;

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

typedef struct
{
    const nu_vec3_t *positions;
    const nu_vec2_t *uvs;
    nu_size_t        count;
} nu_mesh_info_t;

typedef enum
{
    NU_PROJECTION_PERSPECTIVE,
    NU_PROJECTION_ORTHOGRAPHIC,
} nu_projection_t;

typedef union
{
#ifdef NU_BUILD_RENDERER_GL
    nugl__camera_t gl;
#endif
} nu__camera_data_t;

typedef struct
{
    nu_projection_t   projection;
    float             fov;
    float             near;
    float             far;
    nu_vec3_t         up;
    nu_vec3_t         center;
    nu_vec3_t         eye;
    nu__camera_data_t _data;
} nu_camera_t;

typedef union
{
#ifdef NU_BUILD_RENDERER_GL
    nugl__mesh_t gl;
#endif
} nu_mesh_t;

typedef union
{
#ifdef NU_BUILD_RENDERER_GL
    nugl__material_t gl;
#endif
} nu__material_data_t;

typedef struct
{
    nu__material_data_t _data;
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
        nu_renderpass_flat_info_t        flat;
        nu_renderpass_transparent_info_t transparent;
    };
} nu_renderpass_info_t;

typedef struct
{
    const nu_camera_t *camera;
} nu_renderpass_submit_flat_t;

typedef struct
{
    nu_bool_t reset;
    union
    {
        nu_renderpass_submit_flat_t flat;
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
    nu_error_t (*render)(void             *ctx,
                         const nu_uvec4_t *global_viewport,
                         const nu_vec4_t  *viewport);

    // Resources API
    nu_error_t (*create_camera)(void *ctx, nu_camera_t *camera);
    nu_error_t (*delete_camera)(void *ctx, nu_camera_t *camera);
    nu_error_t (*update_camera)(void *ctx, nu_camera_t *camera);
    nu_error_t (*create_mesh)(void                 *ctx,
                              const nu_mesh_info_t *info,
                              nu_mesh_t            *mesh);
    nu_error_t (*delete_mesh)(void *ctx, nu_mesh_t *mesh);
    nu_error_t (*create_renderpass)(void                       *ctx,
                                    const nu_renderpass_info_t *info,
                                    nu_renderpass_t            *pass);
    nu_error_t (*delete_renderpass)(void *ctx, nu_renderpass_t *pass);

    // Commands API
    void (*draw)(void            *ctx,
                 nu_renderpass_t *pass,
                 const nu_mesh_t *mesh,
                 const nu_mat4_t *transform);
    void (*submit_renderpass)(void                         *ctx,
                              nu_renderpass_t              *pass,
                              const nu_renderpass_submit_t *info);
} nu_renderer_api_t;

typedef struct
{
    nu_renderer_api_t      api;
    void                  *ctx;
    nu__renderer_backend_t backend;
} nu__renderer_t;

//////////////////////////////////////////////////////////////////////////
//////                        Context Types                         //////
//////////////////////////////////////////////////////////////////////////

typedef struct
{
    nu_u32_t              width;
    nu_u32_t              height;
    nu_renderer_backend_t renderer;
    nu_allocator_t        allocator;
} nu_context_info_t;

typedef struct
{
    nu_allocator_t        _allocator;
    nu_uvec2_t            _surface_size;
    nu_renderer_backend_t _renderer_backend;
    nu_bool_t             _close_requested;
#ifdef NU_BUILD_GLFW
    nuglfw__surface_t _glfw_surface;
    nuglfw__input_t   _glfw_input;
#endif
    nu__renderer_t _renderer;
    nu__logger_t   _logger;
} nu_context_t;

#endif

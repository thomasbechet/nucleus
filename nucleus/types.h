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

#define NU_V2 2
#define NU_V3 3
#define NU_V4 4
#define NU_M3 9
#define NU_M4 16

#define NU_V3_ZERO     ((const float[]) { 0.0, 0.0, 0.0 })
#define NU_V3_UP       ((const float[]) { 0.0, 1.0, 0.0 })
#define NU_V3_DOWN     ((const float[]) { 0.0, -1.0, 0.0 })
#define NU_V3_FORWARD  ((const float[]) { 0.0, 0.0, -1.0 })
#define NU_V3_BACKWARD ((const float[]) { 0.0, 0.0, -1.0 })
#define NU_V3_LEFT     ((const float[]) { -1.0, 0.0, 0.0 })
#define NU_V3_RIGHT    ((const float[]) { 1.0, 0.0, 0.0 })

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
    nu_i32_t              screen[NU_V2];
    nu_i32_t              extent[NU_V4];
    float                 viewport[NU_V4];
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
    float                 mouse_position[NU_V2];
    float                 mouse_old_position[NU_V2];
    float                 mouse_scroll[NU_V2];
    float                 mouse_motion[NU_V2];
} nuglfw__input_t;

#endif

//////////////////////////////////////////////////////////////////////////
//////                       Renderer Types                         //////
//////////////////////////////////////////////////////////////////////////

#define NU_CAMERA_DEFAULT_PROJECTION NU_PROJECTION_PERSPECTIVE
#define NU_CAMERA_DEFAULT_FOV        90.0f
#define NU_CAMERA_DEFAULT_NEAR       0.01f
#define NU_CAMERA_DEFAULT_FAR        100.0f
#define NU_CAMERA_DEFAULT_EYE        NU_V3_ZERO
#define NU_CAMERA_DEFAULT_CENTER     NU_V3_FORWARD
#define NU_CAMERA_DEFAULT_UP         NU_V3_UP

#ifdef NU_BUILD_RENDERER_GL

#define NUGL_VERTEX_SIZE (3 + 2)

typedef struct
{
    GLuint ubo;
    float  vp[NU_M4];
    float  ivp[NU_M4];
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

typedef struct
{
    GLuint          blit_program;
    GLuint          flat_program;
    GLuint          nearest_sampler;
    GLuint          surface_fbo;
    GLuint          surface_texture;
    nu_int_t        surface_size[NU_V2];
    nugl__mesh_t   *mesh;
    nugl__camera_t *cam;
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
    const float *positions;
    const float *uvs;
    nu_size_t    vertex_count;
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
    float             eye[NU_V3];
    float             center[NU_V3];
    float             up[NU_V3];
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

typedef enum
{
    NU_RENDERPASS_FLAT,
    NU_RENDERPASS_TRANSPARENT,
} nu_renderpass_type_t;

typedef struct
{
    nu_renderpass_type_t type;
} nu_renderpass_info_t;

typedef union
{
#ifdef NU_BUILD_RENDERER_GL
    nu_u32_t gl;
#endif
} nu_renderpass_t;

typedef struct
{
    GLuint target;
} nu_renderpass_begin_t;

typedef struct
{
    // Engine API
    nu_error_t (*init)(void *ctx, const nu_int_t size[NU_V2]);
    nu_error_t (*render)(void           *ctx,
                         const nu_int_t *global_viewport,
                         const float    *viewport);

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
    void (*begin_renderpass)(void                        *ctx,
                             nu_renderpass_t             *pass,
                             const nu_renderpass_begin_t *info);
    void (*end_renderpass)(void *ctx, nu_renderpass_t *pass);
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
} nu_context_info_t;

typedef struct
{
    nu_i32_t              _surface_size[NU_V2];
    nu_renderer_backend_t _renderer_backend;
    nu_bool_t             _close_requested;
#ifdef NU_BUILD_GLFW
    nuglfw__surface_t _glfw_surface;
    nuglfw__input_t   _glfw_input;
#endif
    nu__renderer_t _renderer;
} nu_context_t;

#endif

#ifndef NU_CONTEXT_H
#define NU_CONTEXT_H

#include <nucleus/core.h>

//////////////////////////////////////////////////////////////////////////
//////                        Surface Types                         //////
//////////////////////////////////////////////////////////////////////////

#ifdef NU_BUILD_GLFW

#define NUGLFW_ID_NONE     0xFFFFFFFF
#define NUGLFW_MAX_BINDING 128
#define NUGLFW_MAX_INPUT   128

#define NUGLFW_WINDOW_WIDTH  1280
#define NUGLFW_WINDOW_HEIGHT 720

#define NUGLFW_FULLSCREEN_KEY GLFW_KEY_F11

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
    nu_rect_t             extent;
    nu_rect_t             viewport;
} nuglfw__viewport_t;

typedef struct GLFWwindow *NUGLFWwindow;

typedef struct
{
    NUGLFWwindow       win;
    nuglfw__viewport_t viewport;
    nu_bool_t          fullscreen;
    nu_bool_t          switch_fullscreen;
    nu_bool_t          capture_mouse;
    nu_uvec2_t         previous_size;
    nu_uvec2_t         previous_position;
    nu_timer_t         last_mouse_click;
    nu_bool_t          switch_capture_mouse;
} nuglfw__surface_t;

#endif

typedef struct
{
    nu_logger_t logger;
#ifdef NU_BUILD_GLFW
    nuglfw__surface_t glfw;
#endif
    nu_uvec2_t size;
} nu__surface_t;

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

NU_DEFINE_HANDLE(nu_input_handle_t);

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
    NUEXT_AXIS_MOUSE_MOTION_X_POS,
    NUEXT_AXIS_MOUSE_MOTION_X_NEG,
    NUEXT_AXIS_MOUSE_MOTION_Y_POS,
    NUEXT_AXIS_MOUSE_MOTION_Y_NEG,
} nuext_axis_t;

#ifdef NU_BUILD_GLFW

#define NUGLFW_MAX_KEY_COUNT          350 // Taken from GLFW_KEY_LAST
#define NUGLFW_MAX_MOUSE_BUTTON_COUNT 8   // Taken from GLFW_KEY_LAST

typedef struct
{
    float pressed;
} nuglfw__binding_button_t;

typedef struct
{
    float scale;
} nuglfw__binding_axis_t;

typedef struct
{
    nu_u32_t id;
    nu_u32_t next;
    union
    {
        nuglfw__binding_button_t button;
        nuglfw__binding_axis_t   axis;
    };
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
    nu_u32_t              key_to_first_binding[NUGLFW_MAX_KEY_COUNT];
    nu_u32_t  mouse_button_to_first_binding[NUGLFW_MAX_MOUSE_BUTTON_COUNT];
    nu_u32_t  mouse_x_first_binding;
    nu_u32_t  mouse_y_first_binding;
    nu_u32_t  mouse_motion_x_pos_first_binding;
    nu_u32_t  mouse_motion_x_neg_first_binding;
    nu_u32_t  mouse_motion_y_pos_first_binding;
    nu_u32_t  mouse_motion_y_neg_first_binding;
    nu_vec2_t mouse_position;
    nu_vec2_t mouse_old_position;
    nu_vec2_t mouse_scroll;
    nu_vec2_t mouse_motion;
} nuglfw__input_t;

#endif

typedef struct
{
    nu_logger_t logger;
#ifdef NU_BUILD_GLFW
    nuglfw__input_t glfw;
#endif
} nu__input_t;

//////////////////////////////////////////////////////////////////////////
//////                       Context Types                          //////
//////////////////////////////////////////////////////////////////////////

typedef struct
{
    nu_u32_t         width;
    nu_u32_t         height;
    nu_allocator_t   allocator;
    nu_logger_info_t logger;
} nu_platform_info_t;

typedef struct
{
    nu_allocator_t _allocator;
    nu_bool_t      _close_requested;
    nu_logger_t    _logger;
    nu__surface_t  _surface;
    nu__input_t    _input;
} nu_platform_t;

NU_API nu_platform_info_t nu_platform_info_default(void);

NU_API nu_error_t nu_platform_init(const nu_platform_info_t *info,
                                   nu_platform_t            *platform);
NU_API nu_error_t nu_platform_free(nu_platform_t *platform);
NU_API nu_error_t nu_poll_events(nu_platform_t *platform);
NU_API nu_bool_t  nu_exit_requested(const nu_platform_t *platform);

#endif

#ifndef NU_PLATFORM_API_H
#define NU_PLATFORM_API_H

#include <nucleus/core/api.h>

//////////////////////////////////////////////////////////////////////////
//////                          Platform                            //////
//////////////////////////////////////////////////////////////////////////

NU_API nu_error_t nu_poll_events(void);
NU_API nu_bool_t  nu_exit_requested(void);

//////////////////////////////////////////////////////////////////////////
//////                          Surface                             //////
//////////////////////////////////////////////////////////////////////////

NU_API void       nu_config_surface_size(nu_u32_t width, nu_u32_t height);
NU_API nu_error_t nu_swap_buffers(void);
NU_API float      nu_surface_aspect(void);

typedef enum
{
    NUEXT_VIEWPORT_FIXED,
    NUEXT_VIEWPORT_FIXED_BEST_FIT,
    NUEXT_VIEWPORT_STRETCH_KEEP_ASPECT,
    NUEXT_VIEWPORT_STRETCH,
} nuext_viewport_mode_t;

NU_API nu_error_t nuext_viewport_mode(nuext_viewport_mode_t mode);

//////////////////////////////////////////////////////////////////////////
//////                          Input                               //////
//////////////////////////////////////////////////////////////////////////

#define NU_INPUT_PRESSED       1.0f
#define NU_INPUT_RELEASED      0.0f
#define NU_INPUT_IS_PRESSED(x) (x > 0.5f)

NU_DEFINE_HANDLE(nu_input_t);

NU_API nu_input_t nu_input_create(void);
NU_API nu_bool_t  nu_input_changed(nu_input_t input);
NU_API nu_bool_t  nu_input_pressed(nu_input_t input);
NU_API nu_bool_t  nu_input_just_pressed(nu_input_t input);
NU_API nu_bool_t  nu_input_released(nu_input_t input);
NU_API nu_bool_t  nu_input_just_released(nu_input_t input);
NU_API float      nu_input_value(nu_input_t input);

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

    // EXT Mouse
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

NU_API nu_error_t nuext_input_bind_button(nu_input_t     input,
                                          nuext_button_t button);
NU_API nu_error_t nuext_input_bind_button_value(nu_input_t     input,
                                                nuext_button_t button,
                                                float          value);
NU_API nu_error_t nuext_input_bind_axis(nu_input_t input, nuext_axis_t axis);

#endif

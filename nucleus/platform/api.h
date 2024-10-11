#ifndef NU_PLATFORM_API_H
#define NU_PLATFORM_API_H

#include <nucleus/core/api.h>

NU_DEFINE_HANDLE(nu_input_t);

#define NU_INPUT_PRESSED       1.0f
#define NU_INPUT_RELEASED      0.0f
#define NU_INPUT_IS_PRESSED(x) (x > 0.5f)

typedef enum
{
    NUEXT_VIEWPORT_FIXED,
    NUEXT_VIEWPORT_FIXED_BEST_FIT,
    NUEXT_VIEWPORT_STRETCH_KEEP_ASPECT,
    NUEXT_VIEWPORT_STRETCH,
} nuext_viewport_mode_t;

typedef enum
{
    // Mouse
    NUEXT_BUTTON_MOUSE_LEFT,
    NUEXT_BUTTON_MOUSE_MIDDLE,
    NUEXT_BUTTON_MOUSE_RIGHT,
    NUEXT_BUTTON_MOUSE_SCROLLUP,
    NUEXT_BUTTON_MOUSE_SCROLLDOWN,

    // Keyboard
    NUEXT_BUTTON_ESCAPE,
    NUEXT_BUTTON_F1,
    NUEXT_BUTTON_F2,
    NUEXT_BUTTON_F3,
    NUEXT_BUTTON_F4,
    NUEXT_BUTTON_F5,
    NUEXT_BUTTON_F6,
    NUEXT_BUTTON_F7,
    NUEXT_BUTTON_F8,
    NUEXT_BUTTON_F9,
    NUEXT_BUTTON_F10,
    NUEXT_BUTTON_F11,
    NUEXT_BUTTON_F12,

    NUEXT_BUTTON_BACKTICK,

    NUEXT_BUTTON_0,
    NUEXT_BUTTON_1,
    NUEXT_BUTTON_2,
    NUEXT_BUTTON_3,
    NUEXT_BUTTON_4,
    NUEXT_BUTTON_5,
    NUEXT_BUTTON_6,
    NUEXT_BUTTON_7,
    NUEXT_BUTTON_8,
    NUEXT_BUTTON_9,

    NUEXT_BUTTON_MINUS,
    NUEXT_BUTTON_EQUALS,
    NUEXT_BUTTON_BACKSPACE,
    NUEXT_BUTTON_TAB,
    NUEXT_BUTTON_CAPSLOCK,
    NUEXT_BUTTON_SHIFTL,
    NUEXT_BUTTON_CONTROLL,
    NUEXT_BUTTON_ALTL,
    NUEXT_BUTTON_SUPERL,
    NUEXT_BUTTON_SHIFTR,
    NUEXT_BUTTON_CONTROLR,
    NUEXT_BUTTON_ALTR,
    NUEXT_BUTTON_SUPERR,
    NUEXT_BUTTON_SPACE,

    NUEXT_BUTTON_A,
    NUEXT_BUTTON_B,
    NUEXT_BUTTON_C,
    NUEXT_BUTTON_D,
    NUEXT_BUTTON_E,
    NUEXT_BUTTON_F,
    NUEXT_BUTTON_G,
    NUEXT_BUTTON_H,
    NUEXT_BUTTON_I,
    NUEXT_BUTTON_J,
    NUEXT_BUTTON_K,
    NUEXT_BUTTON_L,
    NUEXT_BUTTON_M,
    NUEXT_BUTTON_N,
    NUEXT_BUTTON_O,
    NUEXT_BUTTON_P,
    NUEXT_BUTTON_Q,
    NUEXT_BUTTON_R,
    NUEXT_BUTTON_S,
    NUEXT_BUTTON_T,
    NUEXT_BUTTON_U,
    NUEXT_BUTTON_V,
    NUEXT_BUTTON_W,
    NUEXT_BUTTON_X,
    NUEXT_BUTTON_Y,
    NUEXT_BUTTON_Z,

    NUEXT_BUTTON_PERIOD,
    NUEXT_BUTTON_COMMA,
    NUEXT_BUTTON_SLASH,
    NUEXT_BUTTON_BRACKET,
    NUEXT_BUTTON_CLOSEBRACKET,
    NUEXT_BUTTON_SEMICOLON,
    NUEXT_BUTTON_RETURN,
    NUEXT_BUTTON_QUOTE,
    NUEXT_BUTTON_BACKSLASH,

    NUEXT_BUTTON_UP,
    NUEXT_BUTTON_DOWN,
    NUEXT_BUTTON_LEFT,
    NUEXT_BUTTON_RIGHT,

    NUEXT_BUTTON_DELETE,
    NUEXT_BUTTON_INSERT,
    NUEXT_BUTTON_END,
    NUEXT_BUTTON_HOME,
    NUEXT_BUTTON_PAGEUP,
    NUEXT_BUTTON_PAGEDOWN,

    NUEXT_BUTTON_NUMLOCK,
    NUEXT_BUTTON_KPSLASH,
    NUEXT_BUTTON_MULTIPLY,
    NUEXT_BUTTON_KPMINUS,
    NUEXT_BUTTON_KP1,
    NUEXT_BUTTON_KP2,
    NUEXT_BUTTON_KP3,
    NUEXT_BUTTON_KP4,
    NUEXT_BUTTON_KP5,
    NUEXT_BUTTON_KP6,
    NUEXT_BUTTON_KP7,
    NUEXT_BUTTON_KP8,
    NUEXT_BUTTON_KP9,
    NUEXT_BUTTON_KP0,
    NUEXT_BUTTON_KPPERIOD,
    NUEXT_BUTTON_KPRETURN,
} nuext_button_t;

typedef enum
{
    NUEXT_AXIS_MOUSE_X,
    NUEXT_AXIS_MOUSE_Y,
    NUEXT_AXIS_MOUSE_MOTION_X,
    NUEXT_AXIS_MOUSE_MOTION_Y,
} nuext_axis_t;

NU_API void      nu_poll_events(void);
NU_API nu_bool_t nu_exit_requested(void);

NU_API void     nu_config_surface_size(nu_u32_t width, nu_u32_t height);
NU_API void     nu_swap_buffers(void);
NU_API nu_f32_t nu_surface_aspect(void);

NU_API nu_error_t nuext_viewport_mode(nuext_viewport_mode_t mode);

NU_API nu_input_t nu_input_create(void);
NU_API nu_bool_t  nu_input_changed(nu_input_t input);
NU_API nu_bool_t  nu_input_pressed(nu_input_t input);
NU_API nu_bool_t  nu_input_just_pressed(nu_input_t input);
NU_API nu_bool_t  nu_input_released(nu_input_t input);
NU_API nu_bool_t  nu_input_just_released(nu_input_t input);
NU_API nu_f32_t   nu_input_value(nu_input_t input);

NU_API nu_error_t nuext_input_bind_button(nu_input_t     input,
                                          nuext_button_t button);
NU_API nu_error_t nuext_input_bind_button_value(nu_input_t     input,
                                                nuext_button_t button,
                                                nu_f32_t       value);
NU_API nu_error_t nuext_input_bind_axis(nu_input_t input, nuext_axis_t axis);

#endif

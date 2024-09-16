#ifndef NU_PLATFORM_H
#define NU_PLATFORM_H

#include <nucleus/platform/api.h>
#include <nucleus/external/glad/gl.h>
#define RGFW_IMPLEMENTATION
#include <nucleus/external/rgfw/RGFW.h>

#define NU__DEFAULT_WINDOW_WIDTH   1280
#define NU__DEFAULT_WINDOW_HEIGHT  720
#define NU__MAX_KEY_COUNT          final_key
#define NU__MAX_MOUSE_BUTTON_COUNT 8
#define NU__FULLSCREEN_KEY         RGFW_F11
#define NU__ID_NONE                0xFFFFFFFF

#define NU__BUTTON_IS_KEY(b)   b >= NUEXT_BUTTON_ESCAPE
#define NU__BUTTON_IS_MOUSE(b) b < NUEXT_BUTTON_ESCAPE
#define NU__BUTTON_TO_KEY(b)   nu__button_to_key[b - NUEXT_BUTTON_ESCAPE]
#define NU__BUTTON_TO_MOUSE(b) nu__button_to_mouse[b]

typedef struct
{
    nu_u32_t width;
    nu_u32_t height;
} nu__platform_config_t;

typedef struct
{
    nuext_viewport_mode_t mode;
    float                 scale_factor;
    nu_uvec2_t            screen;
    nu_rect_t             extent;
    nu_rect_t             viewport;
} nu__viewport_t;

typedef struct
{
    float pressed;
} nu__binding_button_t;

typedef struct
{
    float scale;
} nu__binding_axis_t;

typedef struct
{
    nu_u32_t input_index;
    nu_u32_t next;
    union
    {
        nu__binding_button_t button;
        nu__binding_axis_t   axis;
    };
} nu__binding_t;

typedef NU_POOL(nu__binding_t) nu__binding_pool_t;

typedef struct
{
    float value;
    float previous;
} nu__input_state_t;

typedef struct
{
    nu__input_state_t state;
    nu_bool_t         used;
} nu__input_entry_t;

typedef NU_POOL(nu__input_entry_t) nu__input_pool_t;

typedef struct
{
    nu_bool_t close_requested;

    nu__input_pool_t   entries;
    nu__binding_pool_t bindings;
    nu_u32_t           key_to_first_binding[NU__MAX_KEY_COUNT];
    nu_u32_t  mouse_button_to_first_binding[NU__MAX_MOUSE_BUTTON_COUNT];
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
    nu_vec2_t mouse_motion_previous;

    RGFW_window   *win;
    nu__viewport_t viewport;
    nu_bool_t      fullscreen;
    nu_bool_t      switch_fullscreen;
    nu_bool_t      capture_mouse;
    nu_uvec2_t     previous_size;
    nu_uvec2_t     previous_position;
    nu_timer_t     last_mouse_click;
    nu_bool_t      switch_capture_mouse;
    nu_uvec2_t     size;
} nu__platform_t;

static nu_error_t nu__platform_init(void);
static nu_error_t nu__platform_free(void);

#endif

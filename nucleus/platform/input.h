#ifndef NU_INPUT_H
#define NU_INPUT_H

#include <nucleus/platform/api.h>

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
    nu_u32_t input_index;
    nu_u32_t next;
    union
    {
        nuglfw__binding_button_t button;
        nuglfw__binding_axis_t   axis;
    };
} nuglfw__binding_t;

typedef nu_pool(nuglfw__binding_t) nuglfw__binding_pool_t;

typedef struct
{
    nuglfw__binding_pool_t bindings;
    nu_u32_t               key_to_first_binding[NUGLFW_MAX_KEY_COUNT];
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

static nu_error_t nuglfw__init(void);
static nu_error_t nuglfw__free(void);
static nu_error_t nuglfw__poll_events(void);
static nu_error_t nuglfw__swap_buffers(void);
static nu_error_t nuglfw__bind_button(nu_input_t input, nuext_button_t button);
static nu_error_t nuglfw__bind_button_value(nu_input_t     input,
                                            nuext_button_t button,
                                            float          pressed_value);
static nu_error_t nuglfw__bind_axis(nu_input_t input, nuext_axis_t axis);

#endif

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

typedef nu_pool(nu__input_entry_t) nu__input_pool_t;

typedef struct
{
#ifdef NU_BUILD_GLFW
    nuglfw__input_t glfw;
#endif
    nu__input_pool_t entries;
} nu__input_t;

#endif

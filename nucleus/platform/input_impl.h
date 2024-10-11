#ifndef NU_INPUT_IMPL_H
#define NU_INPUT_IMPL_H

#include <nucleus/internal.h>

static int nu__button_to_mouse[] = { RGFW_mouseLeft,
                                     RGFW_mouseMiddle,
                                     RGFW_mouseRight,
                                     RGFW_mouseScrollUp,
                                     RGFW_mouseScrollDown };

static int nu__button_to_key[] = {
    RGFW_Escape,
    RGFW_F1,
    RGFW_F2,
    RGFW_F3,
    RGFW_F4,
    RGFW_F5,
    RGFW_F6,
    RGFW_F7,
    RGFW_F8,
    RGFW_F9,
    RGFW_F10,
    RGFW_F11,
    RGFW_F12,

    RGFW_Backtick,

    RGFW_0,
    RGFW_1,
    RGFW_2,
    RGFW_3,
    RGFW_4,
    RGFW_5,
    RGFW_6,
    RGFW_7,
    RGFW_8,
    RGFW_9,

    RGFW_Minus,
    RGFW_Equals,
    RGFW_BackSpace,
    RGFW_Tab,
    RGFW_CapsLock,
    RGFW_ShiftL,
    RGFW_ControlL,
    RGFW_AltL,
    RGFW_SuperL,
    RGFW_ShiftR,
    RGFW_ControlR,
    RGFW_AltR,
    RGFW_SuperR,
    RGFW_Space,

    RGFW_a,
    RGFW_b,
    RGFW_c,
    RGFW_d,
    RGFW_e,
    RGFW_f,
    RGFW_g,
    RGFW_h,
    RGFW_i,
    RGFW_j,
    RGFW_k,
    RGFW_l,
    RGFW_m,
    RGFW_n,
    RGFW_o,
    RGFW_p,
    RGFW_q,
    RGFW_r,
    RGFW_s,
    RGFW_t,
    RGFW_u,
    RGFW_v,
    RGFW_w,
    RGFW_x,
    RGFW_y,
    RGFW_z,

    RGFW_Period,
    RGFW_Comma,
    RGFW_Slash,
    RGFW_Bracket,
    RGFW_CloseBracket,
    RGFW_Semicolon,
    RGFW_Return,
    RGFW_Quote,
    RGFW_BackSlash,

    RGFW_Up,
    RGFW_Down,
    RGFW_Left,
    RGFW_Right,

    RGFW_Delete,
    RGFW_Insert,
    RGFW_End,
    RGFW_Home,
    RGFW_PageUp,
    RGFW_PageDown,

    RGFW_Numlock,
    RGFW_KP_Slash,
    RGFW_Multiply,
    RGFW_KP_Minus,
    RGFW_KP_1,
    RGFW_KP_2,
    RGFW_KP_3,
    RGFW_KP_4,
    RGFW_KP_5,
    RGFW_KP_6,
    RGFW_KP_7,
    RGFW_KP_8,
    RGFW_KP_9,
    RGFW_KP_0,
    RGFW_KP_Period,
    RGFW_KP_Return,
};

static void
nu__dispatch_binding_button (nu_u32_t binding, nu_bool_t pressed)
{
    nu_u32_t current = binding;
    while (current != NU__ID_NONE)
    {
        const nu__binding_t *binding = &_ctx.platform.bindings.data[current];
        NU_ASSERT(binding->input_index < _ctx.platform.entries.capacity);

        float value = NU_INPUT_RELEASED;
        if (pressed)
        {
            value = binding->button.pressed;
        }

        _ctx.platform.entries.data[binding->input_index].state.value = value;
        current = binding->next;
    }
}
static void
nu__dispatch_binding_axis (nu_u32_t binding, float value)
{
    nu_u32_t current = binding;
    while (current != NU__ID_NONE)
    {
        const nu__binding_t *binding = &_ctx.platform.bindings.data[current];
        NU_ASSERT(binding->input_index < _ctx.platform.entries.capacity);
        _ctx.platform.entries.data[binding->input_index].state.value
            = value * binding->axis.scale;
        current = binding->next;
    }
}
static nu_bool_t
nu__find_binding (nu_u32_t binding, nu_input_t input)
{
    nu_u32_t current = binding;
    while (current != NU__ID_NONE)
    {
        if (_ctx.platform.bindings.data[current].input_index
            == NU_HANDLE_INDEX(input))
        {
            return NU_TRUE;
        }
        current = _ctx.platform.bindings.data[current].next;
    }
    return NU_FALSE;
}

static void
nu__viewport_cursor (const nu__viewport_t *v, nu_v2_t pos, nu_v2_t *cursor)
{
    nu_v2_t relpos = nu_b2i_normalize(v->viewport, pos);
    *cursor        = nu_v2_mul(*cursor, nu_v2_v2u(nu_b2i_size(v->viewport)));
}

static void
nu__key_callback (RGFW_window *window,
                  u32          keycode,
                  char         key_name[16],
                  u8           lock_state,
                  u8           pressed)
{
    if (keycode == NU__FULLSCREEN_KEY && !pressed)
    {
        _ctx.platform.switch_fullscreen = NU_TRUE;
    }
    else if (keycode == RGFW_Escape && _ctx.platform.capture_mouse)
    {
        _ctx.platform.switch_capture_mouse = NU_TRUE;
    }
    nu__dispatch_binding_button(_ctx.platform.key_to_first_binding[keycode],
                                pressed);
}
static void
nu__mouse_button_callback (RGFW_window *window,
                           u8           button,
                           double       scroll,
                           u8           pressed)
{
    if (button < RGFW_mouseScrollUp)
    {
        if (!_ctx.platform.capture_mouse)
        {
            if (button == RGFW_mouseLeft && pressed)
            {
                if (nu_timer_elapsed(&_ctx.platform.last_mouse_click) < 500.0)
                {
                    _ctx.platform.switch_capture_mouse = NU_TRUE;
                }
                nu_timer_reset(&_ctx.platform.last_mouse_click);
            }
        }
        else
        {
            nu__dispatch_binding_button(
                _ctx.platform.mouse_button_to_first_binding[button], pressed);
        }
    }
    else if (_ctx.platform.capture_mouse)
    {
        _ctx.platform.mouse_scroll = nu_v2((float)scroll, (float)scroll);
    }
}
static void
nu__mouse_position_callback (RGFW_window *window, RGFW_point point)
{
    if (_ctx.platform.capture_mouse)
    {
        _ctx.platform.mouse_motion
            = nu_v2_add(_ctx.platform.mouse_motion,
                        nu_v2_divs(nu_v2((float)point.x, (float)point.y), 200));
    }
    else
    {
        _ctx.platform.mouse_position = nu_v2((float)point.x, (float)point.y);
    }
}

static nu_u32_t *
nu__first_binding_from_button (nuext_button_t button)
{
    if (NU__BUTTON_IS_KEY(button))
    {
        nu_u32_t key = NU__BUTTON_TO_KEY(button);
        return &_ctx.platform.key_to_first_binding[key];
    }
    else if (NU__BUTTON_IS_MOUSE(button))
    {
        nu_u32_t mbutton = NU__BUTTON_TO_MOUSE(button);
        return &_ctx.platform.mouse_button_to_first_binding[mbutton];
    }
    return NU_NULL;
}
static nu_u32_t *
nu__first_binding_from_axis (nuext_axis_t axis)
{
    switch (axis)
    {
        case NUEXT_AXIS_MOUSE_X:
            return &_ctx.platform.mouse_x_first_binding;
        case NUEXT_AXIS_MOUSE_Y:
            return &_ctx.platform.mouse_y_first_binding;
        case NUEXT_AXIS_MOUSE_MOTION_X:
            return &_ctx.platform.mouse_motion_x_first_binding;
        case NUEXT_AXIS_MOUSE_MOTION_Y:
            return &_ctx.platform.mouse_motion_y_first_binding;
    }
    return NU_NULL;
}
static nu__binding_t *
nu__add_binding (nu_u32_t *first_binding, nu_input_t input)
{
    nu_size_t      index;
    nu__binding_t *binding = NU_POOL_ADD(&_ctx.platform.bindings, &index);
    NU_ASSERT(NU_HANDLE_INDEX(input) < _ctx.platform.entries.capacity);
    binding->input_index = NU_HANDLE_INDEX(input);
    binding->next        = *first_binding;
    *first_binding       = index;
    return binding;
}

nu_input_t
nu_input_create (void)
{
    nu_size_t          index;
    nu__input_entry_t *entry = NU_POOL_ADD(&_ctx.platform.entries, &index);
    entry->state.value       = NU_INPUT_RELEASED;
    entry->state.previous    = NU_INPUT_RELEASED;
    entry->used              = NU_TRUE;
    return NU_HANDLE_MAKE(nu_input_t, index);
}
nu_bool_t
nu_input_changed (nu_input_t input)
{
    nu_size_t          index = NU_HANDLE_INDEX(input);
    nu__input_state_t *state = &_ctx.platform.entries.data[index].state;
    return state->value != state->previous;
}
nu_bool_t
nu_input_pressed (nu_input_t input)
{
    nu_size_t          index = NU_HANDLE_INDEX(input);
    nu__input_state_t *state = &_ctx.platform.entries.data[index].state;
    return NU_INPUT_IS_PRESSED(state->value);
}
nu_bool_t
nu_input_just_pressed (nu_input_t input)
{
    nu_size_t          index = NU_HANDLE_INDEX(input);
    nu__input_state_t *state = &_ctx.platform.entries.data[index].state;
    return NU_INPUT_IS_PRESSED(state->value)
           && !NU_INPUT_IS_PRESSED(state->previous);
}
nu_bool_t
nu_input_released (nu_input_t input)
{
    nu_size_t          index = NU_HANDLE_INDEX(input);
    nu__input_state_t *state = &_ctx.platform.entries.data[index].state;
    return !NU_INPUT_IS_PRESSED(state->value);
}
nu_bool_t
nu_input_just_released (nu_input_t input)
{
    nu_size_t          index = NU_HANDLE_INDEX(input);
    nu__input_state_t *state = &_ctx.platform.entries.data[index].state;
    return !NU_INPUT_IS_PRESSED(state->value)
           && NU_INPUT_IS_PRESSED(state->previous);
}
float
nu_input_value (nu_input_t input)
{
    nu_size_t          index = NU_HANDLE_INDEX(input);
    nu__input_state_t *state = &_ctx.platform.entries.data[index].state;
    return state->value;
}

nu_error_t
nuext_input_bind_button (nu_input_t input, nuext_button_t button)
{
    return nuext_input_bind_button_value(input, button, NU_INPUT_PRESSED);
}
nu_error_t
nuext_input_bind_button_value (nu_input_t     input,
                               nuext_button_t button,
                               float          pressed_value)
{
    // Check duplicated binding
    nu_u32_t *first_binding = nu__first_binding_from_button(button);
    NU_ASSERT(first_binding);
    if (nu__find_binding(*first_binding, input))
    {
        return NU_ERROR_DUPLICATED;
    }

    // Add binding
    nu__binding_t *binding  = nu__add_binding(first_binding, input);
    binding->button.pressed = pressed_value;

    return NU_ERROR_NONE;
}
nu_error_t
nuext_input_bind_axis (nu_input_t input, nuext_axis_t axis)
{
    // Check duplicated
    nu_u32_t *first_binding = nu__first_binding_from_axis(axis);
    NU_ASSERT(first_binding);
    if (nu__find_binding(*first_binding, input))
    {
        return NU_ERROR_DUPLICATED;
    }

    // Add binding
    nu__binding_t *binding = nu__add_binding(first_binding, input);
    binding->axis.scale    = 1.0;

    return NU_ERROR_NONE;
}

nu_v2i_t
nuext_platform_cursor (nu_input_t cursor_x, nu_input_t cursor_y)
{
    float cx = nu_input_value(cursor_x);
    float cy = nu_input_value(cursor_y);
    return nu_v2i((nu_i32_t)(cx * (float)_ctx.platform.size.x),
                  (nu_i32_t)(cy * (float)_ctx.platform.size.y));
}

#endif

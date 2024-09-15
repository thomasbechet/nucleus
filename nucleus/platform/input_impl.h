#ifndef NU_INPUT_IMPL_H
#define NU_INPUT_IMPL_H

#include <nucleus/internal.h>

nu_input_t
nu_input_create (void)
{
    nu_size_t          index;
    nu__input_entry_t *entry
        = NU_POOL_ADD(&_ctx.platform.input.entries, &index);
    entry->state.value    = NU_INPUT_RELEASED;
    entry->state.previous = NU_INPUT_RELEASED;
    entry->used           = NU_TRUE;
    return nu_handle_make(nu_input_t, index);
}
nu_bool_t
nu_input_changed (nu_input_t input)
{
    nu_size_t          index = nu_handle_index(input);
    nu__input_state_t *state = &_ctx.platform.input.entries.data[index].state;
    return state->value != state->previous;
}
nu_bool_t
nu_input_pressed (nu_input_t input)
{
    nu_size_t          index = nu_handle_index(input);
    nu__input_state_t *state = &_ctx.platform.input.entries.data[index].state;
    return NU_INPUT_IS_PRESSED(state->value);
}
nu_bool_t
nu_input_just_pressed (nu_input_t input)
{
    nu_size_t          index = nu_handle_index(input);
    nu__input_state_t *state = &_ctx.platform.input.entries.data[index].state;
    return NU_INPUT_IS_PRESSED(state->value)
           && !NU_INPUT_IS_PRESSED(state->previous);
}
nu_bool_t
nu_input_released (nu_input_t input)
{
    nu_size_t          index = nu_handle_index(input);
    nu__input_state_t *state = &_ctx.platform.input.entries.data[index].state;
    return !NU_INPUT_IS_PRESSED(state->value);
}
nu_bool_t
nu_input_just_released (nu_input_t input)
{
    nu_size_t          index = nu_handle_index(input);
    nu__input_state_t *state = &_ctx.platform.input.entries.data[index].state;
    return !NU_INPUT_IS_PRESSED(state->value)
           && NU_INPUT_IS_PRESSED(state->previous);
}
float
nu_input_value (nu_input_t input)
{
    nu_size_t          index = nu_handle_index(input);
    nu__input_state_t *state = &_ctx.platform.input.entries.data[index].state;
    return state->value;
}

nu_error_t
nuext_input_bind_button (nu_input_t input, nuext_button_t button)
{
#ifdef NU_BUILD_GLFW
    return nuglfw__bind_button(input, button);
#endif
    return NU_ERROR_NONE;
}
nu_error_t
nuext_input_bind_button_value (nu_input_t     input,
                               nuext_button_t button,
                               float          value)
{
#ifdef NU_BUILD_GLFW
    return nuglfw__bind_button_value(input, button, value);
#endif
    return NU_ERROR_NONE;
}
nu_error_t
nuext_input_bind_axis (nu_input_t input, nuext_axis_t axis)
{
#ifdef NU_BUILD_GLFW
    return nuglfw__bind_axis(input, axis);
#endif
    return NU_ERROR_NONE;
}

nu_ivec2_t
nuext_platform_cursor (nu_input_t cursor_x, nu_input_t cursor_y)
{
    float cx = nu_input_value(cursor_x);
    float cy = nu_input_value(cursor_y);
    return nu_ivec2((nu_i32_t)(cx * (float)_ctx.platform.surface.size.x),
                    (nu_i32_t)(cy * (float)_ctx.platform.surface.size.y));
}

#endif

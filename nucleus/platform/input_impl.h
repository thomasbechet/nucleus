#ifndef NU_INPUT_IMPL_H
#define NU_INPUT_IMPL_H

#include <nucleus/platform/input.h>

#ifdef NU_BUILD_GLFW
#include <nucleus/platform/glfw_impl.h>
#endif

nu_error_t
nu_input_create (nu_platform_t *platform, nu_input_handle_t *input)
{
#ifdef NU_BUILD_GLFW
    return nuglfw__create_input(&platform->_input.glfw, input);
#endif
    return NU_ERROR_NONE;
}
nu_bool_t
nu_input_changed (const nu_platform_t *platform, nu_input_handle_t input)
{
    const nu__input_state_t *state
        = nuglfw__input_state(&platform->_input.glfw, input);
    return state->value != state->previous;
}
nu_bool_t
nu_input_pressed (const nu_platform_t *platform, nu_input_handle_t input)
{
    const nu__input_state_t *state
        = nuglfw__input_state(&platform->_input.glfw, input);
    return NU_INPUT_IS_PRESSED(state->value);
}
nu_bool_t
nu_input_just_pressed (const nu_platform_t *platform, nu_input_handle_t input)
{
    const nu__input_state_t *state
        = nuglfw__input_state(&platform->_input.glfw, input);
    return NU_INPUT_IS_PRESSED(state->value)
           && !NU_INPUT_IS_PRESSED(state->previous);
}
nu_bool_t
nu_input_released (const nu_platform_t *platform, nu_input_handle_t input)
{
    const nu__input_state_t *state
        = nuglfw__input_state(&platform->_input.glfw, input);
    return !NU_INPUT_IS_PRESSED(state->value);
}
nu_bool_t
nu_input_just_released (const nu_platform_t *platform, nu_input_handle_t input)
{
    const nu__input_state_t *state
        = nuglfw__input_state(&platform->_input.glfw, input);
    return !NU_INPUT_IS_PRESSED(state->value)
           && NU_INPUT_IS_PRESSED(state->previous);
}
float
nu_input_value (const nu_platform_t *platform, nu_input_handle_t input)
{
    const nu__input_state_t *state
        = nuglfw__input_state(&platform->_input.glfw, input);
    return state->value;
}

nu_vec3_t
nu_input_axis3d (const nu_platform_t *platform,
                 nu_input_handle_t    pos_x,
                 nu_input_handle_t    neg_x,
                 nu_input_handle_t    pos_y,
                 nu_input_handle_t    neg_y,
                 nu_input_handle_t    pos_z,
                 nu_input_handle_t    neg_z,
                 nu_bool_t            normalize)
{
    nu_vec3_t ax;

    ax.x = NU_CLAMP(nu_input_value(platform, neg_x)
                        - nu_input_value(platform, pos_x),
                    -1,
                    1);
    ax.y = NU_CLAMP(nu_input_value(platform, pos_y)
                        - nu_input_value(platform, neg_y),
                    -1,
                    1);
    ax.z = NU_CLAMP(nu_input_value(platform, pos_z)
                        - nu_input_value(platform, neg_z),
                    -1,
                    1);
    if (normalize)
    {
        ax = nu_vec3_normalize(ax);
    }

    return ax;
}

nu_error_t
nuext_input_bind_button (nu_platform_t    *platform,
                         nu_input_handle_t input,
                         nuext_button_t    button)
{
#ifdef NU_BUILD_GLFW
    return nuglfw__bind_button(&platform->_input.glfw, input, button);
#endif
    return NU_ERROR_NONE;
}
nu_error_t
nuext_input_bind_button_value (nu_platform_t    *platform,
                               nu_input_handle_t input,
                               nuext_button_t    button,
                               float             value)
{
#ifdef NU_BUILD_GLFW
    return nuglfw__bind_button_value(
        &platform->_input.glfw, input, button, value);
#endif
    return NU_ERROR_NONE;
}
nu_error_t
nuext_input_bind_axis (nu_platform_t    *platform,
                       nu_input_handle_t input,
                       nuext_axis_t      axis)
{
#ifdef NU_BUILD_GLFW
    return nuglfw__bind_axis(&platform->_input.glfw, input, axis);
#endif
    return NU_ERROR_NONE;
}

#endif

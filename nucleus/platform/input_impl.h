#ifndef NU_INPUT_IMPL_H
#define NU_INPUT_IMPL_H

#include <nucleus/platform/input.h>

#ifdef NU_BUILD_GLFW
#include <nucleus/platform/glfw_impl.h>
#endif

nu_error_t
nu_input_create (nu_platform_t platform, nu_input_t *input)
{
    input->ptr        = nu_alloc(platform.ptr->_allocator, sizeof(*input->ptr));
    input->ptr->value = NU_INPUT_RELEASED;
    input->ptr->previous = NU_INPUT_RELEASED;
    return NU_ERROR_NONE;
}
nu_bool_t
nu_input_changed (nu_input_t input)
{
    return input.ptr->value != input.ptr->previous;
}
nu_bool_t
nu_input_pressed (nu_input_t input)
{
    return NU_INPUT_IS_PRESSED(input.ptr->value);
}
nu_bool_t
nu_input_just_pressed (nu_input_t input)
{
    return NU_INPUT_IS_PRESSED(input.ptr->value)
           && !NU_INPUT_IS_PRESSED(input.ptr->previous);
}
nu_bool_t
nu_input_released (nu_input_t input)
{
    return !NU_INPUT_IS_PRESSED(input.ptr->value);
}
nu_bool_t
nu_input_just_released (nu_input_t input)
{
    return !NU_INPUT_IS_PRESSED(input.ptr->value)
           && NU_INPUT_IS_PRESSED(input.ptr->previous);
}
float
nu_input_value (nu_input_t input)
{
    return input.ptr->value;
}

nu_vec3_t
nu_input_axis3d (nu_input_t pos_x,
                 nu_input_t neg_x,
                 nu_input_t pos_y,
                 nu_input_t neg_y,
                 nu_input_t pos_z,
                 nu_input_t neg_z,
                 nu_bool_t  normalize)
{
    nu_vec3_t ax;

    ax.x = NU_CLAMP(nu_input_value(neg_x) - nu_input_value(pos_x), -1, 1);
    ax.y = NU_CLAMP(nu_input_value(pos_y) - nu_input_value(neg_y), -1, 1);
    ax.z = NU_CLAMP(nu_input_value(pos_z) - nu_input_value(neg_z), -1, 1);
    if (normalize)
    {
        ax = nu_vec3_normalize(ax);
    }

    return ax;
}
nu_ivec2_t
nuext_platform_cursor (nu_platform_t platform,
                       nu_input_t    cursor_x,
                       nu_input_t    cursor_y)
{
    float cx = nu_input_value(cursor_x);
    float cy = nu_input_value(cursor_y);
    return nu_ivec2((nu_i32_t)(cx * (float)platform.ptr->_surface.size.x),
                    (nu_i32_t)(cy * (float)platform.ptr->_surface.size.y));
}

nu_error_t
nuext_input_bind_button (nu_platform_t  platform,
                         nu_input_t     input,
                         nuext_button_t button)
{
#ifdef NU_BUILD_GLFW
    return nuglfw__bind_button(&platform.ptr->_input.glfw, input, button);
#endif
    return NU_ERROR_NONE;
}
nu_error_t
nuext_input_bind_button_value (nu_platform_t  platform,
                               nu_input_t     input,
                               nuext_button_t button,
                               float          value)
{
#ifdef NU_BUILD_GLFW
    return nuglfw__bind_button_value(
        &platform.ptr->_input.glfw, input, button, value);
#endif
    return NU_ERROR_NONE;
}
nu_error_t
nuext_input_bind_axis (nu_platform_t platform,
                       nu_input_t    input,
                       nuext_axis_t  axis)
{
#ifdef NU_BUILD_GLFW
    return nuglfw__bind_axis(&platform.ptr->_input.glfw, input, axis);
#endif
    return NU_ERROR_NONE;
}

#endif

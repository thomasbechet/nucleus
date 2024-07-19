#ifndef NU_INPUT_H
#define NU_INPUT_H

#include <nucleus/types.h>

NU_API void       nu_init_input(nu_input_t *input);
NU_API nu_error_t nu_update_input(nu_context_t *ctx, nu_input_t *input);
NU_API nu_error_t nu_update_inputs(nu_context_t *ctx,
                                   nu_input_t  **inputs,
                                   nu_size_t     count);
NU_API nu_bool_t  nu_input_changed(const nu_input_t *input);
NU_API nu_bool_t  nu_input_pressed(const nu_input_t *input);
NU_API nu_bool_t  nu_input_just_pressed(const nu_input_t *input);
NU_API nu_bool_t  nu_input_released(const nu_input_t *input);
NU_API nu_bool_t  nu_input_just_released(const nu_input_t *input);
NU_API float      nu_input_value(const nu_input_t *input);

#ifdef NU_IMPLEMENTATION

void
nu_init_input (nu_input_t *input)
{
    input->_value    = NU_INPUT_RELEASED;
    input->_previous = NU_INPUT_RELEASED;
}
nu_error_t
nu_update_input (nu_context_t *ctx, nu_input_t *input)
{
    NU_UNUSED(ctx);
    input->_previous = input->_value;
    return NU_ERROR_NONE;
}
nu_bool_t
nu_input_changed (const nu_input_t *input)
{
    return input->_value != input->_previous;
}
nu_error_t
nu_update_inputs (nu_context_t *ctx, nu_input_t **inputs, nu_size_t count)
{
    for (nu_size_t i = 0; i < count; ++i)
    {
        nu_error_t error = nu_update_input(ctx, inputs[i]);
        NU_ERROR_CHECK(error, return error);
    }
    return NU_ERROR_NONE;
}
nu_bool_t
nu_input_pressed (const nu_input_t *input)
{
    return NU_INPUT_IS_PRESSED(input->_value);
}
nu_bool_t
nu_input_just_pressed (const nu_input_t *input)
{
    return NU_INPUT_IS_PRESSED(input->_value)
           && !NU_INPUT_IS_PRESSED(input->_previous);
}
nu_bool_t
nu_input_released (const nu_input_t *input)
{
    return !NU_INPUT_IS_PRESSED(input->_value);
}
nu_bool_t
nu_input_just_released (const nu_input_t *input)
{
    return !NU_INPUT_IS_PRESSED(input->_value)
           && NU_INPUT_IS_PRESSED(input->_previous);
}
float
nu_input_value (const nu_input_t *input)
{
    return input->_value;
}

#endif

#endif

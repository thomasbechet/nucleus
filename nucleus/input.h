#ifndef NU_INPUT_H
#define NU_INPUT_H

#include <nucleus/types.h>

NU_API nu_error_t nu_update_input(nu_context_t *ctx, nu_input_t *input);
NU_API nu_error_t nu_update_inputs(nu_context_t *ctx,
                                   nu_input_t  **inputs,
                                   nu_size_t     count);
NU_API nu_bool_t  nu_input_changed(const nu_input_t *input);

#ifdef NU_IMPLEMENTATION

#include <nucleus/error.h>

nu_error_t
nu_update_input (nu_context_t *ctx, nu_input_t *input)
{
    NU_UNUSED(ctx);
    input->prev = input->value;
    return NU_ERROR_NONE;
}
nu_bool_t
nu_input_changed (const nu_input_t *input)
{
    return input->value != input->prev;
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

#endif

#endif

#ifndef NU_INPUT_H
#define NU_INPUT_H

#include <nucleus/types.h>

NU_API nu_error_t nu_input_create(nu_context_t *ctx, nu_input_t *input);
NU_API nu_bool_t  nu_input_changed(const nu_context_t *ctx,
                                   const nu_input_t   *input);
NU_API nu_bool_t  nu_input_pressed(const nu_context_t *ctx,
                                   const nu_input_t   *input);
NU_API nu_bool_t  nu_input_just_pressed(const nu_context_t *ctx,
                                        const nu_input_t   *input);
NU_API nu_bool_t  nu_input_released(const nu_context_t *ctx,
                                    const nu_input_t   *input);
NU_API nu_bool_t  nu_input_just_released(const nu_context_t *ctx,
                                         const nu_input_t   *input);
NU_API float nu_input_value(const nu_context_t *ctx, const nu_input_t *input);

NU_API nu_error_t nuext_input_bind_button(nu_context_t  *ctx,
                                          nu_input_t    *input,
                                          nuext_button_t button);
NU_API nu_error_t nuext_input_bind_button_value(nu_context_t  *ctx,
                                                nu_input_t    *input,
                                                nuext_button_t button,
                                                float          value);
NU_API nu_error_t nuext_input_bind_axis(nu_context_t *ctx,
                                        nu_input_t   *input,
                                        nuext_axis_t  axis);

#ifdef NU_IMPLEMENTATION

#ifdef NU_BUILD_GLFW
#include <nucleus/backend/glfw.h>
#endif

nu_error_t
nu_input_create (nu_context_t *ctx, nu_input_t *input)
{
#ifdef NU_BUILD_GLFW
    return nuglfw__create_input(&ctx->_input.glfw, input);
#endif
    return NU_ERROR_NONE;
}
nu_bool_t
nu_input_changed (const nu_context_t *ctx, const nu_input_t *input)
{
    const nu__input_state_t *state
        = nuglfw__input_state(&ctx->_input.glfw, input);
    return state->value != state->previous;
}
nu_bool_t
nu_input_pressed (const nu_context_t *ctx, const nu_input_t *input)
{
    const nu__input_state_t *state
        = nuglfw__input_state(&ctx->_input.glfw, input);
    return NU_INPUT_IS_PRESSED(state->value);
}
nu_bool_t
nu_input_just_pressed (const nu_context_t *ctx, const nu_input_t *input)
{
    const nu__input_state_t *state
        = nuglfw__input_state(&ctx->_input.glfw, input);
    return NU_INPUT_IS_PRESSED(state->value)
           && !NU_INPUT_IS_PRESSED(state->previous);
}
nu_bool_t
nu_input_released (const nu_context_t *ctx, const nu_input_t *input)
{
    const nu__input_state_t *state
        = nuglfw__input_state(&ctx->_input.glfw, input);
    return !NU_INPUT_IS_PRESSED(state->value);
}
nu_bool_t
nu_input_just_released (const nu_context_t *ctx, const nu_input_t *input)
{
    const nu__input_state_t *state
        = nuglfw__input_state(&ctx->_input.glfw, input);
    return !NU_INPUT_IS_PRESSED(state->value)
           && NU_INPUT_IS_PRESSED(state->previous);
}
float
nu_input_value (const nu_context_t *ctx, const nu_input_t *input)
{
    const nu__input_state_t *state
        = nuglfw__input_state(&ctx->_input.glfw, input);
    return state->value;
}

nu_error_t
nuext_input_bind_button (nu_context_t  *ctx,
                         nu_input_t    *input,
                         nuext_button_t button)
{
#ifdef NU_BUILD_GLFW
    return nuglfw__bind_button(&ctx->_input.glfw, input, button);
#endif
    return NU_ERROR_NONE;
}
nu_error_t
nuext_input_bind_button_value (nu_context_t  *ctx,
                               nu_input_t    *input,
                               nuext_button_t button,
                               float          value)
{
#ifdef NU_BUILD_GLFW
    return nuglfw__bind_button_value(&ctx->_input.glfw, input, button, value);
#endif
    return NU_ERROR_NONE;
}
nu_error_t
nuext_input_bind_axis (nu_context_t *ctx, nu_input_t *input, nuext_axis_t axis)
{
#ifdef NU_BUILD_GLFW
    return nuglfw__bind_axis(&ctx->_input.glfw, input, axis);
#endif
    return NU_ERROR_NONE;
}

#endif

#endif

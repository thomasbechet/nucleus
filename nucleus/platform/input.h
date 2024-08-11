#ifndef NU_INPUT_H
#define NU_INPUT_H

#include <nucleus/core/math.h>
#include <nucleus/core/logger.h>
#include <nucleus/platform/context.h>

NU_API nu_error_t nu_input_create(nu_platform_t     *platform,
                                  nu_input_handle_t *input);
NU_API nu_bool_t  nu_input_changed(const nu_platform_t *platform,
                                   nu_input_handle_t    input);
NU_API nu_bool_t  nu_input_pressed(const nu_platform_t *platform,
                                   nu_input_handle_t    input);
NU_API nu_bool_t  nu_input_just_pressed(const nu_platform_t *platform,
                                        nu_input_handle_t    input);
NU_API nu_bool_t  nu_input_released(const nu_platform_t *platform,
                                    nu_input_handle_t    input);
NU_API nu_bool_t  nu_input_just_released(const nu_platform_t *platform,
                                         nu_input_handle_t    input);
NU_API float      nu_input_value(const nu_platform_t *platform,
                                 nu_input_handle_t    input);

NU_API nu_error_t nuext_input_bind_button(nu_platform_t    *platform,
                                          nu_input_handle_t input,
                                          nuext_button_t    button);
NU_API nu_error_t nuext_input_bind_button_value(nu_platform_t    *platform,
                                                nu_input_handle_t input,
                                                nuext_button_t    button,
                                                float             value);
NU_API nu_error_t nuext_input_bind_axis(nu_platform_t    *platform,
                                        nu_input_handle_t input,
                                        nuext_axis_t      axis);

#endif

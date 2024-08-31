#ifndef NU_INPUT_H
#define NU_INPUT_H

#include <nucleus/core/math.h>
#include <nucleus/core/logger.h>
#include <nucleus/platform/context.h>

NU_DEFINE_HANDLE_POINTER(nu_input_t, nu__input_t);

NU_API nu_error_t nu_input_create(nu_platform_t platform, nu_input_t *input);
NU_API nu_bool_t  nu_input_changed(nu_input_t input);
NU_API nu_bool_t  nu_input_pressed(nu_input_t input);
NU_API nu_bool_t  nu_input_just_pressed(nu_input_t input);
NU_API nu_bool_t  nu_input_released(nu_input_t input);
NU_API nu_bool_t  nu_input_just_released(nu_input_t input);
NU_API float      nu_input_value(nu_input_t input);

NU_API nu_vec3_t  nu_input_axis3d(nu_input_t pos_x,
                                  nu_input_t neg_x,
                                  nu_input_t pos_y,
                                  nu_input_t neg_y,
                                  nu_input_t pos_z,
                                  nu_input_t neg_z,
                                  nu_bool_t  normalize);
NU_API nu_ivec2_t nu_input_cursor(nu_input_t cursor_x, nu_input_t cursor_y);

NU_API nu_error_t nuext_input_bind_button(nu_platform_t  platform,
                                          nu_input_t     input,
                                          nuext_button_t button);
NU_API nu_error_t nuext_input_bind_button_value(nu_platform_t  platform,
                                                nu_input_t     input,
                                                nuext_button_t button,
                                                float          value);
NU_API nu_error_t nuext_input_bind_axis(nu_platform_t platform,
                                        nu_input_t    input,
                                        nuext_axis_t  axis);

#endif

#ifndef NU_SURFACE_H
#define NU_SURFACE_H

#include <nucleus/platform/context.h>

NU_API nu_error_t nu_swap_buffers(nu_platform_t *platform);
NU_API nu_ivec2_t nu_cursor_position(const nu_platform_t *platform,
                                     nu_input_handle_t    cursor_x,
                                     nu_input_handle_t    cursor_y);

NU_API nu_error_t nuext_set_viewport_mode(nu_platform_t        *platform,
                                          nuext_viewport_mode_t mode);

#endif

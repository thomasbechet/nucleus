#ifndef NU_SURFACE_H
#define NU_SURFACE_H

#include <nucleus/platform/context.h>

NU_API nu_error_t nu_platform_swap_buffers(nu_platform_t *platform);

NU_API nu_error_t nuext_platform_viewport_mode(nu_platform_t        *platform,
                                               nuext_viewport_mode_t mode);

#endif

#ifndef NU_SURFACE_H
#define NU_SURFACE_H

#include <nucleus/core/core.h>

NU_API nu_error_t nu_swap_buffers(void);

typedef enum
{
    NUEXT_VIEWPORT_FIXED,
    NUEXT_VIEWPORT_FIXED_BEST_FIT,
    NUEXT_VIEWPORT_STRETCH_KEEP_ASPECT,
    NUEXT_VIEWPORT_STRETCH,
} nuext_viewport_mode_t;

NU_API nu_error_t nuext_viewport_mode(nuext_viewport_mode_t mode);

#endif

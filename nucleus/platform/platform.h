#ifndef NU_PLATFORM_H
#define NU_PLATFORM_H

#include <nucleus/platform/input.h>
#include <nucleus/platform/surface.h>

typedef struct
{
    nu_u32_t width;
    nu_u32_t height;
} nu_platform_info_t;

NU_API nu_error_t nu_poll_events(void);
NU_API nu_bool_t  nu_exit_requested(void);

#endif

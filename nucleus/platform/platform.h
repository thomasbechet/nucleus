#ifndef NU_PLATFORM_H
#define NU_PLATFORM_H

#include <nucleus/platform/input.h>
#include <nucleus/platform/surface.h>
#ifdef NU_BUILD_GLFW
#define GLFW_INCLUDE_NONE
#include <nucleus/external/glfw-3.4/include/GLFW/glfw3.h>
#endif

typedef struct
{
    nu_u32_t width;
    nu_u32_t height;
} nu__platform_config_t;

typedef struct
{
    nu_bool_t     close_requested;
    nu__input_t   input;
    nu__surface_t surface;
} nu__platform_t;

static nu_error_t nu__platform_init(void);
static nu_error_t nu__platform_free(void);

#endif

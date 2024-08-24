#ifndef NU_TEXTURE_H
#define NU_TEXTURE_H

#include <nucleus/core.h>

#ifdef NU_BUILD_RENDERER_GL
#include <nucleus/gl/handle.h>
#endif

typedef enum
{
    NU_TEXTURE_FORMAT_COLOR,
    NU_TEXTURE_FORMAT_DEPTH
} nu_texture_format_t;

typedef enum
{
    NU_TEXTURE_USAGE_TARGET,
    NU_TEXTURE_USAGE_SAMPLE
} nu_texture_usage_t;

typedef enum
{
    NU_TEXTURE_WRAP_CLAMP,
    NU_TEXTURE_WRAP_REPEAT,
    NU_TEXTURE_WRAP_MIRROR
} nu_texture_wrap_mode_t;

typedef struct
{
    nu_uvec2_t          size;
    nu_texture_format_t format;
    nu_texture_usage_t  usage;
    const nu_color_t   *colors;
} nu_texture_info_t;

typedef struct
{
#ifdef NU_BUILD_RENDERER_GL
    nugl__handle_t _gl;
#endif
} nu_texture_handle_t;

#endif

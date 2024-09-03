#ifndef NU_INTERNAL_H
#define NU_INTERNAL_H

#include <nucleus/core/internal.h>
#ifdef NU_BUILD_PLATFORM
#include <nucleus/platform/internal.h>
#endif
#ifdef NU_BUILD_GRAPHICS
#include <nucleus/graphics/internal.h>
#endif
#ifdef NU_BUILD_GL
#include <nucleus/gl/internal.h>
#endif
#ifdef NU_BUILD_UTILS
#include <nucleus/utils/internal.h>
#endif

static struct nu__context
{
    nu__core_t core;
#ifdef NU_BUILD_PLATFORM
    nu__platform_t platform;
#endif
#ifdef NU_BUILD_GRAPHICS
    nu__graphics_t graphics;
#endif
#ifdef NU_BUILD_GL
    nu__gl_t gl;
#endif
#ifdef NU_BUILD_UTILS
    nu__utils_t utils;
#endif
} _ctx;

#endif

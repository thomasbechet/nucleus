#ifndef NU_INTERNAL_H
#define NU_INTERNAL_H

#include <nucleus/core/internal.h>
#ifdef NU_BUILD_PLATFORM
#include <nucleus/platform/internal.h>
#endif
// #ifdef NU_BUILD_GRAPHICS
// #include <nucleus/graphics/internal.h>
// #endif

static struct nu__context
{
    nu__core_t core;
#ifdef NU_BUILD_PLATFORM
    nu__platform_t platform;
#endif
} _ctx;

#endif

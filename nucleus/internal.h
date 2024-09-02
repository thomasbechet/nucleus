#ifndef NU_INTERNAL_H
#define NU_INTERNAL_H

#include <nucleus/core/internal.h>
#ifdef NU_BUILD_PLATFORM
#include <nucleus/platform/internal.h>
#endif

static struct nu__context
{
    nu_allocator_t allocator;
    nu__logger_t   logger;
#ifdef NU_BUILD_PLATFORM
    nu__input_t    input;
    nu__surface_t  surface;
    nu__platform_t platform;
#endif
} _ctx;

#endif

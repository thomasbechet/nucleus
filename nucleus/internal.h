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
#ifdef NU_BUILD_IMPORTER
#include <nucleus/importer/internal.h>
#endif
#ifdef NU_BUILD_ASSET
#include <nucleus/asset/internal.h>
#endif

typedef struct
{
#ifdef NU_BUILD_PLATFORM
    nu__platform_config_t platform;
#endif
#ifdef NU_BUILD_GRAPHICS
    nu__graphics_config_t graphics;
#endif
    nu_bool_t initialized; // False by default
} nu__config_t;

static nu__config_t *nu__config(void);

static struct nu__context
{
    nu__config_t config;
    nu__core_t   core;
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
#ifdef NU_BUILD_IMPORTER
    nu__importer_t importer;
#endif
#ifdef NU_BUILD_ASSET
    nu__asset_t asset;
#endif
} _ctx;

#endif

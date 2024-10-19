#ifndef NU_INTERNAL_H
#define NU_INTERNAL_H

#include <nucleus/core/core.h>
#ifdef NU_BUILD_PLATFORM
#include <nucleus/platform/platform.h>
#endif
#ifdef NU_BUILD_GRAPHICS
#include <nucleus/graphics/graphics.h>
#endif
#ifdef NU_BUILD_GL
#include <nucleus/gl/gl.h>
#endif
#ifdef NU_BUILD_UTILS
#include <nucleus/utils/utils.h>
#endif
#ifdef NU_BUILD_ASSET
#include <nucleus/asset/asset.h>
#endif
#ifdef NU_BUILD_IMPORTER
#include <nucleus/importer/importer.h>
#endif
#ifdef NU_BUILD_UI
#include <nucleus/ui/ui.h>
#endif
#ifdef NU_BUILD_PHYSICS
#include <nucleus/physics/physics.h>
#endif
#ifdef NU_BUILD_ECS
#include <nucleus/ecs/ecs.h>
#endif
#ifdef NU_BUILD_SERIA
#include <nucleus/seria/seria.h>
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
#ifdef NU_BUILD_ASSET
    nu__asset_t asset;
#endif
#ifdef NU_BUILD_IMPORTER
    nu__importer_t importer;
#endif
#ifdef NU_BUILD_UI
    nu__ui_t ui;
#endif
#ifdef NU_BUILD_PHYSICS
    nu__physics_t physics;
#endif
#ifdef NU_BUILD_ECS
    nu__ecs_t ecs;
#endif
#ifdef NU_BUILD_SERIA
    nu__seria_t seria;
#endif
} _ctx;

#endif

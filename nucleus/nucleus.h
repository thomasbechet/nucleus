#ifndef NU_NUCLEUS_H
#define NU_NUCLEUS_H

#include <nucleus/core/api.h>
#ifdef NU_BUILD_PLATFORM
#include <nucleus/platform/api.h>
#endif
#ifdef NU_BUILD_GRAPHICS
#include <nucleus/graphics/api.h>
#endif
#ifdef NU_BUILD_UTILS
#include <nucleus/utils/utils.h>
#endif
#ifdef NU_BUILD_RESOURCE
#include <nucleus/resource/api.h>
#endif
#ifdef NU_BUILD_IMPORTER
#include <nucleus/importer/api.h>
#endif
#ifdef NU_BUILD_UI
#include <nucleus/ui/api.h>
#endif
#ifdef NU_BUILD_PHYSICS
#include <nucleus/physics/api.h>
#endif
#ifdef NU_BUILD_ECS
#include <nucleus/ecs/api.h>
#endif
#ifdef NU_BUILD_SERIA
#include <nucleus/seria/api.h>
#endif

#ifdef NU_IMPLEMENTATION

#include <nucleus/core/core_impl.h>
#ifdef NU_BUILD_PLATFORM
#include <nucleus/platform/platform_impl.h>
#endif
#ifdef NU_BUILD_GRAPHICS
#include <nucleus/graphics/graphics_impl.h>
#endif
#ifdef NU_BUILD_UTILS
#include <nucleus/utils/utils_impl.h>
#endif
#ifdef NU_BUILD_RESOURCE
#include <nucleus/resource/resource_impl.h>
#endif
#ifdef NU_BUILD_IMPORTER
#include <nucleus/importer/importer_impl.h>
#endif
#ifdef NU_BUILD_UI
#include <nucleus/ui/ui_impl.h>
#endif
#ifdef NU_BUILD_PHYSICS
#include <nucleus/physics/physics_impl.h>
#endif
#ifdef NU_BUILD_ECS
#include <nucleus/ecs/ecs_impl.h>
#endif
#ifdef NU_BUILD_SERIA
#include <nucleus/seria/seria_impl.h>
#endif

#endif

#endif

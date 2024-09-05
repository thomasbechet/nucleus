#ifndef NU_NUCLEUS_H
#define NU_NUCLEUS_H

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

#ifdef NU_IMPLEMENTATION

#include <nucleus/core/core_impl.h>
#ifdef NU_BUILD_PLATFORM
#include <nucleus/platform/platform_impl.h>
#endif
#ifdef NU_BUILD_GRAPHICS
#include <nucleus/graphics/graphics_impl.h>
#endif
#ifdef NU_BUILD_GL
#include <nucleus/gl/gl_impl.h>
#endif
#ifdef NU_BUILD_UTILS
#include <nucleus/utils/utils_impl.h>
#endif
#ifdef NU_BUILD_ASSET
#include <nucleus/asset/asset_impl.h>
#endif

#endif

#endif

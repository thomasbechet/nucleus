#ifndef NU_NUCLEUS_H
#define NU_NUCLEUS_H

#include <nucleus/core.h>
#ifdef NU_BUILD_PLATFORM
#include <nucleus/platform.h>
#endif
// #ifdef NU_BUILD_GRAPHICS
// #include <nucleus/graphics.h>
// #endif
// #include <nucleus/asset.h>
// #include <nucleus/utils.h>

#ifdef NU_IMPLEMENTATION

#include <nucleus/core/color_impl.h>
#include <nucleus/core/logger_impl.h>
#include <nucleus/core/math_impl.h>
#include <nucleus/core/memory_impl.h>
#include <nucleus/core/time_impl.h>
#include <nucleus/core/string_impl.h>
#include <nucleus/core/ds_impl.h>
#include <nucleus/core/context_impl.h>

#ifdef NU_BUILD_PLATFORM
#include <nucleus/platform/platform_impl.h>
#include <nucleus/platform/input_impl.h>
#include <nucleus/platform/surface_impl.h>
#ifdef NU_BUILD_GLFW
#include <nucleus/platform/glfw_impl.h>
#endif
#endif

// #ifdef NU_BUILD_GRAPHICS
// #include <nucleus/graphics/renderer_impl.h>
// #include <nucleus/graphics/model_impl.h>
// #include <nucleus/graphics/font_impl.h>
// #include <nucleus/graphics/image_impl.h>
// #endif

#endif

#endif

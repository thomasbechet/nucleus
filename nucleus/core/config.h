#ifndef NU_CONFIG_H
#define NU_CONFIG_H

//////////////////////////////////////////////////////////////////////////
//////                       Module Configuration                   //////
//////////////////////////////////////////////////////////////////////////

#define NU_BUILD_PLATFORM
#define NU_BUILD_GLFW
#define NU_BUILD_GRAPHICS
#define NU_BUILD_GL
#define NU_BUILD_UTILS
#define NU_BUILD_IMPORTER
#define NU_BUILD_STBIMAGE
#define NU_BUILD_CGLTF
#define NU_BUILD_JSMN
#define NU_BUILD_ASSET
#define NU_BUILD_UI

#if defined(NU_NO_PLATFORM)
#undef NU_BUILD_PLATFORM
#undef NU_BUILD_GLFW
#endif

#if defined(NU_NO_GRAPHICS) || !defined(NU_BUILD_PLATFORM)
#undef NU_BUILD_GRAPHICS
#endif

#if defined(NU_NO_GL) || !defined(NU_BUILD_GRAPHICS)
#undef NU_BUILD_GL
#endif

#if defined(NU_NO_UTILS) || !defined(NU_BUILD_GRAPHICS)
#undef NU_BUILD_UTILS
#endif

#if defined(NU_NO_ASSET) || !defined(NU_BUILD_UTILS)
#undef NU_BUILD_ASSET
#endif

#if defined(NU_NO_IMPORTER) || !defined(NU_BUILD_ASSET)
#undef NU_BUILD_IMPORTER
#undef NU_BUILD_STBIMAGE
#undef NU_BUILD_CGLTF
#undef NU_BUILD_JSMN
#endif

//////////////////////////////////////////////////////////////////////////
//////                       Platform Detection                     //////
//////////////////////////////////////////////////////////////////////////
#if (defined(__WIN32__) || defined(WIN32) || defined(__MINGW32__) \
     || defined(_WIN32))
#define NU_PLATFORM_WINDOWS
#elif defined(__linux__) || defined(__unix__)
#ifndef _GNU_SOURCE
#define _GNU_SOURCE // Ensure ppoll definition for glfw
#endif
#define NU_PLATFORM_UNIX
#elif defined(__APPLE__)
#define NU_PLATFORM_APPLE
#endif

#ifdef __cplusplus
#define NU_CXX
#endif

//////////////////////////////////////////////////////////////////////////
//////                         External Includes                    //////
//////////////////////////////////////////////////////////////////////////

#define NU_STDLIB
#ifdef NU_STDLIB
#include <stdlib.h>
#include <assert.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <libgen.h>
#endif

#if defined(NU_PLATFORM_WINDOWS)
#include <Windows.h>
#elif defined(NU_PLATFORM_UNIX)
#elif defined(NU_PLATFORM_APPLE)
#endif

//////////////////////////////////////////////////////////////////////////
//////                     Import/Export Macros                     //////
//////////////////////////////////////////////////////////////////////////

#if defined(NU_PLATFORM_WINDOWS)

#define NU_API_EXPORT __declspec(dllexport)
#define NU_API_IMPORT __declspec(dllimport)
#if defined(_MSC_VER)
#define NU_ALIGN(X) __declspec(align(X))
#else
#define NU_ALIGN(X) __attribute((aligned(X)))
#endif

#elif defined(NU_PLATFORM_UNIX)

#define NU_API_EXPORT __attribute__((visibility("default")))
#define NU_API_IMPORT
#define NU_ALIGN(X) __attribute((aligned(X)))

#else

#define NU_API_EXPORT
#define NU_API_IMPORT
#define NU_ALIGN(X)
#pragma warning Unknown linkage directive import / export semantics.

#endif

#define NU_API NU_API_EXPORT

//////////////////////////////////////////////////////////////////////////
//////                            Macros                            //////
//////////////////////////////////////////////////////////////////////////

#define NU_UNUSED(x)       (void)x
#define nu_array_size(arr) (sizeof(arr) / sizeof(arr[0]))
#define nu_array_fill(arr, size, value)  \
    for (nu_size_t i = 0; i < size; ++i) \
    {                                    \
        (arr)[i] = (value);              \
    }

#define NU_DEFINE_HANDLE(type)      typedef struct type *type
#define nu_handle_index(handle)     ((nu_size_t)handle - 1)
#define nu_handle_make(type, index) ((type)(index + 1))

#if !defined(NU_NDEBUG) && defined(NU_STDLIB)
#define nu_assert(x) assert(x)
#else
#define nu_assert(x) (void)(x)
#endif

#if !defined(NU_NDEBUG) && defined(NU_STDLIB)
#define nu_assert(x) assert(x)
#else
#define nu_assert(x) (void)(x)
#endif

#define _NU_S(x)      #x
#define _NU_S_(x)     _NU_S(x)
#define _NU_S__LINE__ _NU_S_(__LINE__)

#define __SOURCE__ __FILE_NAME__ ":" _NU_S__LINE__

#ifdef NU_DEBUG
#define _nu_check(check, action, source) \
    if (!(check))                        \
    {                                    \
        action;                          \
    }
#else
#define _nu_check(check, action, source) \
    if (!(check))                        \
    {                                    \
        action;                          \
    }
#endif

#define nu_check(check, action) _nu_check(check, action, __SOURCE__)

#define nu_error_check(error, action) \
    _nu_check(error == NU_ERROR_NONE, action, __SOURCE__)
#define nu_error_assert(error) nu_assert(error == NU_ERROR_NONE)

#endif

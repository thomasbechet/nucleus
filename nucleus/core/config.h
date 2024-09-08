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

#if defined(NU_NO_IMPORTER)
#undef NU_BUILD_IMPORTER
#undef NU_BUILD_STBIMAGE
#undef NU_BUILD_CGLTF
#undef NU_BUILD_JSMN
#endif

#if defined(NU_NO_STBIMAGE) || !defined(NU_BUILD_GRAPHICS)
#undef NU_BUILD_STBIMAGE
#endif

#if defined(NU_NO_CGLTF) || !defined(NU_BUILD_GRAPHICS)
#undef NU_BUILD_CGLTF
#endif

#if defined(NU_NO_JSMN)
#undef NU_BUILD_JSMN
#undef NU_BUILD_CGLTF
#endif

#if defined(NU_NO_ASSET) || !defined(NU_BUILD_UTILS)
#undef NU_BUILD_ASSET
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
//////                          Basic Types                         //////
//////////////////////////////////////////////////////////////////////////

typedef enum
{
    NU_ERROR_NONE = 0,
    NU_ERROR_ALLOCATION,
    NU_ERROR_UNSUPPORTED_API,
    NU_ERROR_BACKEND,
    NU_ERROR_DUPLICATED,
    NU_ERROR_OUT_OF_RESOURCE,
    NU_ERROR_RESOURCE_LOADING,
    NU_ERROR_SHADER_COMPILATION,
} nu_error_t;

// TODO: use stdint types
typedef unsigned char  nu_u8_t;
typedef signed char    nu_i8_t;
typedef unsigned short nu_u16_t;
typedef signed short   nu_i16_t;
typedef unsigned int   nu_u32_t;
typedef signed int     nu_i32_t;
typedef unsigned long  nu_u64_t;
typedef signed long    nu_i64_t;

typedef char          nu_char_t;
typedef int           nu_bool_t;
typedef nu_i32_t      nu_int_t;
typedef unsigned long nu_size_t;
typedef unsigned char nu_byte_t;
typedef int           nu_word_t;

#define NU_TRUE  1
#define NU_FALSE 0
#define NU_NULL  0
#define NU_NOOP

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

#define NU_DEFINE_HANDLE(type) \
    typedef union              \
    {                          \
        nu_size_t id;          \
    } type
#define NU_HANDLE_INVALID(type)      ((type) { .id = (nu_size_t) - 1 })
#define NU_HANDLE_NULL(type)         ((type) { .id = 0 })
#define nu_handle_is_invalid(handle) ((handle).id == (nu_size_t) - 1)
#define nu_handle_is_valid(handle)   (!nu_handle_is_invalid(handle))

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

#define nu_error_CHECK(error, action) \
    _nu_check(error == NU_ERROR_NONE, action, __SOURCE__)
#define nu_error_ASSERT(error) nu_assert(error == NU_ERROR_NONE)

#define nu_handle_check(handle, action) \
    _nu_check(nu_handle_is_valid(handle), action, __SOURCE__)
#define nu_handle_assert(handle) nu_assert(nu_handle_is_valid(handle))

#endif

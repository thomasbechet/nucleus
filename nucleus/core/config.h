#ifndef NU_CONFIG_H
#define NU_CONFIG_H

//////////////////////////////////////////////////////////////////////////
//////                       Platform Detection                     //////
//////////////////////////////////////////////////////////////////////////

#define NU_BUILD_PLATFORM
#ifdef NU_NO_PLATFORM
#undef NU_BUILD_PLATFORM
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
//////                         Error Macros                         //////
//////////////////////////////////////////////////////////////////////////

#if !defined(NU_NDEBUG) && defined(NU_STDLIB)
#define NU_ASSERT(x) assert(x)
#else
#define NU_ASSERT(x) (void)(x)
#endif

#if !defined(NU_NDEBUG) && defined(NU_STDLIB)
#define NU_ASSERT(x) assert(x)
#else
#define NU_ASSERT(x) (void)(x)
#endif

#define _NU_S(x)      #x
#define _NU_S_(x)     _NU_S(x)
#define _NU_S__LINE__ _NU_S_(__LINE__)

#ifndef NU_NDEBUG
#define _NU_CHECK(check, action, file, line) \
    if (!(check))                            \
    {                                        \
        action;                              \
    }
#else
#define _NU_CHECK(check, action, file, line) \
    if (!(check))                            \
    {                                        \
        action;                              \
    }
#endif

#define NU_CHECK(check, action) \
    _NU_CHECK(check, action, __FILE__, _NU_S__LINE__)

#define NU_ERROR_CHECK(error, action) \
    _NU_CHECK(error == NU_ERROR_NONE, action, __FILE__, _NU_S__LINE__)

#define NU_ERROR_ASSERT(error) NU_ASSERT(error == NU_ERROR_NONE)

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
//////                     Backends Selection                       //////
//////////////////////////////////////////////////////////////////////////

#if !defined(NU_NO_GLFW)
#if defined(NU_PLATFORM_WINDOWS) || defined(NU_PLATFORM_UNIX) \
    || defined(NU_PLATFORM_APPLE)
#ifndef NU_BUILD_GLFW
#define NU_BUILD_GLFW
#endif
#endif
#endif

#ifdef NU_BUILD_GLFW
#ifndef NU_BUILD_GL
#define NU_BUILD_GL
#endif
#endif

//////////////////////////////////////////////////////////////////////////
//////                          Error Types                         //////
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

//////////////////////////////////////////////////////////////////////////
//////                          Basic Types                         //////
//////////////////////////////////////////////////////////////////////////

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
//////                      Additional Macros                       //////
//////////////////////////////////////////////////////////////////////////

#define NU_UNUSED(x) (void)x

#define NU_DEFINE_HANDLE(type) \
    typedef union              \
    {                          \
        void     *_ptr;        \
        nu_u32_t  _id;         \
        nu_size_t _index;      \
    } type

#endif

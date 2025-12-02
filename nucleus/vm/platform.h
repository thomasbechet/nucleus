#ifndef NU_PLATFORM_H
#define NU_PLATFORM_H

/* detect platform */
#if (defined(__WIN32__) || defined(WIN32) || defined(__MINGW32__) \
     || defined(_WIN32))
#define NU_PLATFORM_WINDOWS
#elif defined(__linux__) || defined(__unix__)
#define NU_PLATFORM_UNIX
#elif defined(__APPLE__)
#define NU_PLATFORM_APPLE
#endif

/* import stdlib */
#ifdef NU_STDLIB
#include <assert.h>
#endif

/**
 * configure core macos per platform
 */
#if defined(NU_PLATFORM_WINDOWS)

/* api */
#define NU_API_EXPORT __declspec(dllexport)
#define NU_API_IMPORT __declspec(dllimport)
/* memory alignment */
#if defined(_MSC_VER)
#define NU_ALIGN(X) __declspec(align(X))
#else
#define NU_ALIGN(X) __attribute((aligned(X)))
#endif
#elif defined(NU_PLATFORM_UNIX)
/* api */
#define NU_API_EXPORT __attribute__((visibility("default")))
#define NU_API_IMPORT
/* memory aligment */
#define NU_ALIGN(X) __attribute((aligned(X)))

#else

/* api */
#define NU_API_EXPORT
#define NU_API_IMPORT
/* memory aligment */
#define NU_ALIGN(X)
#pragma warning Unknown linkage directive import / export semantics.

#endif

#define NU_API NU_API_EXPORT

/* C++ */
#ifdef __cplusplus
#define NU_CXX
#endif
#endif

#ifndef NU_CONFIG_H
#define NU_CONFIG_H

// Platform detection
#if (defined(__WIN32__) || defined(WIN32) || defined(__MINGW32__) \
     || defined(_WIN32))
#define NU_PLATFORM_WINDOWS
#elif defined(__linux__) || defined(__unix__)
#define _GNU_SOURCE // Ensure ppoll definition for glfw
#define NU_PLATFORM_UNIX
#elif defined(__APPLE__)
#define NU_PLATFORM_APPLE
#endif

// Import standard libs
#ifdef NU_STDLIB
#include <assert.h>
#endif

#if defined(NU_DEBUG) && defined(NU_STDLIB)
#define NU_ASSERT(x) assert(x)
#else
#define NU_ASSERT(x) (void)(x)
#endif

// Configure core macros per platform
#if defined(NU_PLATFORM_WINDOWS)

// API
#define NU_API_EXPORT __declspec(dllexport)
#define NU_API_IMPORT __declspec(dllimport)
// Memory alignment
#if defined(_MSC_VER)
#define NU_ALIGN(X) __declspec(align(X))
#else
#define NU_ALIGN(X) __attribute((aligned(X)))
#endif
#elif defined(NU_PLATFORM_UNIX)
// API
#define NU_API_EXPORT __attribute__((visibility("default")))
#define NU_API_IMPORT
// Memory alignment
#define NU_ALIGN(X) __attribute((aligned(X)))

#else

// API
#define NU_API_EXPORT
#define NU_API_IMPORT
// Memory alignment
#define NU_ALIGN(X)
#pragma warning Unknown linkage directive import / export semantics.

#endif

#define NU_API NU_API_EXPORT

// C++
#ifdef __cplusplus
#define NU_CXX
#endif

#endif

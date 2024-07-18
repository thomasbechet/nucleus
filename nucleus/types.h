#ifndef NU_TYPES_H
#define NU_TYPES_H

#include <nucleus/config.h>

//////////////////////////////////////////////////////////////////////////
//////                          Error Types                         //////
//////////////////////////////////////////////////////////////////////////

typedef enum
{
    NU_ERROR_NONE            = 0,
    NU_ERROR_UNSUPPORTED_API = 1,
    NU_ERROR_BACKEND         = 2,
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
typedef int           nu_word_t;

#define NU_TRUE  1
#define NU_FALSE 0
#define NU_NULL  0
#define NU_NOOP

//////////////////////////////////////////////////////////////////////////
//////                          Math Types                          //////
//////////////////////////////////////////////////////////////////////////

typedef nu_int_t nu_fix_t;
typedef nu_fix_t nu_mat3_t[9];
typedef nu_fix_t nu_mat4_t[16];
typedef nu_fix_t nu_quat_t[4];

#define NU_FIX_FRAC  16
#define NU_MIN(a, b) (((a) < (b)) ? (a) : (b))
#define NU_MAX(a, b) (((a) > (b)) ? (a) : (b))

#define NU_VEC2_SIZE 3
#define NU_VEC3_SIZE 3
#define NU_VEC4_SIZE 4
#define NU_MAT3_SIZE 9
#define NU_MAT4_SIZE 16

//////////////////////////////////////////////////////////////////////////
//////                       Allocator Types                        //////
//////////////////////////////////////////////////////////////////////////

typedef struct
{
    void *(*callback)(void     *p, // pointer (NULL if malloc)
                      nu_size_t s, // current size (0 if malloc)
                      nu_size_t n, // new size (0 if free)
                      nu_size_t a, // alignment
                      void     *u);    // userdata
    void *userdata;
} nu_allocator_t;

//////////////////////////////////////////////////////////////////////////
//////                        Surface Types                         //////
//////////////////////////////////////////////////////////////////////////

typedef enum
{
    NU_SURFACE_API_NONE,
    NU_SURFACE_API_GLFW,
} nu_surface_api_t;

typedef struct
{
    nu_u32_t width;
    nu_u32_t height;
} nu_surface_info_t;

//////////////////////////////////////////////////////////////////////////
//////                         Input Types                          //////
//////////////////////////////////////////////////////////////////////////

typedef enum
{
    NU_INPUT_API_NONE,
    NU_INPUT_API_GLFW,
} nu_input_api_t;

typedef struct
{
    nu_u32_t value;
    nu_u32_t prev;
    nu_u32_t max;
} nu_input_t;

#define NU_INPUT(m)                     \
    (nu_input_t)                        \
    {                                   \
        .value = 0, .prev = 0, .max = m \
    }

//////////////////////////////////////////////////////////////////////////
//////                       Graphics Types                         //////
//////////////////////////////////////////////////////////////////////////

typedef enum
{
    NU_GRAPHICS_API_NONE,
    NU_GRAPHICS_API_OPENGL,
} nu_graphics_api_t;

//////////////////////////////////////////////////////////////////////////
//////                        Context Types                         //////
//////////////////////////////////////////////////////////////////////////

typedef struct
{
    nu_surface_api_t  surface_api;
    nu_input_api_t    input_api;
    nu_graphics_api_t graphics_api;
} nu_context_info_t;

typedef struct
{
    nu_context_info_t _info;
    nu_bool_t         _close_requested;
#ifdef NU_BUILD_GLFW
    GLFWwindow *_glfw_window;
#endif
} nu_context_t;

#endif

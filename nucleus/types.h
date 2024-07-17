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

#define NU_DEFINE_VEC2_TYPE(NAME, T) \
    typedef union                    \
    {                                \
        struct                       \
        {                            \
            T x;                     \
            T y;                     \
        };                           \
        T data[2];                   \
    } NAME;
#define NU_DEFINE_VEC3_TYPE(NAME, T) \
    typedef union                    \
    {                                \
        struct                       \
        {                            \
            T x;                     \
            T y;                     \
            T z;                     \
        };                           \
        T data[3];                   \
    } NAME;
#define NU_DEFINE_VEC4_TYPE(NAME, T) \
    typedef union                    \
    {                                \
        struct                       \
        {                            \
            T x;                     \
            T y;                     \
            T z;                     \
            T w;                     \
        };                           \
        T data[4];                   \
    } NAME;

NU_DEFINE_VEC2_TYPE(nu_vec2_t, nu_fix_t);
NU_DEFINE_VEC2_TYPE(nu_ivec2_t, nu_int_t);
NU_DEFINE_VEC3_TYPE(nu_vec3_t, nu_fix_t);
NU_DEFINE_VEC3_TYPE(nu_ivec3_t, nu_int_t);
NU_DEFINE_VEC4_TYPE(nu_vec4_t, nu_fix_t);
NU_DEFINE_VEC4_TYPE(nu_ivec4_t, nu_int_t);

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
    nu_u32_t count;
} nu_action_t;

typedef enum
{
    NU_RANGE_TYPE_CLAMPED,
    NU_RANGE_TYPE_NORMALIZED,
    NU_RANGE_TYPE_CLAMED_NORMALIZED,
    NU_RANGE_TYPE_INFINITE
} nu_range_type_t;

typedef struct
{
    nu_fix_t        value;
    nu_range_type_t type;
    union
    {
        struct
        {
            nu_fix_t min;
            nu_fix_t max;
        } clamped;
        struct
        {
            nu_fix_t norm;
        } normalized;
        struct
        {
            nu_fix_t min;
            nu_fix_t max;
            nu_fix_t norm;
        } clamped_normalized;
    };
} nu_range_t;

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
#ifdef NU_BUILD_GLFW
    GLFWwindow *_glfw_window;
#endif
} nu_context_t;

#endif

#ifndef NU_TYPES_H
#define NU_TYPES_H

#include <nucleus/vm/platform.h>

/**
 * This is not a complete implementation, each
 * compiler should define its own types to ensure
 * pedantry.
 */
typedef unsigned char  nu_u8_t;
typedef signed char    nu_i8_t;
typedef unsigned short nu_u16_t;
typedef signed short   nu_i16_t;
typedef unsigned int   nu_u32_t;
typedef signed int     nu_i32_t;
typedef unsigned long  nu_u64_t;
typedef signed long    nu_i64_t;

#define NU_U16_MAX 0xFFFF

typedef char          nu_char_t;
typedef int           nu_bool_t;
typedef nu_i32_t      nu_int_t;
typedef unsigned long nu_size_t;
typedef int           nu_word_t;

#define NU_TRUE  1
#define NU_FALSE 0
#define NU_NULL  0
#define NU_NOOP

typedef nu_u32_t nu_handle_t;

typedef void (*nu_pfn_t)(void);

#define NU_FOREACH_PRIMITIVE(PRIMITIVE) \
    PRIMITIVE(BOOL)                     \
    PRIMITIVE(INT)                      \
    PRIMITIVE(FIX)                      \
    PRIMITIVE(IVEC2)                    \
    PRIMITIVE(IVEC3)                    \
    PRIMITIVE(IVEC4)                    \
    PRIMITIVE(VEC2)                     \
    PRIMITIVE(VEC3)                     \
    PRIMITIVE(VEC4)                     \
    PRIMITIVE(MAT4)                     \
    PRIMITIVE(QUAT)                     \
    PRIMITIVE(ENTITY)                   \
    PRIMITIVE(RESOURCE)
#define NU_GENERATE_PRIMITIVE_ENUM(PRIMITIVE) NU_PRIMITIVE_##PRIMITIVE,
#define NU_GENERATE_PRIMITIVE_NAME(PRIMITIVE) #PRIMITIVE,
typedef enum
{
    NU_FOREACH_PRIMITIVE(NU_GENERATE_PRIMITIVE_ENUM) NU_PRIMITIVE_UNKNOWN
} nu_primitive_t;
const nu_char_t *NU_PRIMITIVE_NAMES[]
    = { NU_FOREACH_PRIMITIVE(NU_GENERATE_PRIMITIVE_NAME) "UNKNOWN" };

#define NU_ARRAY_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))

#endif

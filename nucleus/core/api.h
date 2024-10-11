#ifndef NU_CORE_API_H
#define NU_CORE_API_H

#include <nucleus/core/config.h>

#define NU_TRUE  1
#define NU_FALSE 0
#define NU_NULL  0
#define NU_NOOP

NU_DEFINE_HANDLE(nu_table_t);

#define NU_DEFAULT_ALIGN  16
#define NU_COLOR_WHITE    nu_color(255, 255, 255, 0)
#define NU_COLOR_BLACK    nu_color(0, 0, 0, 0)
#define NU_COLOR_RED      nu_color(255, 0, 0, 0)
#define NU_COLOR_GREEN    nu_color(0, 255, 0, 0)
#define NU_COLOR_BLUE     nu_color(0, 0, 255, 0)
#define NU_COLOR_BLUE_SKY nu_color(52, 181, 248, 0)
#define NUEXT_PATH_MAX    256
#define NU_PI             3.14159265359
#define NU_FLT_MAX        3.402823E+38

#define NU_MIN(a, b)          (((a) < (b)) ? (a) : (b))
#define NU_MAX(a, b)          (((a) > (b)) ? (a) : (b))
#define NU_CLAMP(x, min, max) (NU_MAX(min, NU_MIN(max, x)))
#define NU_CLAMP01(x)         (NU_CLAMP(x, 0, 1))

#define NU_V2_SIZE 2
#define NU_V3_SIZE 3
#define NU_V4_SIZE 4
#define NU_Q4_SIZE 4
#define NU_M3_SIZE 9
#define NU_M4_SIZE 16

#define NU_V2_ZEROS nu_v2(0, 0)
#define NU_V2_ONES  nu_v2(1, 1)

#define NU_V2I_ZEROS nu_v2i(0, 0)

#define NU_V2U_ZEROS nu_v2u(0, 0)
#define NU_V2U_ONES  nu_v2u(1, 1)

#define NU_V3U_ONES nu_v3u(1, 1, 1)

#define NU_V3_ZEROS    nu_v3(0, 0, 0)
#define NU_V3_ONES     nu_v3(1, 1, 1)
#define NU_V3_UP       nu_v3(0, 1, 0)
#define NU_V3_DOWN     nu_v3(0, -1, 0)
#define NU_V3_FORWARD  nu_v3(0, 0, -1)
#define NU_V3_BACKWARD nu_v3(0, 0, 1)
#define NU_V3_LEFT     nu_v3(-1, 0, 0)
#define NU_V3_RIGHT    nu_v3(1, 0, 0)

#define NU_V4_ZEROS nu_vec4(0, 0, 0, 0)

#define NU_V2_FORMAT "%lf %lf"
#define NU_V3_FORMAT "%lf %lf %lf"
#define NU_V4_FORMAT "%lf %lf %lf %lf"

#define NU_DEBUG(...)   nu_log(NU_LOG_DEBUG, __SOURCE__, __VA_ARGS__)
#define NU_INFO(...)    nu_log(NU_LOG_INFO, __SOURCE__, __VA_ARGS__)
#define NU_WARNING(...) nu_log(NU_LOG_WARNING, __SOURCE__, __VA_ARGS__)
#define NU_ERROR(...)   nu_log(NU_LOG_ERROR, __SOURCE__, __VA_ARGS__)

#define NU_UID(name) nu_hash(name)

#define NU_MATCH(a, b) (nu_strcmp(a, b) == 0)

#ifdef NU_CXX
#define NU_VOID_CAST(type, expr) (static_cast<decltype(type)>(expr))
#else
#define NU_VOID_CAST(type, expr) (expr)
#endif

#define NU_VEC(type)               \
    struct                         \
    {                              \
        type           *data;      \
        nu_size_t       size;      \
        nu_size_t       capacity;  \
        nu_allocator_t *allocator; \
    }

#define NU_VEC_INIT(cap, v) NU_VEC_INIT_A(nu_allocator_core(), cap, v)

#define NU_VEC_INIT_A(alloc, cap, v)                                     \
    do                                                                   \
    {                                                                    \
        (v)->data = NU_VOID_CAST(                                        \
            (v)->data, nu_alloc_a((alloc), sizeof(*(v)->data) * (cap))); \
        (v)->capacity  = (cap);                                          \
        (v)->size      = 0;                                              \
        (v)->allocator = (alloc);                                        \
    } while (0)

#define NU_VEC_FREE(v) \
    nu_free_a((v)->allocator, (v)->data, sizeof(*(v)->data) * (v)->capacity)

#define NU_VEC_CLEAR(v) ((v)->size = 0)

#define NU_VEC_PUSH(v)                                            \
    (((v)->data = NU_VOID_CAST((v)->data,                         \
                               nu__vec_push((v)->allocator,       \
                                            sizeof(*((v)->data)), \
                                            (v)->data,            \
                                            &(v)->size,           \
                                            &(v)->capacity)))     \
         ? (v)->data + ((v)->size - 1)                            \
         : NU_NULL)

#define NU_VEC_POP(v) \
    (nu__vec_pop(&(v)->size) ? ((v)->data + (v)->size) : NU_NULL)

#define NU_VEC_RESIZE(v, new_size)                               \
    do                                                           \
    {                                                            \
        if ((v)->size != (new_size))                             \
        {                                                        \
            (v)->data = NU_VOID_CAST(                            \
                (v)->data,                                       \
                nu_realloc_a((v)->allocator,                     \
                             (v)->data,                          \
                             sizeof(*(v)->data) * (v)->capacity, \
                             sizeof(*(v)->data) * (new_size)));  \
            (v)->capacity = (new_size);                          \
            (v)->size     = (new_size);                          \
        }                                                        \
    } while (0)

#define NU_VEC_APPEND(dst, src)                                  \
    do                                                           \
    {                                                            \
        NU_ASSERT(sizeof(*(dst)->data) == sizeof(*(src)->data)); \
        nu_size_t dst_size = (dst)->size;                        \
        nu_size_t src_size = (src)->size;                        \
        nu_size_t new_size = dst_size + src_size;                \
        NU_VEC_RESIZE((dst), new_size);                          \
        nu_memcpy((dst)->data + dst_size,                        \
                  (src)->data,                                   \
                  sizeof(*(src)->data) * src_size);              \
    } while (0)

#define NU_VEC_LAST(v) ((v)->size ? (v)->data + ((v)->size - 1) : NU_NULL)

#define NU_POOL(type)            \
    struct                       \
    {                            \
        type         *data;      \
        nu_size_t     capacity;  \
        nu_size_vec_t _freelist; \
    }

#define NU_POOL_INIT(cap, p) NU_POOL_INIT_A(nu_allocator_core(), cap, p)

#define NU_POOL_INIT_A(alloc, cap, p)                                    \
    do                                                                   \
    {                                                                    \
        (p)->data = NU_VOID_CAST(                                        \
            (p)->data, nu_alloc_a((alloc), sizeof(*(p)->data) * (cap))); \
        nu_memset((p)->data, 0, sizeof(*(p)->data) * (cap));             \
        (p)->capacity = (cap);                                           \
        NU_VEC_INIT_A(alloc, (cap), &(p)->_freelist);                    \
        for (nu_size_t i = 0; i < (cap); ++i)                            \
        {                                                                \
            *NU_VEC_PUSH(&(p)->_freelist) = (cap) - i - 1;               \
        }                                                                \
    } while (0)

#define NU_POOL_FREE(p)                                \
    do                                                 \
    {                                                  \
        nu_free_a((p)->_freelist.allocator,            \
                  (p)->data,                           \
                  sizeof(*(p)->data) * (p)->capacity); \
        NU_VEC_FREE(&(p)->_freelist);                  \
    } while (0)

#define NU_POOL_ADD(p, pindex)                                  \
    (((p)->data = NU_VOID_CAST((p)->data,                       \
                               nu__pool_add(sizeof(*(p)->data), \
                                            (p)->data,          \
                                            &(p)->capacity,     \
                                            &(p)->_freelist,    \
                                            (pindex))))         \
         ? (p)->data + (*(pindex))                              \
         : NU_NULL)

#define NU_POOL_REMOVE(s, index) (*nu_vec_push_checked(&(s)->_freelist) = index)

// TODO: use stdint types
typedef unsigned char  nu_u8_t;
typedef signed char    nu_i8_t;
typedef unsigned short nu_u16_t;
typedef signed short   nu_i16_t;
typedef unsigned int   nu_u32_t;
typedef signed int     nu_i32_t;
typedef unsigned long  nu_u64_t;
typedef signed long    nu_i64_t;
typedef float          nu_f32_t;
typedef double         nu_f64_t;

typedef char          nu_char_t;
typedef int           nu_bool_t;
typedef nu_i32_t      nu_int_t;
typedef unsigned long nu_size_t;
typedef intptr_t      nu_intptr_t;
typedef unsigned char nu_byte_t;
typedef int           nu_word_t;
typedef nu_u32_t      nu_uid_t;

typedef enum
{
    NU_ERROR_NONE = 0, // error none is required to be NULL
    NU_ERROR_ALLOCATION,
    NU_ERROR_UNSUPPORTED_API,
    NU_ERROR_BACKEND,
    NU_ERROR_DUPLICATED,
    NU_ERROR_OUT_OF_RESOURCE,
    NU_ERROR_RESOURCE_LOADING,
    NU_ERROR_SHADER_COMPILATION,
} nu_error_t;

typedef enum
{
    NU_TABLE_INT,
    NU_TABLE_FLOAT,
} nu_table_type_t;

typedef enum
{
    NU_LOG_DEBUG   = 5,
    NU_LOG_INFO    = 4,
    NU_LOG_WARNING = 3,
    NU_LOG_ERROR   = 2,
} nu_log_level_t;

typedef enum
{
    NUEXT_EXTENSION_GLTF,
    NUEXT_EXTENSION_PNG,
    NUEXT_EXTENSION_JPEG,
    NUEXT_EXTENSION_JSON,
    NUEXT_EXTENSION_UNKNOWN,
} nuext_extension_t;

typedef struct
{
    void *(*callback)(void     *p, // pointer (NULL if malloc)
                      nu_size_t s, // current size (0 if malloc)
                      nu_size_t n, // new size (0 if free)
                      nu_size_t a, // alignment
                      void     *u);    // userdata
    void *userdata;
} nu_allocator_t;

typedef struct
{
    time_t value;
} nu_time_t;

typedef struct
{
    struct timespec start;
} nu_timer_t;

typedef struct
{
    nu_bool_t active;
    nu_u32_t  id;
    nu_f32_t  timestep;
    nu_f32_t  _acc;
} nu_fixed_loop_t;

typedef union
{
    struct
    {
        nu_u8_t r;
        nu_u8_t g;
        nu_u8_t b;
        nu_u8_t a;
    };
    nu_u32_t rgba;
} nu_color_t;

typedef union
{
    struct
    {
        nu_f32_t x;
        nu_f32_t y;
    };
    nu_f32_t data[NU_V2_SIZE];
} nu_v2_t;

typedef union
{
    struct
    {
        nu_f32_t x;
        nu_f32_t y;
        nu_f32_t z;
    };
    nu_f32_t data[NU_V3_SIZE];
} nu_v3_t;

typedef union
{
    struct
    {
        nu_f32_t x;
        nu_f32_t y;
        nu_f32_t z;
        nu_f32_t w;
    };
    nu_f32_t data[NU_V4_SIZE];
} nu_v4_t;

typedef union
{
    struct
    {
        nu_u32_t x;
        nu_u32_t y;
    };
    nu_u32_t data[NU_V2_SIZE];
} nu_v2u_t;

typedef union
{
    struct
    {
        nu_i32_t x;
        nu_i32_t y;
    };
    nu_i32_t data[NU_V2_SIZE];
} nu_v2i_t;

typedef union
{
    struct
    {
        nu_i32_t x;
        nu_i32_t y;
        nu_i32_t z;
    };
    nu_i32_t data[NU_V3_SIZE];
} nu_v3i_t;

typedef union
{
    struct
    {
        nu_u32_t x;
        nu_u32_t y;
        nu_u32_t z;
    };
    nu_u32_t data[NU_V3_SIZE];
} nu_v3u_t;

typedef union
{
    struct
    {
        nu_u32_t x;
        nu_u32_t y;
        nu_u32_t z;
        nu_u32_t w;
    };
    nu_u32_t data[NU_V4_SIZE];
} nu_v4u_t;

typedef union
{
    struct
    {
        nu_f32_t x;
        nu_f32_t y;
        nu_f32_t z;
        nu_f32_t w;
    };
    nu_f32_t data[NU_Q4_SIZE];
} nu_q4_t;

typedef union
{

    struct
    {
        nu_f32_t x1;
        nu_f32_t x2;
        nu_f32_t x3;
        nu_f32_t y1;
        nu_f32_t y2;
        nu_f32_t y3;
        nu_f32_t z1;
        nu_f32_t z2;
        nu_f32_t z3;
    };
    nu_f32_t data[NU_M3_SIZE];
} nu_m3_t;

typedef union
{
    struct
    {
        nu_f32_t x1;
        nu_f32_t x2;
        nu_f32_t x3;
        nu_f32_t x4;
        nu_f32_t y1;
        nu_f32_t y2;
        nu_f32_t y3;
        nu_f32_t y4;
        nu_f32_t z1;
        nu_f32_t z2;
        nu_f32_t z3;
        nu_f32_t z4;
        nu_f32_t w1;
        nu_f32_t w2;
        nu_f32_t w3;
        nu_f32_t w4;
    };
    nu_f32_t data[NU_M4_SIZE];
} nu_m4_t;

typedef struct
{
    nu_v2i_t min;
    nu_v2i_t max;
} nu_b2i_t;

typedef struct
{
    nu_v3i_t min;
    nu_v3i_t max;
} nu_b3i_t;

typedef struct
{
    nu_v3_t min;
    nu_v3_t max;
} nu_b3_t;

typedef NU_VEC(nu_v2_t) nu_v2_vec_t;
typedef NU_VEC(nu_v3_t) nu_v3_vec_t;
typedef NU_VEC(nu_bool_t) nu_bool_vec_t;
typedef NU_VEC(nu_u32_t) nu_u32_vec_t;
typedef NU_VEC(nu_size_t) nu_size_vec_t;

NU_API nu_error_t nu_init(void);
NU_API void       nu_terminate(void);

NU_API void nu__panic(const nu_char_t *source, const nu_char_t *format, ...);
NU_API void nu__vpanic(const nu_char_t *source,
                       const nu_char_t *format,
                       va_list          args);

NU_API void nu_log(nu_log_level_t   level,
                   const nu_char_t *source,
                   const nu_char_t *format,
                   ...);
NU_API void nu_vlog(nu_log_level_t   level,
                    const nu_char_t *source,
                    const nu_char_t *format,
                    va_list          args);

NU_API nu_allocator_t *nu_allocator_core(void);

NU_API void *nu_alloc_a(nu_allocator_t *a, nu_size_t n);
NU_API void *nu_realloc_a(nu_allocator_t *a, void *p, nu_size_t s, nu_size_t n);
NU_API void  nu_free_a(nu_allocator_t *a, void *p, nu_size_t s);

NU_API void *nu_alloc(nu_size_t n);
NU_API void *nu_realloc(void *p, nu_size_t s, nu_size_t n);
NU_API void  nu_free(void *p, nu_size_t s);

NU_API void *nu_memset(void *dst, nu_word_t c, nu_size_t n);
NU_API void  nu_memcpy(void *dst, const void *src, nu_size_t n);
NU_API void *nu_memalign(void *ptr, nu_size_t align);

NU_API nu_time_t nu_time(void);
NU_API nu_u32_t  nu_time_hours(const nu_time_t *time);
NU_API nu_u32_t  nu_time_minutes(const nu_time_t *time);
NU_API nu_u32_t  nu_time_seconds(const nu_time_t *time);

NU_API void     nu_timer_reset(nu_timer_t *timer);
NU_API nu_f32_t nu_timer_elapsed(nu_timer_t *timer);

NU_API nu_fixed_loop_t nu_fixed_loop(nu_u32_t id, nu_f32_t timestep);
NU_API nu_bool_t       nu_fixed_loop_next(nu_fixed_loop_t *loops,
                                          nu_size_t        count,
                                          nu_u32_t        *id);
NU_API void            nu_fixed_loop_update(nu_fixed_loop_t *loops,
                                            nu_size_t        count,
                                            nu_f32_t         dt);

NU_API nu_size_t  nu_strnlen(const nu_char_t *str, nu_size_t maxlen);
NU_API nu_size_t  nu_strlen(const nu_char_t *str);
NU_API nu_size_t  nu_strncmp(const nu_char_t *s1,
                             const nu_char_t *s2,
                             nu_size_t        n);
NU_API nu_size_t  nu_strcmp(const nu_char_t *s1, const nu_char_t *s2);
NU_API nu_char_t *nu_strncpy(nu_char_t *dst, const nu_char_t *src, nu_size_t n);
NU_API nu_int_t   nu_snprintf(nu_char_t       *s,
                              nu_size_t        n,
                              const nu_char_t *format,
                              ...);

NU_API nu_u32_t nu_hash(const nu_char_t *s);
NU_API nu_u32_t nu_hashn(const nu_char_t *s, nu_size_t n);

NU_API nuext_extension_t nuext_path_extension(const nu_char_t *filename);
NU_API void nuext_path_basename(const nu_char_t *path, nu_char_t *name);
NU_API void nuext_path_dirname(const nu_char_t *path, nu_char_t *name);
NU_API void nuext_path_concat(const nu_char_t *p1,
                              const nu_char_t *p2,
                              nu_char_t       *path);

NU_API nu_color_t nu_color(nu_u8_t r, nu_u8_t g, nu_u8_t b, nu_u8_t a);
NU_API nu_v4_t    nu_color_to_vec4(nu_color_t c);
NU_API nu_color_t nu_color_from_vec4(nu_v4_t v);
NU_API nu_color_t nu_color_to_srgb(nu_color_t c);
NU_API nu_color_t nu_color_to_linear(nu_color_t c);

NU_API void *nu__pool_add(nu_size_t      tsize,
                          void          *data,
                          nu_size_t     *capacity,
                          nu_size_vec_t *freelist,
                          nu_size_t     *pindex);

NU_API void     *nu__vec_push(nu_allocator_t *alloc,
                              nu_size_t       tsize,
                              void           *data,
                              nu_size_t      *size,
                              nu_size_t      *capacity);
NU_API nu_bool_t nu__vec_pop(nu_size_t *size);

NU_API nu_bool_t nu_is_power_of_two(nu_size_t n);
NU_API nu_size_t nu_log2(nu_size_t n);
NU_API nu_f32_t  nu_fabs(nu_f32_t f);
NU_API nu_f32_t  nu_floor(nu_f32_t f);
NU_API nu_f32_t  nu_radian(nu_f32_t d);
NU_API nu_f32_t  nu_sqrt(nu_f32_t x);
NU_API nu_f32_t  nu_pow(nu_f32_t b, nu_f32_t e);
NU_API nu_f32_t  nu_cos(nu_f32_t x);
NU_API nu_f32_t  nu_sin(nu_f32_t x);
NU_API nu_f32_t  nu_tan(nu_f32_t x);
NU_API nu_f32_t  nu_exp(nu_f32_t x);

NU_API nu_v2_t nu_v2(nu_f32_t x, nu_f32_t y);
NU_API nu_v2_t nu_v2_v2u(nu_v2u_t v);
NU_API nu_v2_t nu_v2_add(nu_v2_t a, nu_v2_t b);
NU_API nu_v2_t nu_v2_sub(nu_v2_t a, nu_v2_t b);
NU_API nu_v2_t nu_v2_mul(nu_v2_t a, nu_v2_t b);
NU_API nu_v2_t nu_v2_muls(nu_v2_t a, nu_f32_t s);
NU_API nu_v2_t nu_v2_div(nu_v2_t a, nu_v2_t b);
NU_API nu_v2_t nu_v2_divs(nu_v2_t a, nu_f32_t s);
NU_API nu_v2_t nu_v2_floor(nu_v2_t a);

NU_API nu_v3_t  nu_v3(nu_f32_t x, nu_f32_t y, nu_f32_t z);
NU_API nu_v3_t  nu_v3s(nu_f32_t s);
NU_API nu_v3_t  nu_v3_add(nu_v3_t a, nu_v3_t b);
NU_API nu_v3_t  nu_v3_sub(nu_v3_t a, nu_v3_t b);
NU_API nu_v3_t  nu_v3_mul(nu_v3_t a, nu_v3_t b);
NU_API nu_v3_t  nu_v3_muls(nu_v3_t a, nu_f32_t s);
NU_API nu_v3_t  nu_v3_div(nu_v3_t a, nu_v3_t b);
NU_API nu_v3_t  nu_v3_divs(nu_v3_t a, nu_f32_t s);
NU_API nu_f32_t nu_v3_norm(nu_v3_t a);
NU_API nu_v3_t  nu_v3_normalize(nu_v3_t a);
NU_API nu_v3_t  nu_v3_cross(nu_v3_t a, nu_v3_t b);
NU_API nu_f32_t nu_v3_dot(nu_v3_t a, nu_v3_t b);
NU_API nu_v3_t  nu_v3_min(nu_v3_t a, nu_v3_t b);
NU_API nu_v3_t  nu_v3_max(nu_v3_t a, nu_v3_t b);

NU_API nu_v4_t  nu_v4(nu_f32_t x, nu_f32_t y, nu_f32_t z, nu_f32_t w);
NU_API nu_f32_t nu_v4_dot(nu_v4_t a, nu_v4_t b);
NU_API nu_f32_t nu_v4_norm(nu_v4_t v);
NU_API nu_v2_t  nu_v4_xy(nu_v4_t v);
NU_API nu_v2_t  nu_v4_zw(nu_v4_t v);

NU_API nu_v2i_t nu_v2i(nu_i32_t x, nu_i32_t y);
NU_API nu_v2i_t nu_v2i_add(nu_v2i_t a, nu_v2i_t b);
NU_API nu_v2i_t nu_v2i_sub(nu_v2i_t a, nu_v2i_t b);
NU_API nu_v2i_t nu_v2i_min(nu_v2i_t a, nu_v2i_t b);
NU_API nu_v2i_t nu_v2i_max(nu_v2i_t a, nu_v2i_t b);

NU_API nu_v2u_t nu_v2u(nu_u32_t x, nu_u32_t y);
NU_API nu_v2u_t nu_v2u_min(nu_v2u_t a, nu_v2u_t b);

NU_API nu_v3u_t nu_v3u(nu_u32_t x, nu_u32_t y, nu_u32_t z);
NU_API nu_v3u_t nu_v3u_v2u(nu_v2u_t v, nu_u32_t z);
NU_API nu_v2u_t nu_v3u_xy(nu_v3u_t v);

NU_API nu_v4u_t nu_v4u(nu_u32_t x, nu_u32_t y, nu_u32_t z, nu_u32_t w);

NU_API nu_q4_t  nu_q4(nu_f32_t x, nu_f32_t y, nu_f32_t z, nu_f32_t w);
NU_API nu_q4_t  nu_q4_identity(void);
NU_API nu_v4_t  nu_q4_vec4(nu_q4_t a);
NU_API nu_f32_t nu_q4_norm(nu_q4_t a);
NU_API nu_q4_t  nu_q4_axis(nu_v3_t axis, nu_f32_t angle);
NU_API nu_q4_t  nu_q4_mul(nu_q4_t a, nu_q4_t b);
NU_API nu_v3_t  nu_q4_mulv3(nu_q4_t a, nu_v3_t v);
NU_API nu_q4_t  nu_q4_mul_axis(nu_q4_t q, nu_v3_t axis, nu_f32_t angle);
NU_API nu_m3_t  nu_q4_mat3(nu_q4_t q);
NU_API nu_m4_t  nu_q4_mat4(nu_q4_t q);
NU_API nu_m4_t  nu_q4_mulm4(nu_q4_t a, nu_m4_t m);

NU_API nu_m3_t nu_m3_zero(void);
NU_API nu_m3_t nu_m3_identity(void);
NU_API nu_m3_t nu_m3_translate(nu_v2_t v);
NU_API nu_m3_t nu_m3_scale(nu_v2_t v);
NU_API nu_m3_t nu_m3_mul(nu_m3_t a, nu_m3_t b);

NU_API nu_m4_t nu_m4_zero(void);
NU_API nu_m4_t nu_m4_identity(void);
NU_API nu_m4_t nu_m4_translate(nu_v3_t v);
NU_API nu_m4_t nu_m4_scale(nu_v3_t v);
NU_API nu_m4_t nu_m4_rotate_y(nu_f32_t angle);
NU_API nu_m4_t nu_m4_mul(nu_m4_t a, nu_m4_t b);
NU_API nu_v4_t nu_m4_mulv(nu_m4_t a, nu_v4_t v);
NU_API nu_v3_t nu_m4_mulv3(nu_m4_t a, nu_v3_t v);

NU_API nu_b2i_t  nu_b2i(nu_v2i_t min, nu_v2i_t max);
NU_API nu_b2i_t  nu_b2i_xywh(nu_i32_t x, nu_i32_t y, nu_u32_t w, nu_u32_t h);
NU_API nu_v2u_t  nu_b2i_size(nu_b2i_t b);
NU_API nu_b2i_t  nu_b2i_resize(nu_b2i_t b, nu_v2u_t size);
NU_API nu_v2i_t  nu_b2i_tr(nu_b2i_t b);
NU_API nu_v2i_t  nu_b2i_bl(nu_b2i_t b);
NU_API nu_b2i_t  nu_b2i_translate(nu_b2i_t b, nu_v2i_t t);
NU_API nu_b2i_t  nu_b2i_moveto(nu_b2i_t b, nu_v2i_t p);
NU_API nu_b2i_t  nu_b2i_union(nu_b2i_t a, nu_b2i_t b);
NU_API nu_bool_t nu_b2i_contains(nu_b2i_t b, nu_v2_t p);
NU_API nu_bool_t nu_b2i_containsi(nu_b2i_t b, nu_v2i_t p);
NU_API nu_v2_t   nu_b2i_normalize(nu_b2i_t b, nu_v2_t p);

NU_API nu_b3_t   nu_b3(nu_v3_t p, nu_v3_t s);
NU_API nu_v3_t   nu_b3_center(nu_b3_t b);
NU_API nu_bool_t nj_b3_contains(nu_b3_t b, nu_v3_t p);

NU_API nu_m4_t nu_perspective(nu_f32_t fov,
                              nu_f32_t aspect_ratio,
                              nu_f32_t z_near,
                              nu_f32_t z_far);
NU_API nu_m4_t nu_ortho(nu_f32_t left,
                        nu_f32_t right,
                        nu_f32_t bottom,
                        nu_f32_t top,
                        nu_f32_t near,
                        nu_f32_t far);
NU_API nu_m4_t nu_lookat(nu_v3_t eye, nu_v3_t center, nu_v3_t up);

NU_API nu_v3_t nu_axis3d(nu_f32_t  pos_x,
                         nu_f32_t  neg_x,
                         nu_f32_t  pos_y,
                         nu_f32_t  neg_y,
                         nu_f32_t  pos_z,
                         nu_f32_t  neg_z,
                         nu_bool_t normalize);

NU_API nu_v3_t nu_triangle_normal(nu_v3_t p0, nu_v3_t p1, nu_v3_t p2);

#endif

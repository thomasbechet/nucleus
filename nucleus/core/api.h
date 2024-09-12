#ifndef NU_CORE_API_H
#define NU_CORE_API_H

#include <nucleus/core/config.h>

//////////////////////////////////////////////////////////////////////////
//////                        Context                               //////
//////////////////////////////////////////////////////////////////////////

NU_API nu_error_t nu_init(void);
NU_API nu_error_t nu_terminate(void);

NU_API void nu__panic(const nu_char_t *source, const nu_char_t *format, ...);
NU_API void nu__vpanic(const nu_char_t *source,
                       const nu_char_t *format,
                       va_list          args);

#define nu_panic(...)

//////////////////////////////////////////////////////////////////////////
//////                          Table                               //////
//////////////////////////////////////////////////////////////////////////

NU_DEFINE_HANDLE(nu_table_t);

typedef enum
{
    NU_TABLE_INT,
    NU_TABLE_FLOAT,
} nu_table_type_t;

typedef struct
{
    const nu_char_t **names;
    nu_table_type_t  *types;
    nu_size_t         size;
} nu_table_info_t;

NU_API nu_table_t nu_table_create(const nu_table_info_t *info);
NU_API nu_table_t nu_table_delete(nu_table_t table);
NU_API nu_size_t  nu_table_size(nu_table_t table);
NU_API nu_u32_t  *nu_table_int(nu_table_t table, const nu_char_t *name);
NU_API float     *nu_table_float(nu_table_t table, const nu_char_t *name);

//////////////////////////////////////////////////////////////////////////
//////                         Logger                               //////
//////////////////////////////////////////////////////////////////////////

typedef enum
{
    NU_LOG_DEBUG   = 5,
    NU_LOG_INFO    = 4,
    NU_LOG_WARNING = 3,
    NU_LOG_ERROR   = 2,
} nu_log_level_t;

NU_API void nu_log(nu_log_level_t   level,
                   const nu_char_t *source,
                   const nu_char_t *format,
                   ...);
NU_API void nu_vlog(nu_log_level_t   level,
                    const nu_char_t *source,
                    const nu_char_t *format,
                    va_list          args);

#define nu_debug(...)   nu_log(NU_LOG_DEBUG, __SOURCE__, __VA_ARGS__)
#define nu_info(...)    nu_log(NU_LOG_INFO, __SOURCE__, __VA_ARGS__)
#define nu_warning(...) nu_log(NU_LOG_WARNING, __SOURCE__, __VA_ARGS__)
#define nu_error(...)   nu_log(NU_LOG_ERROR, __SOURCE__, __VA_ARGS__)

//////////////////////////////////////////////////////////////////////////
//////                         Memory                               //////
//////////////////////////////////////////////////////////////////////////

#define NU_DEFAULT_ALIGN 16

typedef struct
{
    void *(*callback)(void     *p, // pointer (NULL if malloc)
                      nu_size_t s, // current size (0 if malloc)
                      nu_size_t n, // new size (0 if free)
                      nu_size_t a, // alignment
                      void     *u);    // userdata
    void *userdata;
} nu_allocator_t;

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

//////////////////////////////////////////////////////////////////////////
//////                          Time                                //////
//////////////////////////////////////////////////////////////////////////

typedef struct
{
#if defined(NU_PLATFORM_WINDOWS)
    LARGE_INTEGER value;
#elif defined(NU_PLATFORM_UNIX)
    time_t value;
#endif
} nu_time_t;

typedef struct
{
#if defined(NU_PLATFORM_WINDOWS)
    LARGE_INTEGER t0;
#elif defined(NU_PLATFORM_UNIX)
    struct timespec start;
#endif
} nu_timer_t;

typedef struct
{
    nu_bool_t active;
    nu_u32_t  id;
    float     timestep;
    float     _acc;
} nu_fixed_loop_t;

NU_API nu_time_t nu_time(void);
NU_API nu_u32_t  nu_time_hours(const nu_time_t *time);
NU_API nu_u32_t  nu_time_minutes(const nu_time_t *time);
NU_API nu_u32_t  nu_time_seconds(const nu_time_t *time);

NU_API void  nu_timer_reset(nu_timer_t *timer);
NU_API float nu_timer_elapsed(nu_timer_t *timer);

NU_API nu_fixed_loop_t nu_fixed_loop(nu_u32_t id, float timestep);
NU_API nu_bool_t       nu_fixed_loop_next(nu_fixed_loop_t *loops,
                                          nu_size_t        count,
                                          nu_u32_t        *id);
NU_API void            nu_fixed_loop_update(nu_fixed_loop_t *loops,
                                            nu_size_t        count,
                                            float            dt);

//////////////////////////////////////////////////////////////////////////
//////                        String                                //////
//////////////////////////////////////////////////////////////////////////

#define NU_MATCH(a, b) (nu_strcmp(a, b) == 0)

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

typedef nu_u32_t nu_uid_t;

#define nu_uid(name) nu_hash(name)

#define NUEXT_PATH_MAX 256

typedef enum
{
    NUEXT_EXTENSION_GLTF,
    NUEXT_EXTENSION_PNG,
    NUEXT_EXTENSION_JPEG,
    NUEXT_EXTENSION_UNKNOWN,
} nuext_extension_t;

NU_API nuext_extension_t nuext_path_extension(const nu_char_t *filename);
NU_API void nuext_path_basename(const nu_char_t *path, nu_char_t *name);
NU_API void nuext_path_dirname(const nu_char_t *path, nu_char_t *name);
NU_API void nuext_path_concat(const nu_char_t *p1,
                              const nu_char_t *p2,
                              nu_char_t       *path);

//////////////////////////////////////////////////////////////////////////
//////                          Math                                //////
//////////////////////////////////////////////////////////////////////////

#define NU_PI 3.14159265359

#define nu_min(a, b)          (((a) < (b)) ? (a) : (b))
#define nu_max(a, b)          (((a) > (b)) ? (a) : (b))
#define nu_clamp(x, min, max) (nu_max(min, nu_min(max, x)))
#define nu_clamp01(x)         (nu_clamp(x, 0, 1))

#define NU_VEC2_SIZE 2
#define NU_VEC3_SIZE 3
#define NU_VEC4_SIZE 4
#define NU_QUAT_SIZE 4
#define NU_MAT3_SIZE 9
#define NU_MAT4_SIZE 16

#define NU_VEC2_ZERO nu_vec2(0, 0)
#define NU_VEC2_ONE  nu_vec2(1, 1)

#define NU_IVEC2_ZERO nu_ivec2(0, 0)

#define NU_UVEC2_ZERO nu_uvec2(0, 0)
#define NU_UVEC2_ONE  nu_uvec2(1, 1)

#define NU_VEC3_ZERO     nu_vec3(0, 0, 0)
#define NU_VEC3_ONE      nu_vec3(1, 1, 1)
#define NU_VEC3_UP       nu_vec3(0, 1, 0)
#define NU_VEC3_DOWN     nu_vec3(0, -1, 0)
#define NU_VEC3_FORWARD  nu_vec3(0, 0, -1)
#define NU_VEC3_BACKWARD nu_vec3(0, 0, 1)
#define NU_VEC3_LEFT     nu_vec3(-1, 0, 0)
#define NU_VEC3_RIGHT    nu_vec3(1, 0, 0)

#define NU_VEC4_ZERO nu_vec4(0, 0, 0, 0)

#define NU_VEC2_FORMAT "%lf %lf"
#define NU_VEC3_FORMAT "%lf %lf %lf"
#define NU_VEC4_FORMAT "%lf %lf %lf %lf"

typedef union
{
    struct
    {
        float x;
        float y;
    };
    float xy[NU_VEC2_SIZE];
} nu_vec2_t;

typedef union
{
    struct
    {
        float x;
        float y;
        float z;
    };
    float xyz[NU_VEC3_SIZE];
} nu_vec3_t;

typedef union
{
    struct
    {
        float x;
        float y;
        float z;
        float w;
    };
    float xyzw[NU_VEC4_SIZE];
} nu_vec4_t;

typedef union
{
    struct
    {
        nu_u32_t x;
        nu_u32_t y;
    };
    nu_u32_t xy[NU_VEC2_SIZE];
} nu_uvec2_t;

typedef union
{
    struct
    {
        nu_i32_t x;
        nu_i32_t y;
    };
    nu_i32_t xy[NU_VEC2_SIZE];
} nu_ivec2_t;

typedef union
{
    struct
    {
        nu_u32_t x;
        nu_u32_t y;
        nu_u32_t z;
        nu_u32_t w;
    };
    nu_u32_t xyzw[NU_VEC4_SIZE];
} nu_uvec4_t;

typedef union
{
    struct
    {
        float x;
        float y;
        float z;
        float w;
    };
    float xyzw[NU_QUAT_SIZE];
} nu_quat_t;

typedef union
{

    struct
    {
        float x1;
        float x2;
        float x3;
        float y1;
        float y2;
        float y3;
        float z1;
        float z2;
        float z3;
    };
    float data[NU_MAT3_SIZE];
} nu_mat3_t;

typedef union
{
    struct
    {
        float x1;
        float x2;
        float x3;
        float x4;
        float y1;
        float y2;
        float y3;
        float y4;
        float z1;
        float z2;
        float z3;
        float z4;
        float w1;
        float w2;
        float w3;
        float w4;
    };
    float data[NU_MAT4_SIZE];
} nu_mat4_t;

typedef struct
{
    nu_ivec2_t p;
    nu_uvec2_t s;
} nu_rect_t;

NU_API nu_bool_t nu_is_power_of_two(nu_size_t n);
NU_API nu_size_t nu_log2(nu_size_t n);
NU_API float     nu_fabs(float f);
NU_API float     nu_floor(float f);
NU_API float     nu_radian(float d);
NU_API float     nu_sqrt(float x);
NU_API float     nu_pow(float b, float e);
NU_API float     nu_cos(float x);
NU_API float     nu_sin(float x);
NU_API float     nu_tan(float x);

NU_API nu_vec2_t nu_vec2(float x, float y);
NU_API nu_vec2_t nu_vec2_add(nu_vec2_t a, nu_vec2_t b);
NU_API nu_vec2_t nu_vec2_sub(nu_vec2_t a, nu_vec2_t b);
NU_API nu_vec2_t nu_vec2_mul(nu_vec2_t a, nu_vec2_t b);
NU_API nu_vec2_t nu_vec2_muls(nu_vec2_t a, float s);
NU_API nu_vec2_t nu_vec2_div(nu_vec2_t a, nu_vec2_t b);
NU_API nu_vec2_t nu_vec2_divs(nu_vec2_t a, float s);
NU_API nu_vec2_t nu_vec2_floor(nu_vec2_t a);

NU_API nu_vec3_t nu_vec3(float x, float y, float z);
NU_API nu_vec3_t nu_vec3_add(nu_vec3_t a, nu_vec3_t b);
NU_API nu_vec3_t nu_vec3_sub(nu_vec3_t a, nu_vec3_t b);
NU_API nu_vec3_t nu_vec3_mul(nu_vec3_t a, nu_vec3_t b);
NU_API nu_vec3_t nu_vec3_muls(nu_vec3_t a, float s);
NU_API nu_vec3_t nu_vec3_div(nu_vec3_t a, nu_vec3_t b);
NU_API nu_vec3_t nu_vec3_divs(nu_vec3_t a, float s);
NU_API float     nu_vec3_norm(nu_vec3_t a);
NU_API nu_vec3_t nu_vec3_normalize(nu_vec3_t a);
NU_API nu_vec3_t nu_vec3_cross(nu_vec3_t a, nu_vec3_t b);
NU_API float     nu_vec3_dot(nu_vec3_t a, nu_vec3_t b);

NU_API nu_vec4_t nu_vec4(float x, float y, float z, float w);
NU_API float     nu_vec4_dot(nu_vec4_t a, nu_vec4_t b);
NU_API float     nu_vec4_norm(nu_vec4_t v);
NU_API nu_vec2_t nu_vec4_xy(nu_vec4_t v);
NU_API nu_vec2_t nu_vec4_zw(nu_vec4_t v);

NU_API nu_ivec2_t nu_ivec2(nu_i32_t x, nu_i32_t y);

NU_API nu_uvec2_t nu_uvec2(nu_u32_t x, nu_u32_t y);
NU_API nu_uvec2_t nu_uvec2_min(nu_uvec2_t a, nu_uvec2_t b);

NU_API nu_uvec4_t nu_uvec4(nu_u32_t x, nu_u32_t y, nu_u32_t z, nu_u32_t w);

NU_API nu_quat_t nu_quat(float x, float y, float z, float w);
NU_API nu_quat_t nu_quat_identity(void);
NU_API nu_vec4_t nu_quat_vec4(nu_quat_t a);
NU_API float     nu_quat_norm(nu_quat_t a);
NU_API nu_quat_t nu_quat_axis(nu_vec3_t axis, float angle);
NU_API nu_quat_t nu_quat_mul(nu_quat_t a, nu_quat_t b);
NU_API nu_vec3_t nu_quat_mulv3(nu_quat_t a, nu_vec3_t v);
NU_API nu_quat_t nu_quat_mul_axis(nu_quat_t q, nu_vec3_t axis, float angle);
NU_API nu_mat3_t nu_quat_mat3(nu_quat_t q);
NU_API nu_mat4_t nu_quat_mat4(nu_quat_t q);
NU_API nu_mat4_t nu_quat_mulm4(nu_quat_t a, nu_mat4_t m);

NU_API nu_mat3_t nu_mat3_zero(void);
NU_API nu_mat3_t nu_mat3_identity(void);
NU_API nu_mat3_t nu_mat3_translate(float x, float y);
NU_API nu_mat3_t nu_mat3_scale(float x, float y);
NU_API nu_mat3_t nu_mat3_mul(nu_mat3_t a, nu_mat3_t b);

NU_API nu_mat4_t nu_mat4_zero(void);
NU_API nu_mat4_t nu_mat4_identity(void);
NU_API nu_mat4_t nu_mat4_translate(float x, float y, float z);
NU_API nu_mat4_t nu_mat4_scale(float x, float y, float z);
NU_API nu_mat4_t nu_mat4_rotate_y(float angle);
NU_API nu_mat4_t nu_mat4_mul(nu_mat4_t a, nu_mat4_t b);
NU_API nu_vec4_t nu_mat4_mulv(nu_mat4_t a, nu_vec4_t v);
NU_API nu_vec3_t nu_mat4_mulv3(nu_mat4_t a, nu_vec3_t v);

NU_API nu_rect_t nu_rect(nu_i32_t x, nu_i32_t y, nu_u32_t w, nu_u32_t h);
NU_API nu_bool_t nu_rect_contains(nu_rect_t r, nu_vec2_t p);
NU_API nu_bool_t nu_rect_containsi(nu_rect_t r, nu_ivec2_t p);
NU_API nu_vec2_t nu_rect_normalize(nu_rect_t r, nu_vec2_t p);

NU_API nu_mat4_t nu_perspective(float fov,
                                float aspect_ratio,
                                float z_near,
                                float z_far);
NU_API nu_mat4_t nu_lookat(nu_vec3_t eye, nu_vec3_t center, nu_vec3_t up);

//////////////////////////////////////////////////////////////////////////
//////                          Color                               //////
//////////////////////////////////////////////////////////////////////////

#define NU_COLOR_WHITE    nu_color(255, 255, 255, 0)
#define NU_COLOR_BLACK    nu_color(0, 0, 0, 0)
#define NU_COLOR_RED      nu_color(255, 0, 0, 0)
#define NU_COLOR_BLUE_SKY nu_color(52, 181, 248, 0)

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

NU_API nu_color_t nu_color(nu_u8_t r, nu_u8_t g, nu_u8_t b, nu_u8_t a);
NU_API nu_vec4_t  nu_color_to_vec4(nu_color_t c);
NU_API nu_color_t nu_color_from_vec4(nu_vec4_t v);
NU_API nu_color_t nu_color_to_srgb(nu_color_t c);
NU_API nu_color_t nu_color_to_linear(nu_color_t c);

//////////////////////////////////////////////////////////////////////////
//////                         Vector                               //////
//////////////////////////////////////////////////////////////////////////

#ifdef NU_CXX
#define NU_VOID_CAST(type, expr) (static_cast<decltype(type)>(expr))
#else
#define NU_VOID_CAST(type, expr) (expr)
#endif

#define nu_vec(type)               \
    struct                         \
    {                              \
        type           *data;      \
        nu_size_t       size;      \
        nu_size_t       capacity;  \
        nu_allocator_t *allocator; \
    }

#define nu_vec_init(cap, v) nu_vec_init_a(nu_allocator_core(), cap, v)

#define nu_vec_init_a(alloc, cap, v)                                     \
    do                                                                   \
    {                                                                    \
        (v)->data = NU_VOID_CAST(                                        \
            (v)->data, nu_alloc_a((alloc), sizeof(*(v)->data) * (cap))); \
        (v)->capacity  = (cap);                                          \
        (v)->size      = 0;                                              \
        (v)->allocator = (alloc);                                        \
    } while (0)

#define nu_vec_free(v) \
    nu_free_a((v)->allocator, (v)->data, sizeof(*(v)->data) * (v)->capacity)

#define nu_vec_clear(v) ((v)->size = 0)

#define nu_vec_push(v)                                          \
    (((v)->data = NU_VOID_CAST((v)->data,                       \
                               nu__vec_push((v)->allocator,     \
                                            sizeof(*(v)->data), \
                                            (v)->data,          \
                                            &(v)->size,         \
                                            &(v)->capacity)))   \
         ? (v)->data + ((v)->size - 1)                          \
         : NU_NULL)

#define nu_vec_pop(v) \
    (nu__vec_pop(&(v)->size) ? ((v)->data + (v)->size) : NU_NULL)

#define nu_vec_resize(v, new_size)                               \
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

#define nu_vec_last(v) ((v)->size ? (v)->data + ((v)->size - 1) : NU_NULL)

NU_API void     *nu__vec_push(nu_allocator_t *alloc,
                              nu_size_t       tsize,
                              void           *data,
                              nu_size_t      *size,
                              nu_size_t      *capacity);
NU_API nu_bool_t nu__vec_pop(nu_size_t *size);

typedef nu_vec(nu_bool_t) nu_bool_vec_t;
typedef nu_vec(nu_u32_t) nu_u32_vec_t;
typedef nu_vec(nu_size_t) nu_size_vec_t;
typedef nu_vec(nu_vec2_t) nu_vec2_vec_t;
typedef nu_vec(nu_vec3_t) nu_vec3_vec_t;

//////////////////////////////////////////////////////////////////////////
//////                           Pool                               //////
//////////////////////////////////////////////////////////////////////////

#define nu_pool(type)            \
    struct                       \
    {                            \
        type         *data;      \
        nu_size_t     capacity;  \
        nu_size_vec_t _freelist; \
    }

#define nu_pool_init(cap, p) nu_pool_init_a(nu_allocator_core(), cap, p)

#define nu_pool_init_a(alloc, cap, p)                                    \
    do                                                                   \
    {                                                                    \
        (p)->data = NU_VOID_CAST(                                        \
            (p)->data, nu_alloc_a((alloc), sizeof(*(p)->data) * (cap))); \
        nu_memset((p)->data, 0, sizeof(*(p)->data) * (cap));             \
        (p)->capacity = (cap);                                           \
        nu_vec_init_a(alloc, (cap), &(p)->_freelist);                    \
        for (nu_size_t i = 0; i < (cap); ++i)                            \
        {                                                                \
            *nu_vec_push(&(p)->_freelist) = (cap) - i - 1;               \
        }                                                                \
    } while (0)

#define nu_pool_free(p)                                \
    do                                                 \
    {                                                  \
        nu_free_a((p)->_freelist.allocator,            \
                  (p)->data,                           \
                  sizeof(*(p)->data) * (p)->capacity); \
        nu_vec_free(&(p)->_freelist);                  \
    } while (0)

#define nu_pool_add(p, pindex)                                  \
    (((p)->data = NU_VOID_CAST((p)->data,                       \
                               nu__pool_add(sizeof(*(p)->data), \
                                            (p)->data,          \
                                            &(p)->capacity,     \
                                            &(p)->_freelist,    \
                                            (pindex))))         \
         ? (p)->data + (*(pindex))                              \
         : NU_NULL)

#define nu_pool_remove(s, index) (*nu_vec_push_checked(&(s)->_freelist) = index)

NU_API void *nu__pool_add(nu_size_t      tsize,
                          void          *data,
                          nu_size_t     *capacity,
                          nu_size_vec_t *freelist,
                          nu_size_t     *pindex);

#endif

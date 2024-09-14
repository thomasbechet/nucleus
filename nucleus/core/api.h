#ifndef NU_CORE_API_H
#define NU_CORE_API_H

#include <nucleus/core/config.h>

//////////////////////////////////////////////////////////////////////////
//////                        Context                               //////
//////////////////////////////////////////////////////////////////////////

NU_API nu_error_t nu_init(void);
NU_API void       nu_terminate(void);

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
//////                          Color                               //////
//////////////////////////////////////////////////////////////////////////

#define NU_COLOR_WHITE    nu_color(255, 255, 255, 0)
#define NU_COLOR_BLACK    nu_color(0, 0, 0, 0)
#define NU_COLOR_RED      nu_color(255, 0, 0, 0)
#define NU_COLOR_BLUE_SKY nu_color(52, 181, 248, 0)

typedef nu_u32_t nu_color_t;

NU_API nu_color_t nu_color(nu_u8_t r, nu_u8_t g, nu_u8_t b, nu_u8_t a);
// NU_API nu_vec4_t  nu_color_to_vec4(nu_color_t c);
// NU_API nu_color_t nu_color_from_vec4(nu_vec4_t v);
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

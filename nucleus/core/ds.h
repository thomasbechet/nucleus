#ifndef NU_DS_H
#define NU_DS_H

#include <nucleus/core/memory.h>

//////////////////////////////////////////////////////////////////////////
//////                           Vector                             //////
//////////////////////////////////////////////////////////////////////////

#define nu_vec(type)        \
    struct                  \
    {                       \
        type     *data;     \
        nu_size_t size;     \
        nu_size_t capacity; \
    }

#define nu_vec_init(v, alloc, cap)                                   \
    do                                                               \
    {                                                                \
        (v)->data     = nu_alloc(alloc, sizeof(*(v)->data) * (cap)); \
        (v)->capacity = (cap);                                       \
        (v)->size     = 0;                                           \
    } while (0)

#define nu_vec_free(v, alloc) \
    nu_free(alloc, (v)->data, sizeof(*(v)->data) * (v)->capacity)

#define nu_vec_clear(v) ((v)->size = 0)

#define nu_vec_push(v, alloc)                                                \
    (((v)->data = nu__vec_push(                                              \
          alloc, sizeof(*(v)->data), (v)->data, &(v)->size, &(v)->capacity)) \
         ? (v)->data + ((v)->size - 1)                                       \
         : NU_NULL)

#define nu_vec_pop(v) \
    (nu__vec_pop(&(v)->size) ? (v)->data + (v)->size : NU_NULL)

#define nu_vec_last(v) ((v)->size ? (v)->data + ((v)->size - 1) : NU_NULL)

NU_API void     *nu__vec_push(nu_allocator_t alloc,
                              nu_size_t      tsize,
                              void          *data,
                              nu_size_t     *size,
                              nu_size_t     *capacity);
NU_API nu_bool_t nu__vec_pop(nu_size_t *size);

typedef nu_vec(nu_bool_t) nu_bool_vec_t;
typedef nu_vec(nu_u32_t) nu_u32_vec_t;

//////////////////////////////////////////////////////////////////////////
//////                           Pool                               //////
//////////////////////////////////////////////////////////////////////////

#define nu_pool(type)           \
    struct                      \
    {                           \
        type        *data;      \
        nu_u32_t     capacity;  \
        nu_u32_vec_t _freelist; \
    }

#define nu_pool_init(p, alloc, cap)                                  \
    do                                                               \
    {                                                                \
        (p)->data     = nu_alloc(alloc, sizeof(*(p)->data) * (cap)); \
        (p)->capacity = cap;                                         \
        nu_vec_init(&(p)->_freelist, alloc, (cap));                  \
    } while (0)

#define nu_pool_free(p, alloc)                                         \
    do                                                                 \
    {                                                                  \
        nu_free(alloc, (p)->data, sizeof(*(p)->data) * (p)->capacity); \
        nu_vec_free(&(p)->_freelist, alloc);                           \
    } while (0)

#define nu_pool_add(s, alloc, pindex)             \
    ((s)->data = nu__pool_add(alloc,              \
                              sizeof(*(s)->data), \
                              (s)->data,          \
                              &(s)->capacity,     \
                              &(s)->_freelist,    \
                              pindex)             \
                     ? (s)->data + (*pindex)      \
                     : NU_NULL)

#define nu_pool_remove(s, index) (*nu_vec_push_checked(&(s)->_freelist) = index)

NU_API void *nu__pool_add(nu_allocator_t alloc,
                          nu_size_t      tsize,
                          void          *data,
                          nu_u32_t      *capacity,
                          nu_u32_vec_t  *freelist,
                          nu_u32_t      *pindex);

#endif

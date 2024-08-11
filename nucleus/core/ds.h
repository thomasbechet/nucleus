#ifndef NU_DS_H
#define NU_DS_H

#include <nucleus/core/memory.h>

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
#define nu_vec_clear(v) (v)->size = 0
#define nu_vec_push(v, alloc)                                                       \
    do                                                                              \
    {                                                                               \
        if ((v)->size >= (v)->capacity)                                             \
        {                                                                           \
            nu_size_t new_capacity = (v)->capacity * 2;                             \
            (v)->data              = nu_realloc(alloc,                              \
                                   (v)->data,                          \
                                   sizeof(*(v)->data) * (v)->capacity, \
                                   sizeof(*(v)->data) * new_capacity); \
            (v)->capacity          = new_capacity;                                  \
        }                                                                           \
        ++(v)->size;                                                                \
    } while (0)
#define nu_vec_last(v) (v)->size ? (v)->data + ((v)->size - 1) : NU_NULL

typedef nu_vec(nu_u32_t) nu_u32_vec_t;

#endif

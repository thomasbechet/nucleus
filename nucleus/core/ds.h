#ifndef NU_DS_H
#define NU_DS_H

#include <nucleus/core/memory.h>

typedef struct
{
    nu_size_t _size;
    nu_size_t _capacity;
    void     *_data;
} nu_vector_t;

NU_API nu_error_t  nu_vector_init(nu_vector_t    *vec,
                                  nu_allocator_t *alloc,
                                  nu_size_t       capacity,
                                  nu_size_t       osize);
NU_API void        nu_vector_free(nu_vector_t    *vec,
                                  nu_allocator_t *alloc,
                                  nu_size_t       osize);
NU_API void       *nu_vector_data(nu_vector_t *vec);
NU_API const void *nu_vector_data_const(const nu_vector_t *vec);
NU_API nu_size_t   nu_vector_size(const nu_vector_t *vec);
NU_API void        nu_vector_push(nu_vector_t    *vec,
                                  nu_allocator_t *alloc,
                                  nu_size_t       osize);

#define NU_DEFINE_VECTOR(name, type)                               \
    typedef struct                                                 \
    {                                                              \
        nu_vector_t vec;                                           \
    } name##_t;                                                    \
    nu_error_t name##_init(                                        \
        name##_t *vec, nu_allocator_t *alloc, nu_size_t size);     \
    void        name##_free(name##_t *vec, nu_allocator_t *alloc); \
    type       *name##_data(name##_t *vec);                        \
    const type *name##_data_const(const name##_t *vec);            \
    type       *name##_at(name##_t *vec, nu_size_t i);             \
    const type *name##_at_const(const name##_t *vec, nu_size_t i); \
    nu_size_t   name##_size(const name##_t *vec);                  \
    type       *name##_push(name##_t *vec, nu_allocator_t *alloc);

#define NU_DECLARE_VECTOR(name, type)                                \
    nu_error_t name##_init(                                          \
        name##_t *vec, nu_allocator_t *alloc, nu_size_t size)        \
    {                                                                \
        return nu_vector_init(&vec->vec, alloc, size, sizeof(type)); \
    }                                                                \
    void name##_free(name##_t *vec, nu_allocator_t *alloc)           \
    {                                                                \
        nu_vector_free(&vec->vec, alloc, sizeof(type));              \
    }                                                                \
    type *name##_data(name##_t *vec)                                 \
    {                                                                \
        return nu_vector_data(&vec->vec);                            \
    }                                                                \
    const type *name##_data_const(const name##_t *vec)               \
    {                                                                \
        return nu_vector_data_const(&vec->vec);                      \
    }                                                                \
    type *name##_at(name##_t *vec, nu_size_t i)                      \
    {                                                                \
        return name##_data(vec) + i;                                 \
    }                                                                \
    const type *name##_at_const(const name##_t *vec, nu_size_t i)    \
    {                                                                \
        return name##_data_const(vec) + i;                           \
    }                                                                \
    nu_size_t name##_size(const name##_t *vec)                       \
    {                                                                \
        return nu_vector_size(&vec->vec);                            \
    }                                                                \
    type *name##_push(name##_t *vec, nu_allocator_t *alloc)          \
    {                                                                \
        nu_vector_push(&vec->vec, alloc, sizeof(type));              \
        return name##_at(vec, vec->vec._size - 1);                   \
    }

#endif

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
                                  nu_size_t       size,
                                  nu_size_t       osize);
NU_API void        nu_vector_free(nu_vector_t    *vec,
                                  nu_allocator_t *alloc,
                                  nu_size_t       osize);
NU_API void       *nu_vector_data(nu_vector_t *vec);
NU_API const void *nu_vector_data_const(const nu_vector_t *vec);
NU_API nu_size_t   nu_vector_size(const nu_vector_t *vec);

#define NU_DEFINE_VECTOR(name, type)                              \
    typedef struct                                                \
    {                                                             \
        nu_array_t ar;                                            \
    } name##_t;                                                   \
    nu_error_t name##_init(                                       \
        name##_t *ar, nu_allocator_t *alloc, nu_size_t size);     \
    void        name##_free(name##_t *ar, nu_allocator_t *alloc); \
    type       *name##_data(name##_t *ar);                        \
    const type *name##_data_const(const name##_t *ar);            \
    type       *name##_at(name##_t *ar, nu_size_t i);             \
    const type *name##_at_const(const name##_t *ar, nu_size_t i); \
    nu_size_t   name##_size(const name##_t *ar);

#define NU_DECLARE_ARRAY(name, type)                              \
    nu_error_t name##_init(                                       \
        name##_t *ar, nu_allocator_t *alloc, nu_size_t size)      \
    {                                                             \
        return nu_array_init(&ar->ar, alloc, size, sizeof(type)); \
    }                                                             \
    void name##_free(name##_t *ar, nu_allocator_t *alloc)         \
    {                                                             \
        nu_array_free(&ar->ar, alloc, sizeof(type));              \
    }                                                             \
    type *name##_data(name##_t *ar)                               \
    {                                                             \
        return nu_array_data(&ar->ar);                            \
    }                                                             \
    const type *name##_data_const(const name##_t *ar)             \
    {                                                             \
        return nu_array_data_const(&ar->ar);                      \
    }                                                             \
    type *name##_at(name##_t *ar, nu_size_t i)                    \
    {                                                             \
        return name##_data(ar) + i;                               \
    }                                                             \
    const type *name##_at_const(const name##_t *ar, nu_size_t i)  \
    {                                                             \
        return name##_data_const(ar) + i;                         \
    }                                                             \
    nu_size_t name##_size(const name##_t *ar)                     \
    {                                                             \
        return nu_array_size(&ar->ar);                            \
    }

#endif

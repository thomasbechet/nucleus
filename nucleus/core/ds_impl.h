#ifndef NU_DS_IMPL_H
#define NU_DS_IMPL_H

#include <nucleus/core/ds.h>

inline void *
nu__vec_push (nu_allocator_t *alloc,
              nu_size_t       tsize,
              void           *data,
              nu_size_t      *size,
              nu_size_t      *capacity)
{
    NU_ASSERT(data && size && capacity);
    if ((*size) >= (*capacity))
    {
        nu_size_t new_capacity = (*capacity) * 2;
        data                   = nu_realloc(
            alloc, data, tsize * (*capacity), tsize * new_capacity);
        (*capacity) = new_capacity;
    }
    ++(*size);
    return data;
}
inline nu_size_t
nu__vec_pop (nu_size_t *size)
{
    if ((*size))
    {
        --(*size);
    }
    return (*size);
}

#endif

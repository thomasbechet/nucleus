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
    nu_assert(data && size && capacity);
    if ((*size) >= (*capacity))
    {
        nu_size_t new_capacity = (*capacity) * 2;
        data                   = nu_realloc_a(
            alloc, data, tsize * (*capacity), tsize * new_capacity);
        (*capacity) = new_capacity;
    }
    ++(*size);
    return data;
}
inline nu_bool_t
nu__vec_pop (nu_size_t *size)
{
    if (*size)
    {
        --(*size);
        return NU_TRUE;
    }
    return NU_FALSE;
}

inline void *
nu__pool_add (nu_size_t      tsize,
              void          *data,
              nu_size_t     *capacity,
              nu_size_vec_t *freelist,
              nu_size_t     *pindex)
{
    nu_assert(data && capacity && freelist && pindex);
    if (freelist->size)
    {
        *pindex = *nu_vec_pop(freelist);
        return data;
    }

    nu_size_t new_capacity = (*capacity) * 2;
    data                   = nu_realloc_a(
        freelist->allocator, data, tsize * (*capacity), tsize * new_capacity);
    nu_memset((void *)((nu_size_t)data + tsize * (*capacity)),
              0,
              (new_capacity - (*capacity)) * tsize);

    for (nu_size_t i = 0; i < (new_capacity - *capacity - 1); ++i)
    {
        *nu_vec_push(freelist) = new_capacity - i - 1;
    }

    *pindex   = *capacity;
    *capacity = new_capacity;

    return data;
}

#endif

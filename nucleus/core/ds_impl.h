#ifndef NU_DS_IMPL_H
#define NU_DS_IMPL_H

#include <nucleus/core/ds.h>

nu_error_t
nu_vector_init (nu_vector_t    *vec,
                nu_allocator_t *alloc,
                nu_size_t       capacity,
                nu_size_t       osize)
{
    vec->_capacity = capacity;
    vec->_size     = 0;
    vec->_data     = nu_alloc(alloc, capacity * osize);
    if (!vec->_data)
    {
        return NU_ERROR_ALLOCATION;
    }
    return NU_ERROR_NONE;
}
void
nu_vector_free (nu_vector_t *vec, nu_allocator_t *alloc, nu_size_t osize)
{
    nu_free(alloc, vec->_data, vec->_size * osize);
}
void *
nu_vector_data (nu_vector_t *vec)
{
    return vec->_data;
}
const void *
nu_vector_data_const (const nu_vector_t *vec)
{
    return vec->_data;
}
nu_size_t
nu_vector_size (const nu_vector_t *vec)
{
    return vec->_size;
}
void
nu_vector_push (nu_vector_t *vec, nu_allocator_t *alloc, nu_size_t osize)
{
    if (vec->_size >= vec->_capacity)
    {
        vec->_capacity *= 2;
        vec->_data = nu_realloc(
            alloc, vec->_data, osize * vec->_capacity, osize * vec->_capacity);
    }
    ++vec->_size;
}

#endif

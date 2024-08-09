#ifndef NU_DS_IMPL_H
#define NU_DS_IMPL_H

#include <nucleus/core/ds.h>

nu_error_t
nu_array_init (nu_array_t     *ar,
               nu_allocator_t *alloc,
               nu_size_t       size,
               nu_size_t       osize)
{
    ar->_size = size;
    ar->_data = nu_alloc(alloc, size * osize);
    if (!ar->_data)
    {
        return NU_ERROR_ALLOCATION;
    }
    return NU_ERROR_NONE;
}
void
nu_array_free (nu_array_t *ar, nu_allocator_t *alloc, nu_size_t osize)
{
    nu_free(alloc, ar->_data, ar->_size * osize);
}
void *
nu_array_data (nu_array_t *ar)
{
    return ar->_data;
}
const void *
nu_array_data_const (const nu_array_t *ar)
{
    return ar->_data;
}
nu_size_t
nu_array_size (const nu_array_t *ar)
{
    return ar->_size;
}

#endif

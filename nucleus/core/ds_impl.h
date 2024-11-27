#ifndef NU_DS_IMPL_H
#define NU_DS_IMPL_H

#include <nucleus/internal.h>

// inline void *
// nu__vec_push (nu_allocator_t *alloc,
//               nu_size_t       tsize,
//               void           *data,
//               nu_size_t      *size,
//               nu_size_t      *capacity)
// {
//     NU_ASSERT(data && size && capacity);
//     if ((*size) >= (*capacity))
//     {
//         nu_size_t new_capacity = (*capacity) * 2;
//         data                   = nu_realloc_a(
//             alloc, data, tsize * (*capacity), tsize * new_capacity);
//         (*capacity) = new_capacity;
//     }
//     ++(*size);
//     return data;
// }
// inline nu_bool_t
// nu__vec_pop (nu_size_t *size)
// {
//     if (*size)
//     {
//         --(*size);
//         return NU_TRUE;
//     }
//     return NU_FALSE;
// }

// inline void *
// nu__pool_add (nu_size_t  tsize,
//               void      *data,
//               nu_size_t *capacity,
//               NU_VEC(nu_size_t) * freelist,
//               nu_size_t *pindex)
// {
//     NU_ASSERT(data && capacity && freelist && pindex);
//     if (freelist->size)
//     {
//         nu_size_t *pop = NU_VEC_POP(freelist);
//         *pindex        = *pop;
//         return data;
//     }
//
//     nu_size_t new_capacity = (*capacity) * 2;
//     data                   = nu_realloc_a(
//         freelist->allocator, data, tsize * (*capacity), tsize *
//         new_capacity);
//     nu_memset((void *)((nu_intptr_t)data + tsize * (*capacity)),
//               0,
//               (new_capacity - (*capacity)) * tsize);
//
//     for (nu_size_t i = 0; i < (new_capacity - *capacity - 1); ++i)
//     {
//         nu_size_t *push = NU_VEC_PUSH(freelist);
//         NU_CHECK(push, return NU_NULL);
//         *push = new_capacity - i - 1;
//     }
//
//     *pindex   = *capacity;
//     *capacity = new_capacity;
//
//     return data;
// }

#endif

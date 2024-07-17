#ifndef NU_ALLOCATOR_H
#define NU_ALLOCATOR_H

#include <nucleus/types.h>

#define NU_DEFAULT_ALIGN 16

NU_API void *nu_alloc(nu_allocator_t alloc, nu_size_t s);
NU_API void  nu_free(nu_allocator_t alloc, void *ptr);

#ifdef NU_IMPLEMENTATION

void *
nu_alloc (nu_allocator_t alloc, nu_size_t s)
{
    return alloc.callback(NU_NULL, s, NU_DEFAULT_ALIGN, alloc.userdata);
}
void
nu_free (nu_allocator_t alloc, void *ptr)
{
    alloc.callback(ptr, 0, NU_DEFAULT_ALIGN, alloc.userdata);
}

#if defined(NU_STDLIB) && !defined(NU_ALLOC)
#include <stdlib.h>

static void *
nu__stdlib_alloctor_callback (void     *ptr,
                              nu_size_t size,
                              nu_size_t align,
                              void     *userdata)
{
    if (size)
    {
        return malloc(size);
    }
    else
    {
        free(ptr);
        return NU_NULL;
    }
}

#define NU_ALLOC                                                      \
    (nu_allocator_t)                                                  \
    {                                                                 \
        .callback = nu__stdlib_alloctor_callback, .userdata = NU_NULL \
    }
#endif

#endif

#endif

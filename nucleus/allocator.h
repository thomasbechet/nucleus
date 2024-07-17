#ifndef NU_ALLOCATOR_H
#define NU_ALLOCATOR_H

#include <nucleus/types.h>

#define NU_DEFAULT_ALIGN 16

NU_API void *nu_alloc(nu_allocator_t alloc, nu_size_t s);
NU_API void *nu_realloc(nu_allocator_t alloc,
                        void          *ptr,
                        nu_size_t      s,
                        nu_size_t      n);
NU_API void  nu_free(nu_allocator_t alloc, void *ptr, nu_size_t s);

#ifdef NU_IMPLEMENTATION

void *
nu_alloc (nu_allocator_t alloc, nu_size_t s)
{
    return alloc.callback(NU_NULL, 0, s, NU_DEFAULT_ALIGN, alloc.userdata);
}
void *
nu_realloc (nu_allocator_t alloc, void *ptr, nu_size_t s, nu_size_t n)
{
    if (s == n)
    {
        return ptr;
    }
    return alloc.callback(ptr, s, n, NU_DEFAULT_ALIGN, alloc.userdata);
}
void
nu_free (nu_allocator_t alloc, void *ptr, nu_size_t s)
{
    alloc.callback(ptr, s, 0, NU_DEFAULT_ALIGN, alloc.userdata);
}

#if defined(NU_STDLIB) && !defined(NU_ALLOC)
#include <stdlib.h>

static void *
nu__stdlib_alloctor_callback (
    void *p, nu_size_t s, nu_size_t n, nu_size_t a, void *u)
{
    (void)s;
    (void)a;
    (void)u;
    if (p && n)
    {
        return realloc(p, n);
    }
    else if (n)
    {
        return malloc(n);
    }
    else
    {
        free(p);
        return NU_NULL;
    }
}

#define NU_STDLIB_ALLOCATOR                                           \
    (nu_allocator_t)                                                  \
    {                                                                 \
        .callback = nu__stdlib_alloctor_callback, .userdata = NU_NULL \
    }
#endif

#endif

#endif

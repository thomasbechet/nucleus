#ifndef NU_MEMORY_IMPL_H
#define NU_MEMORY_IMPL_H

#include <nucleus/core/memory.h>

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

#ifdef NU_STDLIB
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

void
nuext_stdlib_allocator_init (nu_allocator_t *alloc)
{
    alloc->callback = nu__stdlib_alloctor_callback;
    alloc->userdata = NU_NULL;
}

#endif

void *
nu_memset (void *dst, nu_word_t c, nu_size_t n)
{
    if (n != 0)
    {
        unsigned char *d = dst;
        do
        {
            *d++ = (unsigned char)c;
        } while (--n != 0);
    }
    return (dst);
}

void
nu_memcpy (void *dst, const void *src, nu_size_t n)
{
    nu_size_t      i;
    nu_u8_t       *u8_dst = dst;
    const nu_u8_t *u8_src = src;

    for (i = 0; i < n; ++i)
    {
        u8_dst[i] = u8_src[i];
    }
}

void *
nu_memalign (void *ptr, nu_size_t align)
{
    NU_ASSERT(align > 0);
    return (void *)(((nu_size_t)ptr + align - 1) & ~(align - 1));
}

#endif

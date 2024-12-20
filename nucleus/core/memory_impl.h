#ifndef NU_MEMORY_IMPL_H
#define NU_MEMORY_IMPL_H

#include <nucleus/internal.h>

#ifdef NU_STDLIB

static void *
nu__malloc (nu_size_t n)
{
    return malloc(n);
}
static void
nu__free (void *p)
{
    free(p);
}

#endif

nu_int_t
nu_memcmp (const void *p0, const void *p1, nu_size_t n)
{
    const nu_byte_t *b0 = p0;
    const nu_byte_t *b1 = p1;
    while (n-- > 0)
    {
        if (*b0++ != *b1++)
        {
            return b0[-1] < b1[-1] ? -1 : 1;
        }
    }
    return 0;
}
void *
nu_memset (void *dst, nu_word_t c, nu_size_t n)
{
    if (n != 0)
    {
        nu_byte_t *d = (nu_byte_t *)dst;
        do
        {
            *d++ = (nu_byte_t)c;
        } while (--n != 0);
    }
    return (dst);
}
void
nu_memcpy (void *dst, const void *src, nu_size_t n)
{
    nu_size_t        i;
    nu_byte_t       *u8_dst = (nu_byte_t *)dst;
    const nu_byte_t *u8_src = (const nu_byte_t *)src;

    for (i = 0; i < n; ++i)
    {
        u8_dst[i] = u8_src[i];
    }
}
void
nu_memswp (void *a, void *b, nu_size_t n)
{
    nu_byte_t *a_swap = (nu_byte_t *)a, *b_swap = (nu_byte_t *)b;
    nu_byte_t *a_end = a + n;

    while (a_swap < a_end)
    {
        nu_byte_t temp = *a_swap;
        *a_swap        = *b_swap;
        *b_swap        = temp;
        a_swap++, b_swap++;
    }
}
void *
nu_memalign (void *ptr, nu_size_t align)
{
    NU_ASSERT(align > 0);
    return (void *)(((nu_intptr_t)ptr + align - 1) & ~(align - 1));
}

#endif

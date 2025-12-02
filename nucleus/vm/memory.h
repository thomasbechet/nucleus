#ifndef NU_MEMORY_H
#define NU_MEMORY_H

#include <nucleus/vm/platform.h>
#include <nucleus/vm/types.h>
#include <nucleus/vm/error.h>

#define NU_MEM_1G   (1024 * 1024 * 1024)
#define NU_MEM_512M (1024 * 1024 * 512)
#define NU_MEM_256M (1024 * 1024 * 256)
#define NU_MEM_128M (1024 * 1024 * 128)
#define NU_MEM_64M  (1024 * 1024 * 64)
#define NU_MEM_32M  (1024 * 1024 * 32)
#define NU_MEM_16M  (1024 * 1024 * 16)
#define NU_MEM_8M   (1024 * 1024 * 8)

NU_API void *nu_memset(void *dst, nu_word_t c, nu_size_t n);
NU_API void  nu_memcpy(void *dst, const void *src, nu_size_t n);
NU_API void *nu_memalign(void *ptr, nu_size_t align);

#ifdef NU_IMPL

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

#endif

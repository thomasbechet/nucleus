#ifndef NU_MEMORY_H
#define NU_MEMORY_H

#include <nucleus/core/config.h>

#define NU_DEFAULT_ALIGN 16

typedef struct
{
    void *(*callback)(void     *p, // pointer (NULL if malloc)
                      nu_size_t s, // current size (0 if malloc)
                      nu_size_t n, // new size (0 if free)
                      nu_size_t a, // alignment
                      void     *u);    // userdata
    void *userdata;
} nu_allocator_t;

NU_API nu_allocator_t *nu_allocator_core(void);

NU_API void *nu_alloc_a(nu_allocator_t *a, nu_size_t s);
NU_API void *nu_realloc_a(nu_allocator_t *a, void *p, nu_size_t s, nu_size_t n);
NU_API void  nu_free_a(nu_allocator_t *a, void *p, nu_size_t s);

NU_API void *nu_alloc(nu_size_t s);
NU_API void *nu_realloc(void *p, nu_size_t s, nu_size_t n);
NU_API void  nu_free(void *p, nu_size_t s);

NU_API void *nu_memset(void *dst, nu_word_t c, nu_size_t n);
NU_API void  nu_memcpy(void *dst, const void *src, nu_size_t n);
NU_API void *nu_memalign(void *ptr, nu_size_t align);

#endif

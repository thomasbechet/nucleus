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
} nu__allocator_t;

NU_DEFINE_HANDLE_POINTER(nu_allocator_t, nu__allocator_t);

NU_API void *nu_alloc(nu_allocator_t alloc, nu_size_t s);
NU_API void *nu_realloc(nu_allocator_t alloc,
                        void          *ptr,
                        nu_size_t      size,
                        nu_size_t      new_size);
NU_API void  nu_free(nu_allocator_t alloc, void *ptr, nu_size_t s);

NU_API void *nu_memset(void *dst, nu_word_t c, nu_size_t n);
NU_API void  nu_memcpy(void *dst, const void *src, nu_size_t n);
NU_API void *nu_memalign(void *ptr, nu_size_t align);

#ifdef NU_STDLIB
NU_API void nuext_allocator_create_stdlib(nu_allocator_t *alloc);
#endif

#endif

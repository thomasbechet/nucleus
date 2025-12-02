#ifndef NU_ALLOCATOR_H
#define NU_ALLOCATOR_H

#include <nucleus/vm/types.h>
#include <nucleus/vm/error.h>
#include <nucleus/vm/memory.h>
#include <nucleus/vm/math.h>

typedef enum
{
    NU_MEMORY_USAGE_CORE,
    NU_MEMORY_USAGE_TABLE,
    NU_MEMORY_USAGE_RENDERER,
    NU_MEMORY_USAGE_COMPILER
} nu_memory_usage_t;

typedef void *(*nu_allocator_callback_pfn_t)(nu_size_t         size,
                                             nu_size_t         align,
                                             nu_memory_usage_t usage,
                                             void             *userdata);

typedef struct
{
    nu_allocator_callback_pfn_t callback;
    void                       *userdata;
} nu_allocator_api_t;

#ifdef NU_STDLIB
NU_API void nu_allocator_api_stdlib(nu_allocator_api_t *info);
#endif

#ifdef NU_IMPL

typedef struct
{
    nu_allocator_api_t api;
} nu__allocator_t;

nu_error_t nu__allocator_init(const nu_allocator_api_t *info,
                              nu__allocator_t          *alloc);
void      *nu__alloc(nu__allocator_t  *alloc,
                     nu_size_t         size,
                     nu_memory_usage_t usage);
void      *nu__aligned_alloc(nu__allocator_t  *alloc,
                             nu_size_t         size,
                             nu_size_t         align,
                             nu_memory_usage_t usage);

#ifdef NU_STDLIB

#include <stdlib.h>

static void *
nu__static_allocator_callback (nu_size_t         size,
                               nu_size_t         align,
                               nu_memory_usage_t usage,
                               void             *userdata)
{
    (void)align;
    (void)usage;
    (void)userdata;
    return malloc(size);
}

void
nu_allocator_api_stdlib (nu_allocator_api_t *info)
{
    info->userdata = NU_NULL;
    info->callback = nu__static_allocator_callback;
}

#endif

nu_error_t
nu__allocator_init (const nu_allocator_api_t *api, nu__allocator_t *alloc)
{
    alloc->api = *api;
    return NU_ERROR_NONE;
}

void *
nu__alloc (nu__allocator_t *alloc, nu_size_t size, nu_memory_usage_t usage)
{
    NU_ASSERT(size > 0);
    return alloc->api.callback(size, 16, usage, alloc->api.userdata);
}

void *
nu__aligned_alloc (nu__allocator_t  *alloc,
                   nu_size_t         size,
                   nu_size_t         align,
                   nu_memory_usage_t usage)
{
    NU_ASSERT(size > 0);
    return alloc->api.callback(size, align, usage, alloc->api.userdata);
}

#endif

#endif

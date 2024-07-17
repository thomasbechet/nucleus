#ifndef NU_SURFACE_H
#define NU_SURFACE_H

#include <nucleus/types.h>

NU_API void nu_surface_create(nu_allocator_t alloc);

#ifdef NU_IMPLEMENTATION

#include <nucleus/surface/glfw.h>

void
nu_surface_create (nu_allocator_t alloc)
{
    (void)alloc;
}

#endif

#endif

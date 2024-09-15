#ifndef NU_IMAGE_H
#define NU_IMAGE_H

#include <nucleus/graphics/api.h>

typedef struct
{
    nu_uvec2_t     size;
    nu_color_t    *colors;
    nu_allocator_t allocator;
} nu__image_t;

typedef NU_POOL(nu__image_t) nu__image_pool_t;

#endif

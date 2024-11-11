#ifndef NU_IMAGE_H
#define NU_IMAGE_H

#include <nucleus/graphics/api.h>

typedef struct
{
    nu_image_type_t type;
    nu_v3u_t        size;
    nu_size_t       layer;
    nu_byte_t      *data;
    nu_allocator_t  allocator;
} nu__image_t;

typedef NU_POOL(nu__image_t) nu__image_pool_t;

#endif

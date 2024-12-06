#ifndef NU_IMAGE_H
#define NU_IMAGE_H

#include <nucleus/graphics/api.h>

typedef struct
{
    nu_image_type_t type;
    nu_v3u_t        size;
    nu_u32_t        layer;
    nu_byte_t      *data;
} nu__image_t;

#endif

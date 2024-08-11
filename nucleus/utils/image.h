#ifndef NU_IMAGE_H
#define NU_IMAGE_H

#include <nucleus/graphics.h>

typedef struct
{
    nu_uvec2_t  size;
    nu_color_t *data;
} nu_image_t;

NU_API nu_error_t nu_image_init(nu_allocator_t *alloc,
                                nu_uvec2_t      size,
                                nu_image_t     *ima);
NU_API void       nu_image_free(nu_image_t *ima, nu_allocator_t *alloc);

#endif

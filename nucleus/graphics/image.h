#ifndef NU_IMAGE_H
#define NU_IMAGE_H

#include <nucleus/graphics/texture.h>

NU_DEFINE_HANDLE(nu_image_t);

NU_API nu_error_t        nu_image_create(nu_uvec2_t size, nu_image_t *ima);
NU_API void              nu_image_delete(nu_image_t image);
NU_API nu_color_t       *nu_image_colors(nu_image_t image);
NU_API nu_uvec2_t        nu_image_size(nu_image_t image);
NU_API nu_texture_info_t nu_image_texture_info(nu_image_t image);

#endif

#ifndef NU_IMPORTER_H
#define NU_IMPORTER_H

#include <nucleus/graphics/graphics.h>

NU_API nu_image_t nuext_image_load_filename(const nu_char_t *filename);
NU_API nu_image_t nuext_image_load_memory(const nu_byte_t *data,
                                          nu_size_t        size);

NU_API nu_model_t nuext_model_load_filename(const nu_char_t *filename);

#endif

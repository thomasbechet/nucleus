#ifndef NU_IMPORTER_H
#define NU_IMPORTER_H

#include <nucleus/graphics/graphics.h>

#ifdef NU_BUILD_GRAPHICS
NU_API nu_image_t nuext_image_load_filename(const nu_char_t *filename);
NU_API nu_image_t nuext_image_load_memory(const nu_byte_t *data,
                                          nu_size_t        size);

NU_API nu_model_t   nuext_model_load_filename(const nu_char_t *filename);
NU_API nu_cubemap_t nuext_cubemap_load_filename(const nu_char_t *filename);
#endif

#ifdef NU_STDLIB
NU_API nu_byte_t *nuext_bytes_load_filename(const nu_char_t *name,
                                            nu_size_t       *size);
#endif

#endif

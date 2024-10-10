#ifndef NU_IMPORTER_API_H
#define NU_IMPORTER_API_H

#include <nucleus/graphics/api.h>
#include <nucleus/asset/api.h>

NU_API nu_image_t nuext_image_load_filename(const nu_char_t *filename);
NU_API nu_image_t nuext_image_load_memory(const nu_byte_t *data,
                                          nu_size_t        size);

NU_API nu_model_t   nuext_model_load_filename(const nu_char_t *filename);
NU_API nu_texture_t nuext_cubemap_load_filename(const nu_char_t *filename);

NU_API nu_asset_t nuext_import_asset(nu_asset_type_t  type,
                                     const nu_char_t *name,
                                     const nu_char_t *filename);
NU_API nu_error_t nuext_import_package(const nu_char_t *filename);

#endif

#ifndef NU_IMPORTER_H
#define NU_IMPORTER_H

#include <nucleus/graphics/graphics.h>
#include <nucleus/asset/asset.h>

NU_API nu_byte_t *nuext_bytes_load_filename(const nu_char_t *name,
                                            nu_size_t       *size);

NU_API nu_image_t nuext_image_load_filename(const nu_char_t *filename);
NU_API nu_image_t nuext_image_load_memory(const nu_byte_t *data,
                                          nu_size_t        size);

NU_API nu_model_t   nuext_model_load_filename(const nu_char_t *filename);
NU_API nu_cubemap_t nuext_cubemap_load_filename(const nu_char_t *filename);

NU_API nu_asset_t nuext_asset_load_filename(nu_asset_type_t  type,
                                            const nu_char_t *name,
                                            const nu_char_t *filename);
NU_API nu_error_t nuext_asset_load_package(const nu_char_t *filename);

#endif

#ifndef NU_IMPORTER_API_H
#define NU_IMPORTER_API_H

#include <nucleus/graphics/api.h>
#include <nucleus/resource/api.h>

NU_API nu_image_t nuext_image_load_file(nu_str_t filename);
NU_API nu_image_t nuext_image_load_memory(const nu_byte_t *data,
                                          nu_size_t        size);
NU_API nu_model_t nuext_model_load_file(nu_str_t filename);

NU_API nu_error_t nuext_import_package(nu_str_t filename, nu_uid_t group);

#endif

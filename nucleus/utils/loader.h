#ifndef NU_LOADER_H
#define NU_LOADER_H

#include <nucleus/core.h>

NU_API nu_error_t nuext_load_gltf(const nu_char_t *filename,
                                  nu_logger_t     *logger);

#endif

#ifndef NU_LOADER_H
#define NU_LOADER_H

#include <nucleus/core.h>

typedef struct
{
    const nu_char_t *name;
    const nu_vec3_t *positions;
    const nu_vec2_t *uvs;
    const nu_vec3_t *normals;
    nu_size_t        count;
} nuext_loader_mesh_t;

typedef nu_error_t (*nuext_mesh_load_callback_t)(
    const nuext_loader_mesh_t *mesh, void *user);

NU_API nu_error_t nuext_load_gltf(const nu_char_t           *filename,
                                  nu_logger_t               *logger,
                                  nu_allocator_t             allocator,
                                  nuext_mesh_load_callback_t callback,
                                  void                      *user);

#endif

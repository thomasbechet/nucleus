#ifndef NU_MESH_IMPL_H
#define NU_MESH_IMPL_H

#include <nucleus/internal.h>

nu_mesh_t
nu_mesh_create (nu_primitive_t primitive, nu_size_t count)
{
#ifdef NU_BUILD_GL
    return nugl__mesh_create(primitive, count);
#endif
}
void
nu_mesh_delete (nu_mesh_t mesh)
{
#ifdef NU_BUILD_GL
    nugl__mesh_delete(mesh);
#endif
}
void
nu_mesh_write_uvs (nu_mesh_t      mesh,
                   nu_size_t      first,
                   nu_size_t      count,
                   const nu_v2_t *data)
{
#ifdef NU_BUILD_GL
    nugl__mesh_write_uvs(mesh, first, count, data);
#endif
}
void
nu_mesh_write_positions (nu_mesh_t      mesh,
                         nu_size_t      first,
                         nu_size_t      count,
                         const nu_v3_t *data)
{
#ifdef NU_BUILD_GL
    nugl__mesh_write_positions(mesh, first, count, data);
#endif
}
void
nu_mesh_write_colors (nu_mesh_t         mesh,
                      nu_size_t         first,
                      nu_size_t         count,
                      const nu_color_t *data)
{
}

#endif

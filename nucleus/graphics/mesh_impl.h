#ifndef NU_MESH_IMPL_H
#define NU_MESH_IMPL_H

#include <nucleus/internal.h>

nu_mesh_t
nu_mesh_create (nu_primitive_t primitive, nu_size_t count)
{
    nu_size_t   index;
    nu__mesh_t *mesh = NU_POOL_ADD(&_ctx.graphics.meshes, &index);
    mesh->primitive  = primitive;
    mesh->capacity   = count;
#ifdef NU_BUILD_GRAPHICS_GL
    nugl__mesh_init(mesh);
#endif
    return NU_HANDLE_MAKE(nu_mesh_t, index);
}
void
nu_mesh_delete (nu_mesh_t mesh)
{
    nu__mesh_t *pmesh = _ctx.graphics.meshes.data + NU_HANDLE_INDEX(mesh);
#ifdef NU_BUILD_GRAPHICS_GL
    nugl__mesh_free(pmesh);
#endif
    NU_POOL_REMOVE(&_ctx.graphics.meshes, NU_HANDLE_INDEX(mesh));
}
void
nu_mesh_write_uvs (nu_mesh_t      mesh,
                   nu_size_t      first,
                   nu_size_t      count,
                   const nu_v2_t *data)
{
    NU_ASSERT(mesh);
    nu__mesh_t *pmesh = _ctx.graphics.meshes.data + NU_HANDLE_INDEX(mesh);
#ifdef NU_BUILD_GRAPHICS_GL
    nugl__mesh_write_uvs(pmesh, first, count, data);
#endif
}
void
nu_mesh_write_positions (nu_mesh_t      mesh,
                         nu_size_t      first,
                         nu_size_t      count,
                         const nu_v3_t *data)
{
    NU_ASSERT(mesh);
    nu__mesh_t *pmesh = _ctx.graphics.meshes.data + NU_HANDLE_INDEX(mesh);
    NU_ASSERT(first + count <= pmesh->capacity);
#ifdef NU_BUILD_GRAPHICS_GL
    nugl__mesh_write_positions(pmesh, first, count, data);
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

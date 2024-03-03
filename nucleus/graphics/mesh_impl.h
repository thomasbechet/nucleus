#ifndef NU_MESH_IMPL_H
#define NU_MESH_IMPL_H

#include <nucleus/internal.h>
#include <nucleus/graphics/backend_impl.h>

static void
nu__mesh_cleanup (void *data)
{
    nu__mesh_t *mesh = data;
#ifdef NU_BUILD_GRAPHICS_GL
    nugl__mesh_free(mesh);
#endif
}
nu_object_type_t
nu_mesh (void)
{
    return _ctx.graphics.obj_mesh;
}
nu_mesh_t
nu_mesh_new (nu_primitive_t primitive, nu_size_t capacity)
{
    nu__mesh_t *mesh = nu_object_new(nu_mesh());
    mesh->primitive  = primitive;
    mesh->capacity   = capacity;
#ifdef NU_BUILD_GRAPHICS_GL
    nugl__mesh_init(mesh);
#endif
    return (nu_mesh_t)mesh;
}
void
nu_mesh_set_uvs (nu_mesh_t      mesh,
                 nu_size_t      first,
                 nu_size_t      count,
                 const nu_v2_t *data)
{
    NU_ASSERT(mesh);
    nu__mesh_t *pmesh = (nu__mesh_t *)mesh;
#ifdef NU_BUILD_GRAPHICS_GL
    nugl__mesh_write_uvs(pmesh, first, count, data);
#endif
}
void
nu_mesh_set_positions (nu_mesh_t      mesh,
                       nu_size_t      first,
                       nu_size_t      count,
                       const nu_v3_t *data)
{
    NU_ASSERT(mesh);
    nu__mesh_t *pmesh = (nu__mesh_t *)mesh;
    NU_ASSERT(first + count <= pmesh->capacity);
#ifdef NU_BUILD_GRAPHICS_GL
    nugl__mesh_write_positions(pmesh, first, count, data);
#endif
}
void
nu_mesh_set_colors (nu_mesh_t         mesh,
                    nu_size_t         first,
                    nu_size_t         count,
                    const nu_color_t *data)
{
}
size_t
nu_mesh_capacity (nu_mesh_t mesh)
{
    NU_ASSERT(mesh);
    return ((nu__mesh_t *)mesh)->capacity;
}

#endif

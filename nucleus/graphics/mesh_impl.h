#ifndef NU_MESH_IMPL_H
#define NU_MESH_IMPL_H

#include <nucleus/internal.h>

static void
nu__mesh_handler (nu_object_hook_t hook, void *data)
{
    switch (hook)
    {
        case NU_OBJECT_CLEANUP: {
            nu__mesh_t *mesh = data;
#ifdef NU_BUILD_GRAPHICS_GL
            nugl__mesh_free(mesh);
#endif
        }
        break;
        case NU_OBJECT_SAVE:
        case NU_OBJECT_LOAD:
            break;
    }
}
nu_mesh_t
nu_mesh_new (nu_scope_t scope, nu_primitive_t primitive, nu_size_t capacity)
{
    nu__mesh_t *mesh = nu_object_new(scope, _ctx.graphics.obj_mesh);
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

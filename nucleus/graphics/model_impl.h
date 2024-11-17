#ifndef NU_MODEL_IMPL_H
#define NU_MODEL_IMPL_H

#include <nucleus/internal.h>

static void
nu__model_handler (nu_object_hook_t hook, void *data)
{
}
nu_model_t
nu_model_new (nu_scope_t scope, nu_size_t node_count)
{
    NU_ASSERT(node_count);
    nu__model_t *m = nu_object_new(scope, _ctx.graphics.obj_model);
    NU_ARRAY_ALLOC(scope, &m->nodes, node_count);
    for (nu_size_t i = 0; i < m->nodes.size; ++i)
    {
        m->nodes.data[i].mesh      = NU_NULL;
        m->nodes.data[i].material  = NU_NULL;
        m->nodes.data[i].transform = nu_m4_identity();
    }
    return (nu_model_t)m;
}
void
nu_model_set (nu_model_t    model,
              nu_size_t     index,
              nu_mesh_t     mesh,
              nu_material_t material,
              nu_m4_t       transform)
{
    NU_ASSERT(model);
    nu__model_t *m = (nu__model_t *)model;
    NU_ASSERT(index < m->nodes.size);
    m->nodes.data[index].mesh      = mesh;
    m->nodes.data[index].material  = material;
    m->nodes.data[index].transform = transform;
}
void
nu_draw_model (nu_renderpass_t pass, nu_model_t model, nu_m4_t transform)
{
    nu__model_t            *m     = (nu__model_t *)model;
    const nu__model_node_t *nodes = m->nodes.data;
    for (nu_size_t i = 0; i < m->nodes.size; ++i)
    {
        if (m->nodes.data[i].mesh)
        {
            nu_m4_t global_transform = nu_m4_mul(transform, nodes[i].transform);
            nu_draw_mesh(
                pass, nodes[i].mesh, nodes[i].material, global_transform);
        }
    }
}

#endif

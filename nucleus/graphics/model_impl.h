#ifndef NU_MODEL_IMPL_H
#define NU_MODEL_IMPL_H

#include <nucleus/internal.h>

nu_model_t
nu_model_create (nu_size_t node_count)
{
    NU_ASSERT(node_count);
    nu_size_t    index;
    nu__model_t *m = NU_POOL_ADD(&_ctx.graphics.models, &index);
    NU_VEC_INIT(node_count, &m->nodes);
    NU_VEC_RESIZE(&m->nodes, node_count);
    for (nu_size_t i = 0; i < m->nodes.size; ++i)
    {
        m->nodes.data[i].mesh      = NU_NULL;
        m->nodes.data[i].material  = NU_NULL;
        m->nodes.data[i].transform = nu_m4_identity();
    }
    return NU_HANDLE_MAKE(nu_model_t, index);
}
void
nu_model_delete (nu_model_t model)
{
    NU_ASSERT(model);
    nu__model_t *m = _ctx.graphics.models.data + NU_HANDLE_INDEX(model);
    NU_VEC_FREE(&m->nodes);
    NU_POOL_REMOVE(&_ctx.graphics.models, NU_HANDLE_INDEX(model));
}
void
nu_model_set (nu_model_t    model,
              nu_size_t     index,
              nu_mesh_t     mesh,
              nu_material_t material,
              nu_m4_t       transform)
{
    NU_ASSERT(model);
    nu__model_t *m = _ctx.graphics.models.data + NU_HANDLE_INDEX(model);
    NU_ASSERT(index < m->nodes.size);
    m->nodes.data[index].mesh      = mesh;
    m->nodes.data[index].material  = material;
    m->nodes.data[index].transform = transform;
}
void
nu_draw_model (nu_renderpass_t pass, nu_model_t model, nu_m4_t transform)
{
    nu__model_t *m = _ctx.graphics.models.data + NU_HANDLE_INDEX(model);
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

#ifndef NU_MODEL_IMPL_H
#define NU_MODEL_IMPL_H

#include <nucleus/internal.h>

nu_model_t
nu_model_create (void)
{
    nu_size_t    index;
    nu__model_t *m = NU_POOL_ADD(&_ctx.graphics.models, &index);
    NU_VEC_INIT(1, &m->nodes);
    return NU_HANDLE_MAKE(nu_model_t, index);
}
void
nu_model_delete (nu_model_t model)
{
    nu_size_t    index = NU_HANDLE_INDEX(model);
    nu__model_t *m     = &_ctx.graphics.models.data[index];
    NU_VEC_FREE(&m->nodes);
}
void
nu_draw_model (nu_renderpass_t pass, nu_model_t model, nu_m4_t transform)
{
    nu_size_t               index = NU_HANDLE_INDEX(model);
    nu__model_t            *m     = &_ctx.graphics.models.data[index];
    const nu__model_node_t *nodes = m->nodes.data;
    for (nu_size_t i = 0; i < m->nodes.size; ++i)
    {
        nu_m4_t global_transform = nu_m4_mul(transform, nodes[i].transform);
        nu_draw_mesh(pass, nodes[i].mesh, nodes[i].material, global_transform);
    }
}

#endif

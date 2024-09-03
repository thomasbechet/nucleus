#ifndef NU_MODEL_IMPL_H
#define NU_MODEL_IMPL_H

#include <nucleus/internal.h>

void
nu_model_create (nu_model_t *model)
{
    nu__model_t *m = nu_pool_add(&_ctx.graphics.models, &model->_index);
    nu_vec_init(1, &m->assets);
    nu_vec_init(1, &m->nodes);
}
void
nu_model_delete (nu_model_t model)
{
    nu__model_t *m = &_ctx.graphics.models.data[model._index];
    nu_vec_free(&m->assets);
    nu_vec_free(&m->nodes);
}
void
nu_draw_model (nu_renderpass_t pass, nu_model_t model, nu_mat4_t transform)
{
    nu__model_t            *m     = &_ctx.graphics.models.data[model._index];
    const nu__model_node_t *nodes = m->nodes.data;
    for (nu_size_t i = 0; i < m->nodes.size; ++i)
    {
        nu_material_t material     = m->assets.data[nodes[i].material].material;
        nu_mesh_t     mesh         = m->assets.data[nodes[i].mesh].mesh;
        nu_mat4_t global_transform = nu_mat4_mul(transform, nodes[i].transform);
        nu_draw_mesh(pass, material, mesh, global_transform);
    }
}

#endif

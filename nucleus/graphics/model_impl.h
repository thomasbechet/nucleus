#ifndef NU_MODEL_IMPL_H
#define NU_MODEL_IMPL_H

#include <nucleus/graphics/model.h>

void
nu_model_create (nu_allocator_t alloc, nu_model_t *model)
{
    model->_ptr = (nu__model_t *)nu_alloc(alloc, sizeof(*model->_ptr));
    model->_ptr->allocator = alloc;
    nu_vec_init(&model->_ptr->assets, alloc, 1);
    nu_vec_init(&model->_ptr->nodes, alloc, 1);
}
void
nu_model_delete (nu_model_t model)
{
    nu_vec_free(&model._ptr->assets, model._ptr->allocator);
    nu_vec_free(&model._ptr->nodes, model._ptr->allocator);
    nu_free(model._ptr->allocator, model._ptr, sizeof(*model._ptr));
}
void
nu_draw_model (nu_renderer_t   renderer,
               nu_renderpass_t pass,
               nu_model_t      model,
               nu_mat4_t       transform)
{
    const nu__model_node_t *nodes = model._ptr->nodes.data;
    for (nu_size_t i = 0; i < model._ptr->nodes.size; ++i)
    {
        nu_material_t material
            = model._ptr->assets.data[nodes[i].material].material;
        nu_mesh_t mesh = model._ptr->assets.data[nodes[i].mesh].mesh;
        nu_mat4_t global_transform = nu_mat4_mul(transform, nodes[i].transform);
        nu_draw_mesh(renderer, pass, material, mesh, global_transform);
    }
}

#endif

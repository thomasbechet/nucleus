#ifndef NU_MODEL_IMPL_H
#define NU_MODEL_IMPL_H

#include <nucleus/graphics/model.h>

void
nu_model_init (nu_allocator_t *alloc, nu_model_t *model)
{
    nu_vec_init(&model->assets, alloc, 1);
    nu_vec_init(&model->nodes, alloc, 1);
}
void
nu_model_free (nu_model_t *model, nu_allocator_t *alloc)
{
    nu_vec_free(&model->assets, alloc);
    nu_vec_free(&model->nodes, alloc);
}
void
nu_draw_model (nu_renderer_t         *renderer,
               nu_renderpass_handle_t pass,
               const nu_model_t      *model,
               nu_mat4_t              transform)
{
    const nu__model_node_t *nodes = model->nodes.data;
    for (nu_size_t i = 0; i < model->nodes.size; ++i)
    {
        const nu_material_handle_t material
            = model->assets.data[nodes[i].material].material;
        const nu_mesh_handle_t mesh = model->assets.data[nodes[i].mesh].mesh;
        nu_mat4_t global_transform = nu_mat4_mul(transform, nodes[i].transform);
        nu_draw_mesh(renderer, pass, material, mesh, global_transform);
    }
}

#endif

#ifndef NU_MODEL_IMPL_H
#define NU_MODEL_IMPL_H

#include <nucleus/internal.h>

nu_object_type_t
nu_model (void)
{
    return _ctx.graphics.obj_model;
}
static nu_object_t
nu__model_load (nu_seria_t seria)
{
    return nu_model_load(seria);
}
static void
nu__model_save (nu_seria_t seria, nu_object_t model)
{
    nu_model_save(model, seria);
}
nu_model_t
nu_model_new (nu_size_t node_count)
{
    NU_ASSERT(node_count);
    nu__model_t *m = nu_object_new(nu_model());
    NU_ARRAY_ALLOC(&m->nodes, node_count);
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
nu_model_t
nu_model_load (nu_seria_t seria)
{
    nu_u32_t node_count;
    nu_seria_read_u32(seria, 1, &node_count);
    nu_model_t model = nu_model_new(node_count);
    for (nu_size_t i = 0; i < node_count; ++i)
    {
        nu_material_t material = nu_seria_read_object(seria, nu_material());
        nu_mesh_t     mesh     = nu_seria_read_object(seria, nu_mesh());
        NU_ASSERT(material && mesh);
        nu_m4_t transform;
        nu_seria_read_m4(seria, 1, &transform);
        nu_model_set(model, i, mesh, material, transform);
    }
    return model;
}
void
nu_model_save (nu_model_t model, nu_seria_t seria)
{
    nu__model_t *m    = (nu__model_t *)model;
    nu_u32_t     size = m->nodes.size;
    nu_seria_write_u32(seria, 1, &size);
    for (nu_size_t i = 0; i < m->nodes.size; ++i)
    {
        nu_seria_write_object(seria, m->nodes.data[i].material);
        nu_seria_write_object(seria, m->nodes.data[i].mesh);
        nu_seria_write_m4(seria, 1, &m->nodes.data[i].transform);
    }
}

#endif

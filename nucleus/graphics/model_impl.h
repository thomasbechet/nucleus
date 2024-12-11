#ifndef NU_MODEL_IMPL_H
#define NU_MODEL_IMPL_H

#include <nucleus/internal.h>

nugfx_model_t
nugfx_model_new (nu_size_t node_count)
{
    NU_ASSERT(node_count);
    nugfx__model_t *m = nu_object_new(_ctx.graphics.obj_model);
    NU_ARRAY_ALLOC(&m->nodes, node_count);
    for (nu_size_t i = 0; i < m->nodes.size; ++i)
    {
        m->nodes.data[i].mesh      = NU_NULL;
        m->nodes.data[i].texture   = NU_NULL;
        m->nodes.data[i].transform = nu_m4_identity();
    }
    return (nugfx_model_t)m;
}
void
nugfx_model_set (nugfx_model_t   model,
                 nu_size_t       index,
                 nugfx_mesh_t    mesh,
                 nugfx_texture_t texture,
                 nu_m4_t         transform)
{
    NU_ASSERT(model);
    nugfx__model_t *m = (nugfx__model_t *)model;
    NU_ASSERT(index < m->nodes.size);
    m->nodes.data[index].mesh      = mesh;
    m->nodes.data[index].texture   = texture;
    m->nodes.data[index].transform = transform;
}
void
nugfx_draw_model (nugfx_model_t model)
{
    nugfx__model_t            *m         = (nugfx__model_t *)model;
    const nugfx__model_node_t *nodes     = m->nodes.data;
    nu_m4_t                    transform = _ctx.graphics.state.transform;
    for (nu_size_t i = 0; i < m->nodes.size; ++i)
    {
        if (m->nodes.data[i].mesh)
        {
            nu_m4_t global_transform = nu_m4_mul(transform, nodes[i].transform);
            nugfx_push_transform(global_transform);
            nugfx_push_texture(nodes[i].texture);
            nugfx_draw(nodes[i].mesh, 0, nugfx_mesh_capacity(nodes[i].mesh));
        }
    }
}
nugfx_model_t
nugfx_model_load (nu_seria_t seria)
{
    nu_u32_t node_count;
    nu_seria_read_u32(seria, 1, &node_count);
    nugfx_model_t model = nugfx_model_new(node_count);
    for (nu_size_t i = 0; i < node_count; ++i)
    {
        nu_material_t material = nu_seria_read_object(seria, nu_material());
        nu_mesh_t     mesh     = nu_seria_read_object(seria, nu_mesh());
        NU_ASSERT(material && mesh);
        nu_m4_t transform;
        nu_seria_read_m4(seria, 1, &transform);
        nugfx_model_set(model, i, mesh, material, transform);
    }
    return model;
}
void
nugfx_model_save (nugfx_model_t model, nu_seria_t seria)
{
    nugfx__model_t *m    = (nugfx__model_t *)model;
    nu_u32_t        size = m->nodes.size;
    nu_seria_write_u32(seria, 1, &size);
    for (nu_size_t i = 0; i < m->nodes.size; ++i)
    {
        nu_seria_write_object(seria, m->nodes.data[i].material);
        nu_seria_write_object(seria, m->nodes.data[i].mesh);
        nu_seria_write_m4(seria, 1, &m->nodes.data[i].transform);
    }
}

#endif

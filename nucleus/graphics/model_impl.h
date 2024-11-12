#ifndef NU_MODEL_IMPL_H
#define NU_MODEL_IMPL_H

#include <nucleus/internal.h>

nu_model_t
nu_model_create (void)
{
    nu_size_t    index;
    nu__model_t *m = NU_POOL_ADD(&_ctx.graphics.models, &index);
    NU_VEC_INIT(1, &m->resources);
    NU_VEC_INIT(1, &m->nodes);
    return NU_HANDLE_MAKE(nu_model_t, index);
}
void
nu_model_delete (nu_model_t model)
{
    nu_size_t    index = NU_HANDLE_INDEX(model);
    nu__model_t *m     = &_ctx.graphics.models.data[index];
    NU_VEC_FREE(&m->resources);
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
        nu_material_t material = m->resources.data[nodes[i].material].material;
        nu_mesh_t     mesh     = m->resources.data[nodes[i].mesh].mesh;
        nu_m4_t global_transform = nu_m4_mul(transform, nodes[i].transform);
        nu_draw_mesh(pass, mesh, material, global_transform);
    }
}

#ifdef NU_BUILD_RESOURCE
nu_model_t
nu_model (nu_uid_t uid)
{
    return nu_resource_data(NU_UID(NU_RESOURCE_MODEL), uid);
}
static void
nu__model_resource_unload (void *data)
{
    nu_model_delete(data);
}
static void *
nu__model_resource_load (nu_seria_t seria)
{
    return NU_NULL;
}
static void
nu__model_resource_save (void *data, nu_seria_t seria)
{
}
static void
nu__model_resource_register (void)
{
    nu_resource_register(NU_UID(NU_RESOURCE_MODEL),
                         nu__model_resource_load,
                         nu__model_resource_unload,
                         nu__model_resource_save);
}
#endif

#endif

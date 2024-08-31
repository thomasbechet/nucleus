#ifndef NU_MODEL_H
#define NU_MODEL_H

#include <nucleus/graphics.h>

typedef struct
{
    nu_u16_t  mesh;
    nu_u16_t  material;
    nu_mat4_t transform;
} nu__model_node_t;

typedef union
{
    nu_mesh_t     mesh;
    nu_texture_t  texture;
    nu_material_t material;
} nu__model_asset_t;

typedef nu_vec(nu__model_asset_t) nu__model_asset_vec_t;
typedef nu_vec(nu__model_node_t) nu__model_node_vec_t;

typedef struct
{
    nu_allocator_t        allocator;
    nu__model_asset_vec_t assets;
    nu__model_node_vec_t  nodes;
} nu__model_t;

NU_DEFINE_HANDLE_POINTER(nu_model_t, nu__model_t);

NU_API void nu_model_create(nu_allocator_t alloc, nu_model_t *model);
NU_API void nu_model_delete(nu_model_t model);
NU_API void nu_draw_model(nu_renderer_t   renderer,
                          nu_renderpass_t pass,
                          nu_model_t      model,
                          nu_mat4_t       transform);

#endif

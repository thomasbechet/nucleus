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
    nu_mesh_handle_t     mesh;
    nu_texture_handle_t  texture;
    nu_material_handle_t material;
} nu__model_asset_t;

typedef nu_vec(nu__model_asset_t) nu__model_asset_vec_t;
typedef nu_vec(nu__model_node_t) nu__model_node_vec_t;

typedef struct
{
    nu__model_asset_vec_t assets;
    nu__model_node_vec_t  nodes;
} nu_model_t;

NU_API void nu_model_init(nu_allocator_t alloc, nu_model_t *model);
NU_API void nu_model_free(nu_model_t *model, nu_allocator_t alloc);
NU_API void nu_draw_model(nu_renderer_t          renderer,
                          nu_renderpass_handle_t pass,
                          const nu_model_t      *model,
                          nu_mat4_t              transform);

#endif

#ifndef NU_MODEL_IMPL_H
#define NU_MODEL_IMPL_H

#include <nucleus/utils/model.h>
#include <nucleus/utils/loader.h>

// typedef struct
// {
//     nu_u32_t id;
//     nu_u32_t index;
// } nuext__model_asset_entry_t;
//
// typedef struct
// {
//     nu_model_t     *model;
//     nu_renderer_t  *renderer;
//     nu_allocator_t *alloc;
//     nu_logger_t    *logger;
//     nu_u32_t        default_material_item;
// } nuext__gltf_to_model_userdata_t;
//
// static nu_error_t
// nuext__gltf_to_model_callback (const nuext_gltf_asset_t *asset, void
// *userdata)
// {
//     nuext__gltf_to_model_userdata_t *data = userdata;
//     NU_CHECK(item, return NU_ERROR_ALLOCATION);
//     switch (asset->type)
//     {
//         case NUEXT_GLTF_MESH: {
//             NU_DEBUG(data->logger, "load mesh %lu", asset->id);
//             nu_vec_push(&data->model->assets, data->alloc);
//             nu_vec_push nu_mesh_info_t info;
//             info.positions = asset->mesh.positions;
//             info.uvs       = asset->mesh.uvs;
//             info.normals   = asset->mesh.normals;
//             info.count     = asset->mesh.count;
//             nu_error_t error
//                 = nu_mesh_create(data->renderer,
//                                  &info,
//                                  &nu_vec_last(&data->model->assets)->mesh);
//             NU_ERROR_CHECK(error, return error);
//             item->id = asset->id;
//         }
//         break;
//         case NUEXT_GLTF_TEXTURE: {
//             NU_DEBUG(data->logger, "load texture %lu", asset->id);
//             nu_texture_info_t info;
//             info.size   = asset->texture.size;
//             info.usage  = NU_TEXTURE_USAGE_SAMPLE;
//             info.format = NU_TEXTURE_FORMAT_COLOR;
//             info.colors = asset->texture.data;
//             nu_error_t error
//                 = nu_texture_create(data->renderer, &info, &item->texture);
//             NU_ERROR_CHECK(error, return error);
//             item->id = asset->id;
//         }
//         break;
//         case NUEXT_GLTF_MATERIAL: {
//             NU_DEBUG(data->logger, "load material %lu", asset->id);
//             const nu_texture_handle_t *diffuse_tex = NU_NULL;
//             const nu__model_item_t    *items       = data->model->items.data;
//             for (nu_size_t i = 0; i < data->model->items.size; ++i)
//             {
//                 if (items[i].id == asset->material.diffuse_id)
//                 {
//                     diffuse_tex = &items[i].texture;
//                     break;
//                 }
//             }
//             if (!diffuse_tex)
//             {
//                 NU_ERROR(data->logger, "diffuse texture not found");
//                 return NU_ERROR_RESOURCE_LOADING;
//             }
//             nu_material_info_t info = NU_MATERIAL_INFO_DEFAULT_MESH;
//             info.mesh.color0        = diffuse_tex;
//             nu_error_t error        = nu_material_create(
//                 data->renderer, &info, &model_asset->material);
//             NU_ERROR_CHECK(error, return error);
//             item->id = asset->id;
//         }
//         break;
//         case NUEXT_GLTF_NODE: {
//             NU_DEBUG(data->logger, "load node %lu", asset->id);
//             nu_vec_push(&data->model->commands, data->alloc);
//             nu__model_command_t *cmd = nu_vec_last(&data->model->commands);
//             NU_CHECK(cmd, return NU_ERROR_ALLOCATION);
//             cmd->transform                = asset->node.transform;
//             cmd->material                 = -1;
//             cmd->mesh                     = -1;
//             const nu__model_item_t *items = data->model->items.data;
//             for (nu_size_t i = 0; i < data->model->items.size; ++i)
//             {
//                 if (items[i].id == asset->node.mesh_id)
//                 {
//                     cmd->mesh = i;
//                 }
//                 if (asset->node.material_id
//                     && items[i].id == asset->node.material_id)
//                 {
//                     cmd->material = i;
//                 }
//             }
//             if (cmd->mesh == (nu_u16_t)-1)
//             {
//                 NU_ERROR(
//                     data->logger, "mesh not found %lu", asset->node.mesh_id);
//                 return NU_ERROR_RESOURCE_LOADING;
//             }
//             if (cmd->material == (nu_u16_t)-1)
//             {
//                 NU_ERROR(data->logger,
//                          "material not found %lu, using default one",
//                          asset->node.material_id);
//                 cmd->material = data->default_material_item;
//             }
//             item->id = asset->id;
//         }
//         break;
//     }
//     return NU_ERROR_NONE;
// }

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

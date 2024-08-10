#ifndef NU_MODEL_IMPL_H
#define NU_MODEL_IMPL_H

#include <nucleus/utils/model.h>
#include <nucleus/utils/loader.h>

NU_DECLARE_VECTOR(nu__model_items, nu__model_item_t)
NU_DECLARE_VECTOR(nu__model_commands, nu__model_command_t)

typedef struct
{
    nu_model_t     *model;
    nu_renderer_t  *renderer;
    nu_allocator_t *alloc;
    nu_logger_t    *logger;
} nuext__gltf_to_model_userdata_t;

static nu_error_t
nuext__gltf_to_model_callback (const nuext_gltf_asset_t *asset, void *userdata)
{
    nuext__gltf_to_model_userdata_t *data = userdata;
    nu__model_item_t                *item
        = nu__model_items_push(&data->model->items, data->alloc);
    NU_CHECK(item, return NU_ERROR_ALLOCATION);
    switch (asset->type)
    {
        case NUEXT_GLTF_ASSET_MESH: {
            nu_mesh_info_t info;
            info.positions = asset->mesh.positions;
            info.uvs       = asset->mesh.uvs;
            info.normals   = asset->mesh.normals;
            info.count     = asset->mesh.count;
            nu_error_t error
                = nu_mesh_create(data->renderer, &info, &item->mesh);
            NU_ERROR_CHECK(error, return error);
            item->hash = nu_hash(asset->mesh.name);
        }
        break;
        case NUEXT_GLTF_ASSET_TEXTURE: {
        }
        break;
        case NUEXT_GLTF_ASSET_MATERIAL: {
        }
        break;
        case NUEXT_GLTF_ASSET_NODE: {
            nu__model_command_t *cmd
                = nu__model_commands_push(&data->model->commands, data->alloc);
            NU_CHECK(cmd, return NU_ERROR_ALLOCATION);
            cmd->transform = asset->node.transform;
            cmd->material  = 0;
            const nu__model_item_t *items
                = nu__model_items_data_const(&data->model->items);
            nu_u32_t mesh_hash = nu_hash(asset->node.mesh);
            for (nu_size_t i = 0; i < nu__model_items_size(&data->model->items);
                 ++i)
            {
                if (items[i].hash == mesh_hash)
                {
                    cmd->mesh = i;
                    break;
                }
            }
        }
        break;
    }
    return NU_ERROR_NONE;
}

nu_error_t
nuext_model_from_gltf (const nu_char_t *filename,
                       nu_renderer_t   *renderer,
                       nu_logger_t     *logger,
                       nu_allocator_t  *alloc,
                       nu_model_t      *model)
{
    nu__model_items_init(&model->items, alloc, 8);
    nu__model_commands_init(&model->commands, alloc, 8);
    nuext__gltf_to_model_userdata_t userdata;
    userdata.alloc    = alloc;
    userdata.model    = model;
    userdata.logger   = logger;
    userdata.renderer = renderer;
    {
        nu__model_item_t *mat_item = nu__model_items_push(&model->items, alloc);
        nu__model_item_t *tex_item = nu__model_items_push(&model->items, alloc);
        nu_texture_info_t tinfo;
        tinfo.size   = nu_uvec2(1, 1);
        tinfo.usage  = NU_TEXTURE_USAGE_SAMPLE;
        tinfo.format = NU_TEXTURE_FORMAT_COLOR;
        nu_texture_create(renderer, &tinfo, &tex_item->texture);
        nu_color_t white = NU_COLOR_WHITE;
        nu_texture_write(renderer, &tex_item->texture, &white);
        nu_material_info_t minfo = nu_material_info_default();
        minfo.texture0           = &tex_item->texture;
        nu_material_create(renderer, &minfo, &mat_item->material);
    }
    nu_error_t error = nuext_load_gltf(
        filename, logger, alloc, nuext__gltf_to_model_callback, &userdata);
    NU_ERROR_CHECK(error, return error);
    return NU_ERROR_NONE;
}
void
nu_model_free (nu_model_t *model, nu_allocator_t *alloc)
{
    nu__model_items_free(&model->items, alloc);
    nu__model_commands_free(&model->commands, alloc);
}
void
nu_model_draw (nu_renderer_t    *renderer,
               nu_renderpass_t  *pass,
               const nu_model_t *model,
               const nu_mat4_t  *transform)
{
    const nu__model_command_t *cmds
        = nu__model_commands_data_const(&model->commands);
    for (nu_size_t i = 0; i < nu__model_commands_size(&model->commands); ++i)
    {
        const nu_material_t *material
            = &nu__model_items_at_const(&model->items, cmds[i].material)
                   ->material;
        const nu_mesh_t *mesh
            = &nu__model_items_at_const(&model->items, cmds[i].mesh)->mesh;
        nu_mat4_t global_transform = nu_mat4_mul(*transform, cmds[i].transform);
        nu_draw(renderer, pass, mesh, material, &global_transform);
    }
}

#endif

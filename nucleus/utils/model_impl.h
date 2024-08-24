#ifndef NU_MODEL_IMPL_H
#define NU_MODEL_IMPL_H

#include <nucleus/utils/model.h>
#include <nucleus/utils/loader.h>

typedef struct
{
    nu_model_t     *model;
    nu_renderer_t  *renderer;
    nu_allocator_t *alloc;
    nu_logger_t    *logger;
    nu_u32_t        default_material_item;
} nuext__gltf_to_model_userdata_t;

static nu_error_t
nuext__gltf_to_model_callback (const nuext_gltf_asset_t *asset, void *userdata)
{
    nuext__gltf_to_model_userdata_t *data = userdata;
    nu_vec_push(&data->model->items, data->alloc);
    nu__model_item_t *item = nu_vec_last(&data->model->items);
    NU_CHECK(item, return NU_ERROR_ALLOCATION);
    switch (asset->type)
    {
        case NUEXT_GLTF_ASSET_MESH: {
            NU_DEBUG(data->logger, "load mesh %lu", asset->id);
            nu_mesh_info_t info;
            info.positions = asset->mesh.positions;
            info.uvs       = asset->mesh.uvs;
            info.normals   = asset->mesh.normals;
            info.count     = asset->mesh.count;
            nu_error_t error
                = nu_mesh_create(data->renderer, &info, &item->mesh);
            NU_ERROR_CHECK(error, return error);
            item->id = asset->id;
        }
        break;
        case NUEXT_GLTF_ASSET_TEXTURE: {
            NU_DEBUG(data->logger, "load texture %lu", asset->id);
            nu_texture_info_t info;
            info.size   = asset->texture.size;
            info.usage  = NU_TEXTURE_USAGE_SAMPLE;
            info.format = NU_TEXTURE_FORMAT_COLOR;
            info.colors = asset->texture.data;
            nu_error_t error
                = nu_texture_create(data->renderer, &info, &item->texture);
            NU_ERROR_CHECK(error, return error);
            item->id = asset->id;
        }
        break;
        case NUEXT_GLTF_ASSET_MATERIAL: {
            NU_DEBUG(data->logger, "load material %lu", asset->id);
            const nu_texture_handle_t *diffuse_tex = NU_NULL;
            const nu__model_item_t    *items       = data->model->items.data;
            for (nu_size_t i = 0; i < data->model->items.size; ++i)
            {
                if (items[i].id == asset->material.diffuse_id)
                {
                    diffuse_tex = &items[i].texture;
                    break;
                }
            }
            if (!diffuse_tex)
            {
                NU_ERROR(data->logger, "diffuse texture not found");
                return NU_ERROR_RESOURCE_LOADING;
            }
            nu_material_info_t info
                = nu_material_info_default(NU_MATERIAL_MESH);
            info.mesh.color0 = diffuse_tex;
            nu_error_t error
                = nu_material_create(data->renderer, &info, &item->material);
            NU_ERROR_CHECK(error, return error);
            item->id = asset->id;
        }
        break;
        case NUEXT_GLTF_ASSET_NODE: {
            NU_DEBUG(data->logger, "load node %lu", asset->id);
            nu_vec_push(&data->model->commands, data->alloc);
            nu__model_command_t *cmd = nu_vec_last(&data->model->commands);
            NU_CHECK(cmd, return NU_ERROR_ALLOCATION);
            cmd->transform                = asset->node.transform;
            cmd->material                 = -1;
            cmd->mesh                     = -1;
            const nu__model_item_t *items = data->model->items.data;
            for (nu_size_t i = 0; i < data->model->items.size; ++i)
            {
                if (items[i].id == asset->node.mesh_id)
                {
                    cmd->mesh = i;
                }
                if (asset->node.material_id
                    && items[i].id == asset->node.material_id)
                {
                    cmd->material = i;
                }
            }
            if (cmd->mesh == (nu_u16_t)-1)
            {
                NU_ERROR(
                    data->logger, "mesh not found %lu", asset->node.mesh_id);
                return NU_ERROR_RESOURCE_LOADING;
            }
            if (cmd->material == (nu_u16_t)-1)
            {
                NU_ERROR(data->logger,
                         "material not found %lu, using default one",
                         asset->node.material_id);
                cmd->material = data->default_material_item;
            }
            item->id = asset->id;
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
    nu_vec_init(&model->items, alloc, 32);
    nu_vec_init(&model->commands, alloc, 32);
    nuext__gltf_to_model_userdata_t userdata;
    userdata.alloc    = alloc;
    userdata.model    = model;
    userdata.logger   = logger;
    userdata.renderer = renderer;
    {
        nu_vec_push(&model->items, alloc);
        nu_vec_push(&model->items, alloc);
        nu__model_item_t *mat_item = model->items.data + 0;
        nu__model_item_t *tex_item = model->items.data + 1;
        mat_item->id               = 0;
        tex_item->id               = 1;
        nu_texture_info_t tinfo;
        nu_color_t        white = NU_COLOR_WHITE;
        tinfo.size              = nu_uvec2(1, 1);
        tinfo.usage             = NU_TEXTURE_USAGE_SAMPLE;
        tinfo.format            = NU_TEXTURE_FORMAT_COLOR;
        tinfo.colors            = &white;
        nu_texture_create(renderer, &tinfo, &tex_item->texture);
        nu_material_info_t minfo = nu_material_info_default(NU_MATERIAL_MESH);
        minfo.mesh.color0        = &tex_item->texture;
        nu_material_create(renderer, &minfo, &mat_item->material);
        userdata.default_material_item = 0;
    }
    nu_error_t error = nuext_load_gltf(
        filename, logger, alloc, nuext__gltf_to_model_callback, &userdata);
    NU_ERROR_CHECK(error, return error);
    return NU_ERROR_NONE;
}
void
nu_model_free (nu_model_t *model, nu_allocator_t *alloc)
{
    nu_vec_free(&model->items, alloc);
    nu_vec_free(&model->commands, alloc);
}
void
nu_model_draw (nu_renderer_t         *renderer,
               nu_renderpass_handle_t pass,
               const nu_model_t      *model,
               nu_mat4_t              transform)
{
    const nu__model_command_t *cmds = model->commands.data;
    for (nu_size_t i = 0; i < model->commands.size; ++i)
    {
        if (cmds[i].material == (nu_u16_t)-1 || cmds[i].mesh == (nu_u16_t)-1)
        {
            continue;
        }
        const nu_material_handle_t material
            = model->items.data[cmds[i].material].material;
        const nu_mesh_handle_t mesh = model->items.data[cmds[i].mesh].mesh;
        nu_mat4_t global_transform  = nu_mat4_mul(transform, cmds[i].transform);
        nu_draw(renderer, pass, material, mesh, global_transform);
    }
}

#endif

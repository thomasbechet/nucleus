#ifndef NU_MODEL_IMPL_H
#define NU_MODEL_IMPL_H

#include <nucleus/utils/model.h>
#include <nucleus/utils/loader.h>

NU_DECLARE_ARRAY(nu_model_textures, nu_texture_t)
NU_DECLARE_ARRAY(nu_model_materials, nu_material_t)
NU_DECLARE_ARRAY(nu_model_meshes, nu_mesh_t)
NU_DECLARE_ARRAY(nu_model_commands, nu_model_command_t)

typedef struct
{
    nu_model_t    *model;
    nu_renderer_t *renderer;
    nu_logger_t   *logger;
} nuext__gltf_to_model_userdata_t;

static nu_error_t
nuext__gltf_to_model_callback (const nuext_gltf_asset_t *asset, void *userdata)
{
    nuext__gltf_to_model_userdata_t *data = userdata;
    switch (asset->type)
    {
        case NUEXT_GLTF_ASSET_MESH:
            break;
        case NUEXT_GLTF_ASSET_TEXTURE:
            break;
        case NUEXT_GLTF_ASSET_NODE:
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
    return NU_ERROR_NONE;
}
void
nu_model_free (nu_model_t *model, nu_allocator_t *alloc)
{
    nu_model_textures_free(&model->textures, alloc);
    nu_model_materials_free(&model->materials, alloc);
    nu_model_meshes_free(&model->meshes, alloc);
    nu_model_commands_free(&model->commands, alloc);
}
void
nu_model_draw (nu_renderer_t    *renderer,
               nu_renderpass_t  *pass,
               const nu_model_t *model,
               const nu_mat4_t  *transform)
{
    const nu_model_command_t *cmds
        = nu_model_commands_data_const(&model->commands);
    for (nu_size_t i = 0; i < nu_model_commands_size(&model->commands); ++i)
    {
        const nu_material_t *material
            = nu_model_materials_at_const(&model->materials, cmds[i].material);
        const nu_mesh_t *mesh
            = nu_model_meshes_at_const(&model->meshes, cmds[i].mesh);
        nu_mat4_t global_transform = nu_mat4_mul(*transform, cmds[i].transform);
        nu_draw(renderer, pass, mesh, material, &global_transform);
    }
}

#endif

#ifndef NU_MODEL_H
#define NU_MODEL_H

#include <nucleus/graphics.h>

NU_DEFINE_ARRAY(nu_model_textures, nu_texture_t)
NU_DEFINE_ARRAY(nu_model_materials, nu_material_t)
NU_DEFINE_ARRAY(nu_model_meshes, nu_mesh_t)

typedef struct
{
    nu_u16_t  mesh;
    nu_u16_t  material;
    nu_mat4_t transform;
} nu_model_command_t;

NU_DEFINE_ARRAY(nu_model_commands, nu_model_command_t)

typedef struct
{
    nu_model_textures_t  textures;
    nu_model_materials_t materials;
    nu_model_meshes_t    meshes;
    nu_model_commands_t  commands;
} nu_model_t;

NU_API nu_error_t nuext_model_from_gltf(const nu_char_t *filename,
                                        nu_renderer_t   *renderer,
                                        nu_logger_t     *logger,
                                        nu_allocator_t  *alloc,
                                        nu_model_t      *model);
NU_API void       nu_model_free(nu_model_t *model, nu_allocator_t *alloc);
NU_API void       nu_model_draw(nu_renderer_t    *renderer,
                                nu_renderpass_t  *pass,
                                const nu_model_t *model,
                                const nu_mat4_t  *transform);

#endif

#ifndef NU_LOADER_H
#define NU_LOADER_H

#include <nucleus/core.h>
#include <nucleus/graphics.h>

typedef struct
{
    const nu_char_t *name;
    const nu_vec3_t *positions;
    const nu_vec2_t *uvs;
    const nu_vec3_t *normals;
    nu_size_t        count;
} nuext_gltf_mesh_t;

typedef struct
{
    const nu_char_t  *name;
    nu_uvec2_t        size;
    const nu_color_t *data;
} nuext_gltf_texture_t;

typedef struct
{
    const nu_char_t *name;
    const nu_char_t *mesh;
    const nu_char_t *material;
    nu_mat4_t        transform;
} nuext_gltf_node_t;

typedef enum
{
    NUEXT_GLTF_ASSET_MESH,
    NUEXT_GLTF_ASSET_TEXTURE,
    NUEXT_GLTF_ASSET_MATERIAL,
    NUEXT_GLTF_ASSET_NODE,
} nuext_gltf_asset_type_t;

typedef struct
{
    nuext_gltf_asset_type_t type;
    union
    {
        nuext_gltf_node_t node;
        nuext_gltf_mesh_t mesh;
    };
} nuext_gltf_asset_t;

typedef nu_error_t (*nuext_gltf_callback_t)(const nuext_gltf_asset_t *asset,
                                            void                     *userdata);

NU_API nu_error_t nuext_load_gltf(const nu_char_t      *filename,
                                  nu_logger_t          *logger,
                                  nu_allocator_t       *allocator,
                                  nuext_gltf_callback_t callback,
                                  void                 *userdata);

NU_API nu_error_t nuext_load_image_memory(const nu_byte_t *data,
                                          nu_size_t        data_size,
                                          nu_allocator_t  *allocator,
                                          nu_uvec2_t      *size,
                                          nu_color_t     **colors);
NU_API nu_error_t nuext_load_image(const nu_char_t *filename,
                                   nu_allocator_t  *allocator,
                                   nu_uvec2_t      *size,
                                   nu_color_t     **colors);

#endif

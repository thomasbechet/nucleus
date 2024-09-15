#ifndef NU_IMPORTER_MODEL_H
#define NU_IMPORTER_MODEL_H

#include <nucleus/internal.h>

#ifdef NU_BUILD_CGLTF
#define CGLTF_IMPLEMENTATION
#include <nucleus/external/cgltf/cgltf.h>
#endif

static void
nu__emplace_vertex (const nu_vec3_t *positions,
                    const nu_vec2_t *uvs,
                    const nu_vec3_t *normals,
                    nu_vec3_t       *buf_positions,
                    nu_vec2_t       *buf_uvs,
                    nu_vec3_t       *buf_normals,
                    nu_u32_t         vertex_index,
                    nu_u32_t         index)
{

    buf_positions[vertex_index] = positions[index];
    if (uvs)
    {
        buf_uvs[vertex_index] = uvs[index];
    }
    else
    {
        buf_uvs[vertex_index] = NU_VEC2_ZERO;
    }
    if (normals)
    {
        buf_normals[vertex_index] = normals[index];
    }
    else
    {
        buf_normals[vertex_index] = NU_VEC3_ZERO;
    }
}

#define NU__EMPLACE_VERTEX(index_type)                            \
    index_type *indices                                           \
        = (index_type *)(data + view->offset + accessor->offset); \
    for (nu_size_t i = 0; i < indice_count; ++i)                  \
    {                                                             \
        nu__emplace_vertex(positions,                             \
                           uvs,                                   \
                           normals,                               \
                           buf_positions,                         \
                           buf_uvs,                               \
                           buf_normals,                           \
                           i,                                     \
                           indices[i]);                           \
    }

static nu_error_t
nu__load_mesh (nu__model_gltf_loader_t *loader,
               const cgltf_mesh        *mesh,
               nu__model_t             *model)
{
    nu_error_t error;
    nu_debug("loading mesh: %s", mesh->name);
    for (nu_size_t p = 0; p < mesh->primitives_count; ++p)
    {
        cgltf_primitive *primitive = mesh->primitives + p;

        // Access attributes
        const nu_vec3_t *positions = NU_NULL;
        const nu_vec2_t *uvs       = NU_NULL;
        const nu_vec3_t *normals   = NU_NULL;
        for (nu_size_t a = 0; a < primitive->attributes_count; ++a)
        {
            cgltf_attribute   *attribute = primitive->attributes + a;
            cgltf_accessor    *accessor  = attribute->data;
            cgltf_buffer_view *view      = accessor->buffer_view;
            nu_byte_t         *data      = (nu_byte_t *)view->buffer->data;
            switch (attribute->type)
            {
                case cgltf_attribute_type_position:
                    positions
                        = (nu_vec3_t *)(data + accessor->offset + view->offset);
                    break;
                case cgltf_attribute_type_texcoord:
                    uvs = (nu_vec2_t *)(data + accessor->offset + view->offset);
                    break;
                case cgltf_attribute_type_normal:
                    normals
                        = (nu_vec3_t *)(data + accessor->offset + view->offset);
                    break;
                default:
                    break;
            }
        }

        // Build buffers
        {
            cgltf_accessor    *accessor     = primitive->indices;
            cgltf_buffer_view *view         = accessor->buffer_view;
            nu_byte_t         *data         = (nu_byte_t *)view->buffer->data;
            nu_size_t          indice_count = accessor->count;

            nu_vec3_t *buf_positions = NU_NULL;
            nu_vec2_t *buf_uvs       = NU_NULL;
            nu_vec3_t *buf_normals   = NU_NULL;
            buf_positions
                = (nu_vec3_t *)nu_alloc(sizeof(*buf_positions) * indice_count);
            buf_uvs = (nu_vec2_t *)nu_alloc(sizeof(*buf_uvs) * indice_count);
            buf_normals
                = (nu_vec3_t *)nu_alloc(sizeof(*buf_normals) * indice_count);

            switch (accessor->component_type)
            {
                case cgltf_component_type_r_8:
                case cgltf_component_type_r_8u: {
                    NU__EMPLACE_VERTEX(nu_u8_t)
                }
                break;
                case cgltf_component_type_r_16:
                case cgltf_component_type_r_16u: {
                    NU__EMPLACE_VERTEX(nu_u16_t)
                }
                break;
                case cgltf_component_type_r_32f:
                case cgltf_component_type_r_32u: {
                    NU__EMPLACE_VERTEX(nu_u32_t)
                }
                break;
                default:
                    break;
            }

            // Create mesh
            nu_mesh_t handle = nu_mesh_create(indice_count);
            nu_mesh_update(handle, buf_positions, buf_uvs, buf_normals);

            // Free resources
            nu_free(buf_positions, sizeof(*buf_positions) * indice_count);
            nu_free(buf_uvs, sizeof(*buf_uvs) * indice_count);
            nu_free(buf_normals, sizeof(*buf_normals) * indice_count);

            // Append asset
            nu__model_asset_t *asset      = nu_vec_push(&model->assets);
            asset->mesh                   = handle;
            nu__model_gltf_cache_t *cache = nu_vec_push(&loader->_cache);
            cache->ptr                    = mesh;
            cache->index                  = model->assets.size - 1;
        }
    }

    return NU_ERROR_NONE;
}
static nu_error_t
nu__load_texture (nu__model_gltf_loader_t *loader,
                  const cgltf_texture     *texture,
                  nu__model_t             *model)
{
    nu_debug("loading texture: %s", texture->name);

    cgltf_buffer_view *tview = texture->image->buffer_view;

    nu_image_t image = nuext_image_load_memory(
        (const nu_byte_t *)tview->buffer->data + tview->offset, tview->size);

    // Create texture
    nu_texture_t handle = nu_texture_create_image(image);
    nu_image_delete(image);

    // Append asset
    nu__model_asset_t *asset      = nu_vec_push(&model->assets);
    asset->texture                = handle;
    nu__model_gltf_cache_t *cache = nu_vec_push(&loader->_cache);
    cache->ptr                    = texture;
    cache->index                  = model->assets.size - 1;

    return NU_ERROR_NONE;
}
static nu_error_t
nu__load_material (nu__model_gltf_loader_t *loader,
                   const cgltf_material    *material,
                   nu__model_t             *model)
{
    nu_debug("loading material: %s", material->name);

    // Find color texture
    nu_bool_t found = NU_FALSE;
    nu_u32_t  index;
    for (nu_size_t i = 0; i < loader->_cache.size; ++i)
    {
        if (loader->_cache.data[i].ptr
            == material->pbr_metallic_roughness.base_color_texture.texture)
        {
            index = loader->_cache.data[i].index;
            found = NU_TRUE;
            break;
        }
    }

    if (!found)
    {
        NU_ERROR("texture not found");
        return NU_ERROR_RESOURCE_LOADING;
    }

    // Create material
    nu_material_info_t info = nu_material_info_default(NU_MATERIAL_MESH);
    info.mesh.color0        = &model->assets.data[index].texture;
    nu_material_t handle    = nu_material_create(&info);

    // Append asset
    nu__model_asset_t *asset      = nu_vec_push(&model->assets);
    asset->material               = handle;
    nu__model_gltf_cache_t *cache = nu_vec_push(&loader->_cache);
    cache->ptr                    = material;
    cache->index                  = model->assets.size - 1;

    return NU_ERROR_NONE;
}
static nu_error_t
nu__load_material_default (nu__model_gltf_loader_t *loader, nu__model_t *model)
{
    if (!loader->_has_default_material)
    {
        nu_debug("loading default material");

        nu_texture_t texture = nu_texture_create_color(NU_COLOR_RED);
        nu_vec_push(&model->assets)->texture = texture;

        nu_material_info_t info = nu_material_info_default(NU_MATERIAL_MESH);
        info.mesh.color0        = &texture;
        nu_material_t material  = nu_material_create(&info);
        nu_vec_push(&model->assets)->material = material;

        loader->_default_material     = model->assets.size - 1;
        loader->_has_default_material = NU_TRUE;
    }
    return NU_ERROR_NONE;
}

static void
nu__model_gltf_loader_init (void)
{
    nu_vec_init(10, &_ctx.importer.model_gltf_loader._cache);
}
static void
nu__model_gltf_loader_free (void)
{
    nu_vec_free(&_ctx.importer.model_gltf_loader._cache);
}
static nu_model_t
nu__model_gltf_load (nu__model_gltf_loader_t *loader, const nu_char_t *filename)
{
    cgltf_options options;
    nu_memset(&options, 0, sizeof(options));
    // TODO: custom allocator
    cgltf_data  *data = NU_NULL;
    cgltf_result result;
    nu_error_t   error;

    // Reset cache
    nu_vec_clear(&loader->_cache);
    loader->_has_default_material = NU_FALSE;

    // Parse file and load buffers
    result = cgltf_parse_file(&options, filename, &data);
    if (result != cgltf_result_success)
    {
        return NU_NULL;
    }
    result = cgltf_load_buffers(&options, data, filename);
    if (result != cgltf_result_success)
    {
        return NU_NULL;
    }

    // Create model
    nu_model_t   handle = nu_model_create();
    nu__model_t *m      = &_ctx.graphics.models.data[nu_handle_index(handle)];

    // Load resources
    for (nu_size_t i = 0; i < data->meshes_count; ++i)
    {
        error = nu__load_mesh(loader, data->meshes + i, m);
        nu_error_check(error, return NU_NULL);
    }
    for (nu_size_t i = 0; i < data->textures_count; ++i)
    {
        error = nu__load_texture(loader, data->textures + i, m);
        nu_error_check(error, return NU_NULL);
    }
    for (nu_size_t i = 0; i < data->materials_count; ++i)
    {
        const cgltf_material *mat = data->materials + i;
        if (mat->has_pbr_metallic_roughness
            && mat->pbr_metallic_roughness.base_color_texture.texture)
        {
            error = nu__load_material(loader, mat, m);
            nu_error_check(error, return NU_NULL);
        }
    }

    // Load scenes and nodes
    for (nu_size_t s = 0; s < data->scenes_count; ++s)
    {
        cgltf_scene *scene = data->scenes + s;

        for (nu_size_t n = 0; n < scene->nodes_count; ++n)
        {
            cgltf_node *node = scene->nodes[n];
            nu_debug("loading node: %s", node->name);

            nu_mat4_t transform = nu_mat4_identity();
            if (node->has_scale)
            {
                transform = nu_mat4_mul(nu_mat4_scale(nu_vec3(node->scale[0],
                                                              node->scale[1],
                                                              node->scale[2])),
                                        transform);
            }
            if (node->has_rotation)
            {
                nu_quat_t q = nu_quat(node->rotation[0],
                                      node->rotation[1],
                                      node->rotation[2],
                                      node->rotation[3]);
                transform   = nu_quat_mulm4(q, transform);
            }
            if (node->has_translation)
            {
                transform = nu_mat4_mul(
                    nu_mat4_translate(nu_vec3(node->translation[0],
                                              node->translation[1],
                                              node->translation[2])),
                    transform);
            }

            if (node->mesh)
            {
                // Find material
                nu_u16_t material_index;
                {
                    nu_bool_t found = NU_FALSE;
                    for (nu_size_t i = 0; i < loader->_cache.size; ++i)
                    {
                        if (loader->_cache.data[i].ptr
                            == node->mesh->primitives->material)
                        {
                            material_index = loader->_cache.data[i].index;
                            found          = NU_TRUE;
                            break;
                        }
                    }
                    if (!found)
                    {
                        nu_warning("material not found, using default");
                        nu__load_material_default(loader, m);
                        material_index = loader->_default_material;
                    }
                }

                // Find mesh
                nu_u16_t mesh_index;
                {
                    nu_bool_t found = NU_FALSE;
                    for (nu_size_t i = 0; i < loader->_cache.size; ++i)
                    {
                        if (loader->_cache.data[i].ptr == node->mesh)
                        {
                            mesh_index = loader->_cache.data[i].index;
                            found      = NU_TRUE;
                            break;
                        }
                    }
                    if (!found)
                    {
                        return NU_NULL;
                    }
                }

                // Append node
                nu__model_node_t *node = nu_vec_push(&m->nodes);
                node->mesh             = mesh_index;
                node->material         = material_index;
                node->transform        = transform;
            }
        }
    }

    cgltf_free(data);
    return handle;
}

nu_model_t
nuext_model_load_filename (const nu_char_t *filename)
{
#ifdef NU_BUILD_CGLTF
    return nu__model_gltf_load(&_ctx.importer.model_gltf_loader, filename);
#endif
    return NU_NULL;
}

#endif

#ifndef NU_LOADER_IMPL_H
#define NU_LOADER_IMPL_H

#include <nucleus/utils/loader.h>

#define CGLTF_IMPLEMENTATION
#include <nucleus/external/cgltf/cgltf.h>
#define STB_IMAGE_IMPLEMENTATION
#include <nucleus/external/stb/stb_image.h>

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
nu__load_mesh (nu_gltf_loader_t *loader,
               const cgltf_mesh *mesh,
               nu_allocator_t   *alloc,
               nu_renderer_t     renderer,
               nu_model_t       *model)
{
    nu_error_t error;
    NU_DEBUG(loader->_logger, "loading mesh: %s", mesh->name);
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
            nu_char_t         *data      = view->buffer->data;
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
            nu_char_t         *data         = view->buffer->data;
            nu_size_t          indice_count = accessor->count;

            nu_vec3_t *buf_positions = NU_NULL;
            nu_vec2_t *buf_uvs       = NU_NULL;
            nu_vec3_t *buf_normals   = NU_NULL;
            buf_positions            = nu_alloc(loader->_allocator,
                                     sizeof(*buf_positions) * indice_count);
            buf_uvs
                = nu_alloc(loader->_allocator, sizeof(*buf_uvs) * indice_count);
            buf_normals = nu_alloc(loader->_allocator,
                                   sizeof(*buf_normals) * indice_count);

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
            nu_mesh_handle_t handle;
            nu_mesh_info_t   info;
            info.positions = buf_positions;
            info.uvs       = buf_uvs;
            info.normals   = buf_normals;
            info.count     = indice_count;
            error          = nu_mesh_create(renderer, &info, &handle);
            NU_ERROR_ASSERT(error);

            // Free resources
            nu_free(loader->_allocator,
                    buf_positions,
                    sizeof(*buf_positions) * indice_count);
            nu_free(
                loader->_allocator, buf_uvs, sizeof(*buf_uvs) * indice_count);
            nu_free(loader->_allocator,
                    buf_normals,
                    sizeof(*buf_normals) * indice_count);
            NU_ERROR_ASSERT(error);

            // Append asset
            nu__model_asset_t *asset = nu_vec_push(&model->assets, alloc);
            asset->mesh              = handle;
            nu__gltf_model_cache_t *cache
                = nu_vec_push(&loader->_cache, loader->_allocator);
            cache->ptr   = mesh;
            cache->index = model->assets.size - 1;
        }
    }

    return NU_ERROR_NONE;
}
static nu_error_t
nu__load_texture (nu_gltf_loader_t    *loader,
                  const cgltf_texture *texture,
                  nu_allocator_t      *alloc,
                  nu_renderer_t        renderer,
                  nu_model_t          *model)
{
    NU_DEBUG(loader->_logger, "loading texture: %s", texture->name);

    cgltf_buffer_view *tview = texture->image->buffer_view;

    nu_image_t image;
    nu_error_t error = nu_load_image_memory(
        (const nu_byte_t *)tview->buffer->data + tview->offset,
        tview->size,
        loader->_allocator,
        &image);
    NU_ERROR_CHECK(error, return error);

    // Create texture
    nu_texture_handle_t handle;
    nu_texture_info_t   info;
    info.size   = image.size;
    info.usage  = NU_TEXTURE_USAGE_SAMPLE;
    info.format = NU_TEXTURE_FORMAT_COLOR;
    info.colors = image.data;
    error       = nu_texture_create(renderer, &info, &handle);
    NU_ERROR_ASSERT(error);
    nu_image_free(&image, loader->_allocator);

    // Append asset
    nu__model_asset_t *asset = nu_vec_push(&model->assets, alloc);
    asset->texture           = handle;
    nu__gltf_model_cache_t *cache
        = nu_vec_push(&loader->_cache, loader->_allocator);
    cache->ptr   = texture;
    cache->index = model->assets.size - 1;

    return error;
}
static nu_error_t
nu__load_material (nu_gltf_loader_t     *loader,
                   const cgltf_material *material,
                   nu_allocator_t       *alloc,
                   nu_renderer_t         renderer,
                   nu_model_t           *model)
{
    NU_DEBUG(loader->_logger, "loading material: %s", material->name);

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
        NU_ERROR(loader->_logger, "texture not found");
        return NU_ERROR_RESOURCE_LOADING;
    }

    // Create material
    nu_material_handle_t handle;
    nu_material_info_t   info = NU_MATERIAL_INFO_DEFAULT_MESH;
    info.mesh.color0          = &model->assets.data[index].texture;
    nu_error_t error          = nu_material_create(renderer, &info, &handle);
    NU_ERROR_ASSERT(error);

    // Append asset
    nu__model_asset_t *asset = nu_vec_push(&model->assets, alloc);
    asset->material          = handle;
    nu__gltf_model_cache_t *cache
        = nu_vec_push(&loader->_cache, loader->_allocator);
    cache->ptr   = material;
    cache->index = model->assets.size - 1;

    return NU_ERROR_NONE;
}
static nu_error_t
nu__load_material_default (nu_gltf_loader_t *loader,
                           nu_allocator_t   *alloc,
                           nu_renderer_t     renderer,
                           nu_model_t       *model)
{
    if (!loader->_has_default_material)
    {
        NU_DEBUG(loader->_logger, "loading default material");

        nu_texture_handle_t texture;
        nu_texture_create_color(renderer, NU_COLOR_RED, &texture);
        nu_vec_push(&model->assets, alloc)->texture = texture;

        nu_material_handle_t material;
        nu_material_info_t   info = NU_MATERIAL_INFO_DEFAULT_MESH;
        info.mesh.color0          = &texture;
        nu_material_create(renderer, &info, &material);
        nu_vec_push(&model->assets, alloc)->material = material;

        loader->_default_material     = model->assets.size - 1;
        loader->_has_default_material = NU_TRUE;
    }
    return NU_ERROR_NONE;
}

nu_error_t
nu_gltf_loader_init (nu_allocator_t   *alloc,
                     nu_logger_t      *logger,
                     nu_gltf_loader_t *loader)
{
    loader->_allocator = alloc;
    loader->_logger    = logger;
    nu_vec_init(&loader->_cache, alloc, 10);
    return NU_ERROR_NONE;
}
void
nu_gltf_loader_free (nu_gltf_loader_t *loader)
{
    nu_vec_free(&loader->_cache, loader->_allocator);
}
nu_error_t
nuext_gltf_load_model_filename (nu_gltf_loader_t *loader,
                                const nu_char_t  *filename,
                                nu_allocator_t   *alloc,
                                nu_renderer_t     renderer,
                                nu_model_t       *model)
{
    cgltf_options options = { 0 };
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
        return NU_ERROR_RESOURCE_LOADING;
    }
    result = cgltf_load_buffers(&options, data, filename);
    if (result != cgltf_result_success)
    {
        return NU_ERROR_RESOURCE_LOADING;
    }

    // Create model
    nu_model_init(alloc, model);

    // Load resources
    for (nu_size_t i = 0; i < data->meshes_count; ++i)
    {
        error = nu__load_mesh(loader, data->meshes + i, alloc, renderer, model);
        NU_ERROR_CHECK(error, return error);
    }
    for (nu_size_t i = 0; i < data->textures_count; ++i)
    {
        error = nu__load_texture(
            loader, data->textures + i, alloc, renderer, model);
        NU_ERROR_CHECK(error, return error);
    }
    for (nu_size_t i = 0; i < data->materials_count; ++i)
    {
        const cgltf_material *mat = data->materials + i;
        if (mat->has_pbr_metallic_roughness
            && mat->pbr_metallic_roughness.base_color_texture.texture)
        {
            error = nu__load_material(loader, mat, alloc, renderer, model);
            NU_ERROR_CHECK(error, return error);
        }
    }

    // Load scenes and nodes
    for (nu_size_t s = 0; s < data->scenes_count; ++s)
    {
        cgltf_scene *scene = data->scenes + s;

        for (nu_size_t n = 0; n < scene->nodes_count; ++n)
        {
            cgltf_node *node = scene->nodes[n];
            NU_DEBUG(loader->_logger, "loading node: %s", node->name);

            nu_mat4_t transform = nu_mat4_identity();
            if (node->has_scale)
            {
                transform = nu_mat4_mul(nu_mat4_scale(node->scale[0],
                                                      node->scale[1],
                                                      node->scale[2]),
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
                transform = nu_mat4_mul(nu_mat4_translate(node->translation[0],
                                                          node->translation[1],
                                                          node->translation[2]),
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
                        NU_WARNING(loader->_logger,
                                   "material not found, using default");
                        nu__load_material_default(
                            loader, alloc, renderer, model);
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
                        return NU_ERROR_RESOURCE_LOADING;
                    }
                }

                // Append node
                nu__model_node_t *node = nu_vec_push(&model->nodes, alloc);
                node->mesh             = mesh_index;
                node->material         = material_index;
                node->transform        = transform;
            }
        }
    }

    cgltf_free(data);
    return NU_ERROR_NONE;
}

static nu_color_t *
nu__parse_colors (const nu_byte_t *img,
                  nu_uvec2_t       size,
                  nu_size_t        comp,
                  nu_allocator_t  *alloc)
{
    nu_color_t *colors = nu_alloc(alloc, sizeof(nu_color_t) * size.x * size.y);
    NU_CHECK(colors, return NU_NULL);
    for (nu_size_t i = 0; i < (size.x * size.y); ++i)
    {
        for (nu_size_t n = 0; n < comp; ++n)
        {
        }
        colors[i].r = img[i * comp + 0];
        colors[i].g = img[i * comp + 1];
        colors[i].b = img[i * comp + 2];
        if (comp == 4)
        {
            colors[i].a = img[i * comp + 3];
        }
        else
        {
            colors[i].a = 255;
        }
    }
    return colors;
}
nu_error_t
nuext_load_image_filename (const nu_char_t *filename,
                           nu_allocator_t  *allocator,
                           nu_image_t      *image)
{
    int            w, h, n;
    unsigned char *img = stbi_load(filename, &w, &h, &n, STBI_default);
    NU_CHECK(img, return NU_ERROR_RESOURCE_LOADING);
    image->size = nu_uvec2(w, h);
    image->data = nu__parse_colors(img, image->size, n, allocator);
    stbi_image_free(img);
    NU_CHECK(image->data, return NU_ERROR_ALLOCATION);
    return NU_ERROR_NONE;
}
nu_error_t
nu_load_image_memory (const nu_byte_t *data,
                      nu_size_t        data_size,
                      nu_allocator_t  *allocator,
                      nu_image_t      *image)
{
    int            w, h, n;
    unsigned char *img
        = stbi_load_from_memory(data, data_size, &w, &h, &n, STBI_default);
    NU_CHECK(img, return NU_ERROR_RESOURCE_LOADING);
    image->size = nu_uvec2(w, h);
    image->data = nu__parse_colors(img, image->size, n, allocator);
    stbi_image_free(img);
    NU_CHECK(image->data, return NU_ERROR_ALLOCATION);
    return NU_ERROR_NONE;
}

#endif

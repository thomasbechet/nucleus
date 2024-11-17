#ifndef NU_IMPORTER_MODEL_H
#define NU_IMPORTER_MODEL_H

#include <nucleus/internal.h>

#ifdef NU_BUILD_IMPORTER_CGLTF
#define CGLTF_IMPLEMENTATION
#include <nucleus/external/cgltf/cgltf.h>
#endif

static void
nu__emplace_vertex (const nu_v3_t *positions,
                    const nu_v2_t *uvs,
                    nu_v3_t       *buf_positions,
                    nu_v2_t       *buf_uvs,
                    nu_u32_t       vertex_index,
                    nu_u32_t       index)
{

    buf_positions[vertex_index] = positions[index];
    if (uvs)
    {
        buf_uvs[vertex_index] = uvs[index];
    }
    else
    {
        buf_uvs[vertex_index] = NU_V2_ZEROS;
    }
}

#define NU__EMPLACE_VERTEX(index_type)                              \
    index_type *indices                                             \
        = (index_type *)(data + view->offset + accessor->offset);   \
    for (nu_size_t i = 0; i < indice_count; ++i)                    \
    {                                                               \
        nu__emplace_vertex(                                         \
            positions, uvs, buf_positions, buf_uvs, i, indices[i]); \
    }

static nu_error_t
nu__load_mesh (nu__model_gltf_loader_t *loader, const cgltf_mesh *mesh)
{
    nu_error_t error;
    NU_DEBUG("loading mesh: %s", mesh->name);
    for (nu_size_t p = 0; p < mesh->primitives_count; ++p)
    {
        cgltf_primitive *primitive = mesh->primitives + p;

        // Access attributes
        const nu_v3_t *positions = NU_NULL;
        const nu_v2_t *uvs       = NU_NULL;
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
                        = (nu_v3_t *)(data + accessor->offset + view->offset);
                    break;
                case cgltf_attribute_type_texcoord:
                    uvs = (nu_v2_t *)(data + accessor->offset + view->offset);
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

            nu_v3_t *buf_positions = NU_NULL;
            nu_v2_t *buf_uvs       = NU_NULL;
            nu_v3_t *buf_normals   = NU_NULL;
            buf_positions
                = (nu_v3_t *)nu_alloc(sizeof(*buf_positions) * indice_count);
            buf_uvs = (nu_v2_t *)nu_alloc(sizeof(*buf_uvs) * indice_count);

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
            nu_size_t primitive_count = indice_count / 3;
            nu_mesh_t handle          = nu_mesh_new(
                loader->scope, NU_PRIMITIVE_TRIANGLES, primitive_count);
            nu_mesh_set_positions(handle, 0, primitive_count, buf_positions);
            nu_mesh_set_uvs(handle, 0, primitive_count, buf_uvs);

            // Free resources
            nu_free(buf_positions, sizeof(*buf_positions) * indice_count);
            nu_free(buf_uvs, sizeof(*buf_uvs) * indice_count);

            // Append asset
            nu__model_gltf_resource_t *cache = NU_VEC_PUSH(&loader->resources);
            cache->ptr                       = mesh;
            cache->handle                    = handle;
        }
    }

    return NU_ERROR_NONE;
}
static nu_error_t
nu__load_texture (nu__model_gltf_loader_t *loader, const cgltf_texture *texture)
{
    NU_DEBUG("loading texture: %s", texture->name);

    cgltf_buffer_view *tview = texture->image->buffer_view;

    // TODO: avoid temporary image ?
    nu_image_t image = nuext_image_load_memory(
        loader->scope,
        (const nu_byte_t *)tview->buffer->data + tview->offset,
        tview->size);

    // Create texture
    nu_texture_t handle
        = nu_texture_new_from_image(loader->scope, NU_TEXTURE_COLORMAP, image);

    // Append asset
    nu__model_gltf_resource_t *cache = NU_VEC_PUSH(&loader->resources);
    cache->ptr                       = texture;
    cache->handle                    = handle;

    return NU_ERROR_NONE;
}
static nu_error_t
nu__load_material (nu__model_gltf_loader_t *loader,
                   const cgltf_material    *material)
{
    NU_DEBUG("loading material: %s", material->name);

    // Find color texture
    nu_texture_t texture = NU_NULL;
    for (nu_size_t i = 0; i < loader->resources.size; ++i)
    {
        if (loader->resources.data[i].ptr
            == material->pbr_metallic_roughness.base_color_texture.texture)
        {
            texture = loader->resources.data[i].handle;
            break;
        }
    }

    if (!texture)
    {
        NU_ERROR("texture not found");
        return NU_ERROR_RESOURCE_LOADING;
    }

    // Create material
    nu_material_t handle = nu_material_new(loader->scope, NU_MATERIAL_SURFACE);
    nu_material_set_texture(handle, texture);

    // Append asset
    nu__model_gltf_resource_t *cache = NU_VEC_PUSH(&loader->resources);
    cache->ptr                       = material;
    cache->handle                    = handle;

    return NU_ERROR_NONE;
}
static void
nu__load_material_default (nu__model_gltf_loader_t *loader)
{
    if (!loader->default_material)
    {
        NU_DEBUG("loading default material");

        nu_texture_t texture
            = nu_texture_new_from_color(loader->scope, NU_COLOR_RED);
        nu_material_t material
            = nu_material_new(loader->scope, NU_MATERIAL_SURFACE);
        nu_material_set_texture(material, texture);

        loader->default_material = material;
    }
}

static void
nu__model_gltf_loader_init (void)
{
    NU_VEC_INIT(10, &_ctx.importer.model_gltf_loader.resources);
}
static void
nu__model_gltf_loader_free (void)
{
    NU_VEC_FREE(&_ctx.importer.model_gltf_loader.resources);
}
static nu_model_t
nu__model_gltf_load (nu_scope_t               scope,
                     nu__model_gltf_loader_t *loader,
                     nu_str_t                 filename)
{
    cgltf_options options;
    nu_memset(&options, 0, sizeof(options));
    // TODO: custom allocator
    cgltf_data  *data = NU_NULL;
    cgltf_result result;
    nu_error_t   error;

    // Reset cache
    NU_VEC_CLEAR(&loader->resources);
    loader->default_material = NU_NULL;
    loader->scope            = scope;

    // Parse file and load buffers
    nu_byte_t fn[256];
    nu_str_to_cstr(filename, fn, 256);
    result = cgltf_parse_file(&options, (char *)fn, &data);
    if (result != cgltf_result_success)
    {
        return NU_NULL;
    }
    result = cgltf_load_buffers(&options, data, (char *)fn);
    if (result != cgltf_result_success)
    {
        return NU_NULL;
    }

    // Load resources
    for (nu_size_t i = 0; i < data->meshes_count; ++i)
    {
        error = nu__load_mesh(loader, data->meshes + i);
        NU_ERROR_CHECK(error, return NU_NULL);
    }
    for (nu_size_t i = 0; i < data->textures_count; ++i)
    {
        error = nu__load_texture(loader, data->textures + i);
        NU_ERROR_CHECK(error, return NU_NULL);
    }
    for (nu_size_t i = 0; i < data->materials_count; ++i)
    {
        const cgltf_material *mat = data->materials + i;
        if (mat->has_pbr_metallic_roughness
            && mat->pbr_metallic_roughness.base_color_texture.texture)
        {
            error = nu__load_material(loader, mat);
            NU_ERROR_CHECK(error, return NU_NULL);
        }
    }

    nu_model_t model = NU_NULL;

    // Load scenes and nodes
    for (nu_size_t s = 0; s < data->scenes_count; ++s)
    {
        cgltf_scene *scene = data->scenes + s;

        // Create model
        model = nu_model_create(scene->nodes_count);

        for (nu_size_t n = 0; n < scene->nodes_count; ++n)
        {
            cgltf_node *node = scene->nodes[n];
            NU_DEBUG("loading node: %s", node->name);

            nu_m4_t transform = nu_m4_identity();
            if (node->has_scale)
            {
                transform = nu_m4_mul(
                    nu_m4_scale(
                        nu_v3(node->scale[0], node->scale[1], node->scale[2])),
                    transform);
            }
            if (node->has_rotation)
            {
                nu_q4_t q = nu_q4(node->rotation[0],
                                  node->rotation[1],
                                  node->rotation[2],
                                  node->rotation[3]);
                transform = nu_q4_mulm4(q, transform);
            }
            if (node->has_translation)
            {
                transform
                    = nu_m4_mul(nu_m4_translate(nu_v3(node->translation[0],
                                                      node->translation[1],
                                                      node->translation[2])),
                                transform);
            }

            if (node->mesh)
            {
                // Find material
                nu_material_t material = NU_NULL;
                {
                    for (nu_size_t i = 0; i < loader->resources.size; ++i)
                    {
                        if (loader->resources.data[i].ptr
                            == node->mesh->primitives->material)
                        {
                            material = loader->resources.data[i].handle;
                            break;
                        }
                    }
                    if (!material)
                    {
                        NU_WARNING(
                            "material not found for node '%s', using default",
                            node->name);
                        nu__load_material_default(loader);
                        material = loader->default_material;
                    }
                }

                // Find mesh
                nu_mesh_t mesh = NU_NULL;
                {
                    for (nu_size_t i = 0; i < loader->resources.size; ++i)
                    {
                        if (loader->resources.data[i].ptr == node->mesh)
                        {
                            mesh = loader->resources.data[i].handle;
                            break;
                        }
                    }
                    if (!mesh)
                    {
                        return NU_NULL;
                    }
                }

                // Set node
                nu_model_set(model, n, mesh, material, transform);
            }
        }

        break; // TODO: support multiple scene
    }

    cgltf_free(data);
    return model;
}

nu_model_t
nuext_model_load_file (nu_scope_t scope, nu_str_t filename)
{
#ifdef NU_BUILD_IMPORTER_CGLTF
    return nu__model_gltf_load(
        scope, &_ctx.importer.model_gltf_loader, filename);
#endif
    return NU_NULL;
}

#endif

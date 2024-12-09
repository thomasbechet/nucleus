#ifndef NU_IMPORTER_MODEL_H
#define NU_IMPORTER_MODEL_H

#include <nucleus/internal.h>

#ifdef NU_BUILD_IMPORTER_CGLTF
#define CGLTF_IMPLEMENTATION
#include <nucleus/external/cgltf/cgltf.h>
#endif

static void
nu__emplace_index (nu_mesh_t      mesh,
                   const nu_v3_t *positions,
                   const nu_v2_t *uvs,
                   nu_u16_t       index)
{
    const nu_v3_t *pos = positions + index;
    const nu_v2_t *uv  = uvs ? uvs + index : NU_NULL;
    nu_mesh_push(mesh, 1, pos, uv);
}

#define NU__EMPLACE_VERTEX(index_type)                            \
    index_type *indices                                           \
        = (index_type *)(data + view->offset + accessor->offset); \
    for (nu_size_t i = 0; i < indice_count; ++i)                  \
    {                                                             \
        nu__emplace_index(handle, positions, uvs, indices[i]);    \
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

            // Create mesh
            nu_mesh_attribute_t attributes = NU_MESH_POSITION;
            if (uvs)
            {
                attributes |= NU_MESH_UV;
            }
            nu_mesh_t handle
                = nu_mesh_new(NU_PRIMITIVE_TRIANGLES, indice_count, attributes);

            // Push vertices
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

            // Append asset
            nu__model_gltf_resource_t *cache = NU_VEC_PUSH(&loader->resources);
            NU_CHECK_PANIC(cache, "out of loader resource cache");
            cache->ptr    = mesh;
            cache->handle = handle;
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
    nu_texture_t handle = nuext_texture_load_memory(
        NU_TEXTURE_COLORMAP,
        (const nu_byte_t *)tview->buffer->data + tview->offset,
        tview->size);
    NU_ASSERT(handle);

    // Append asset
    nu__model_gltf_resource_t *cache = NU_VEC_PUSH(&loader->resources);
    NU_CHECK_PANIC(cache, "out of loader resource cache");
    cache->ptr    = texture;
    cache->handle = handle;

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
    nu_material_t handle = nu_material_new(NU_MATERIAL_SURFACE);
    nu_material_set_texture(handle, texture);

    // Append asset
    nu__model_gltf_resource_t *cache = NU_VEC_PUSH(&loader->resources);
    NU_CHECK_PANIC(cache, "out of loader resource cache");
    cache->ptr    = material;
    cache->handle = handle;

    return NU_ERROR_NONE;
}
static void
nu__load_material_default (nu__model_gltf_loader_t *loader)
{
    if (!loader->default_material)
    {
        NU_DEBUG("loading default material");

        nu_texture_t  texture  = nu_texture_new_from_color(NU_COLOR_RED);
        nu_material_t material = nu_material_new(NU_MATERIAL_SURFACE);
        nu_material_set_texture(material, texture);

        loader->default_material = material;
    }
}

static void
nu__model_gltf_loader_init (void)
{
    NU_VEC_ALLOC(&_ctx.importer.model_gltf_loader.resources, 512);
}
static void
nu__model_gltf_loader_free (void)
{
}
static nu_model_t
nu__model_gltf_load (nu__model_gltf_loader_t *loader, nu_str_t filename)
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
        NU_CHECK_ERROR(error, return NU_NULL);
    }
    for (nu_size_t i = 0; i < data->textures_count; ++i)
    {
        error = nu__load_texture(loader, data->textures + i);
        NU_CHECK_ERROR(error, return NU_NULL);
    }
    for (nu_size_t i = 0; i < data->materials_count; ++i)
    {
        const cgltf_material *mat = data->materials + i;
        if (mat->has_pbr_metallic_roughness
            && mat->pbr_metallic_roughness.base_color_texture.texture)
        {
            error = nu__load_material(loader, mat);
            NU_CHECK_ERROR(error, return NU_NULL);
        }
    }

    nu_model_t model = NU_NULL;

    // Load scenes and nodes
    for (nu_size_t s = 0; s < data->scenes_count; ++s)
    {
        cgltf_scene *scene = data->scenes + s;

        // Create model
        model = nu_model_new(scene->nodes_count);

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
nuext_model_load_file (nu_str_t filename)
{
#ifdef NU_BUILD_IMPORTER_CGLTF
    return nu__model_gltf_load(&_ctx.importer.model_gltf_loader, filename);
#endif
    return NU_NULL;
}

#endif

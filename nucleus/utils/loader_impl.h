#ifndef NU_LOADER_IMPL_H
#define NU_LOADER_IMPL_H

#include <nucleus/utils/loader.h>

#define CGLTF_IMPLEMENTATION
#include <nucleus/external/cgltf/cgltf.h>

static void
nuext__emplace_vertex (const nu_vec3_t *positions,
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
nu_error_t
nuext__load_mesh (cgltf_mesh                  *mesh,
                  nu_allocator_t              *allocator,
                  nu_logger_t                 *logger,
                  nuext_loader_mesh_callback_t mesh_callback,
                  void                        *userdata)
{
    NU_DEBUG(logger, "mesh name: %s", mesh->name);
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
            buf_positions
                = nu_alloc(allocator, sizeof(*buf_positions) * indice_count);
            buf_uvs = nu_alloc(allocator, sizeof(*buf_uvs) * indice_count);
            buf_normals
                = nu_alloc(allocator, sizeof(*buf_normals) * indice_count);

            switch (accessor->component_type)
            {
                case cgltf_component_type_r_8:
                case cgltf_component_type_r_8u: {
                    nu_u8_t *indices
                        = (nu_u8_t *)(data + view->offset + accessor->offset);
                    for (nu_u8_t i = 0; i < indice_count; ++i)
                    {
                        nuext__emplace_vertex(positions,
                                              uvs,
                                              normals,
                                              buf_positions,
                                              buf_uvs,
                                              buf_normals,
                                              i,
                                              indices[i]);
                    }
                }
                break;
                case cgltf_component_type_r_16:
                case cgltf_component_type_r_16u: {
                    nu_u16_t *indices
                        = (nu_u16_t *)(data + view->offset + accessor->offset);
                    for (nu_u16_t i = 0; i < indice_count; ++i)
                    {
                        nuext__emplace_vertex(positions,
                                              uvs,
                                              normals,
                                              buf_positions,
                                              buf_uvs,
                                              buf_normals,
                                              i,
                                              indices[i]);
                    }
                }
                break;
                case cgltf_component_type_r_32f:
                case cgltf_component_type_r_32u: {
                    nu_u32_t *indices
                        = (nu_u32_t *)(data + view->offset + accessor->offset);
                    for (nu_u32_t i = 0; i < indice_count; ++i)
                    {
                        nuext__emplace_vertex(positions,
                                              uvs,
                                              normals,
                                              buf_positions,
                                              buf_uvs,
                                              buf_normals,
                                              i,
                                              indices[i]);
                    }
                }
                break;
                default:
                    break;
            }

            nuext_loader_mesh_t info = { 0 };
            info.name                = mesh->name;
            info.positions           = buf_positions;
            info.uvs                 = buf_uvs;
            info.normals             = buf_normals;
            info.count               = indice_count;
            nu_error_t error         = mesh_callback(&info, userdata);
            nu_free(allocator,
                    buf_positions,
                    sizeof(*buf_positions) * indice_count);
            nu_free(allocator, buf_uvs, sizeof(*buf_uvs) * indice_count);
            nu_free(
                allocator, buf_normals, sizeof(*buf_normals) * indice_count);
            NU_ERROR_ASSERT(error);
        }
    }

    return NU_ERROR_NONE;
}
nu_error_t
nuext_load_gltf (const nu_char_t             *filename,
                 nu_logger_t                 *logger,
                 nu_allocator_t              *allocator,
                 nuext_loader_node_callback_t node_callback,
                 nuext_loader_mesh_callback_t mesh_callback,
                 void                        *userdata)
{
    cgltf_options options = { 0 };
    cgltf_data   *data    = NU_NULL;
    cgltf_result  result;
    nu_error_t    error;

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

    for (nu_size_t s = 0; s < data->scenes_count; ++s)
    {
        cgltf_scene *scene = data->scenes + s;

        for (nu_size_t n = 0; n < scene->nodes_count; ++n)
        {
            cgltf_node *node = scene->nodes[n];
            NU_DEBUG(logger, "node %d name: %s", n, node->name);
            NU_DEBUG(logger, "has_matrix %d", node->has_matrix);
            NU_DEBUG(logger, "has_translation %d", node->has_translation);
            NU_DEBUG(logger, "has_rotation %d", node->has_rotation);
            NU_DEBUG(logger, "has_scale %d", node->has_scale);

            if (node->mesh && mesh_callback)
            {
                error = nuext__load_mesh(
                    node->mesh, allocator, logger, mesh_callback, userdata);
                NU_ERROR_CHECK(error, return error);
            }

            nu_mat4_t transform = nu_mat4_identity();
            if (node->has_scale)
            {
                transform = nu_mat4_mul(nu_mat4_scale(node->scale[0],
                                                      node->scale[1],
                                                      node->scale[2]),
                                        transform);
            }
            if (node->has_translation)
            {
                transform = nu_mat4_mul(nu_mat4_translate(node->translation[0],
                                                          node->translation[1],
                                                          node->translation[2]),
                                        transform);
            }
            if (node->has_rotation)
            {
            }

            nuext_loader_node_t info;
            info.name      = node->name;
            info.transform = transform;
            info.mesh      = NU_NULL;
            info.material  = NU_NULL;
            error          = node_callback(&info, userdata);
            NU_ERROR_CHECK(error, return error);
        }
    }

    cgltf_free(data);
    return NU_ERROR_NONE;
}

#endif

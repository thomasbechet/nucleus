#ifndef NU_LOADER_IMPL_H
#define NU_LOADER_IMPL_H

#include <nucleus/utils/loader.h>

#define CGLTF_IMPLEMENTATION
#include <nucleus/external/cgltf/cgltf.h>
#define STB_IMAGE_IMPLEMENTATION
#include <nucleus/external/stb/stb_image.h>

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
#define NUEXT__EMPLACE_VERTEX(index_type)                         \
    index_type *indices                                           \
        = (index_type *)(data + view->offset + accessor->offset); \
    for (nu_size_t i = 0; i < indice_count; ++i)                  \
    {                                                             \
        nuext__emplace_vertex(positions,                          \
                              uvs,                                \
                              normals,                            \
                              buf_positions,                      \
                              buf_uvs,                            \
                              buf_normals,                        \
                              i,                                  \
                              indices[i]);                        \
    }

static nu_error_t
nuext__load_mesh (nuext_gltf_loader_t *loader, cgltf_mesh *mesh)
{
    NU_DEBUG(loader->_logger, "mesh name: %s", mesh->name);
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
                    NUEXT__EMPLACE_VERTEX(nu_u8_t)
                }
                break;
                case cgltf_component_type_r_16:
                case cgltf_component_type_r_16u: {
                    NUEXT__EMPLACE_VERTEX(nu_u16_t)
                }
                break;
                case cgltf_component_type_r_32f:
                case cgltf_component_type_r_32u: {
                    NUEXT__EMPLACE_VERTEX(nu_u32_t)
                }
                break;
                default:
                    break;
            }

            nuext_gltf_asset_t info = { 0 };
            info.type               = NUEXT_GLTF_MESH;
            info.id                 = (nu_size_t)mesh;
            info.mesh.name          = mesh->name;
            info.mesh.positions     = buf_positions;
            info.mesh.uvs           = buf_uvs;
            info.mesh.normals       = buf_normals;
            info.mesh.count         = indice_count;
            nu_error_t error = loader->_callback(&info, loader->_userdata);
            nu_free(loader->_allocator,
                    buf_positions,
                    sizeof(*buf_positions) * indice_count);
            nu_free(
                loader->_allocator, buf_uvs, sizeof(*buf_uvs) * indice_count);
            nu_free(loader->_allocator,
                    buf_normals,
                    sizeof(*buf_normals) * indice_count);
            NU_ERROR_ASSERT(error);
        }
    }

    return NU_ERROR_NONE;
}
static nu_error_t
nuext__load_texture (nuext_gltf_loader_t *loader, cgltf_texture *texture)
{
    cgltf_buffer_view *tview = texture->image->buffer_view;

    nu_image_t image;
    nu_error_t error = nuext_load_image_memory(
        (const nu_byte_t *)tview->buffer->data + tview->offset,
        tview->size,
        loader->_allocator,
        &image);
    NU_ERROR_CHECK(error, return error);

    nuext_gltf_asset_t info = { 0 };
    info.type               = NUEXT_GLTF_TEXTURE;
    info.id                 = (nu_size_t)texture;
    info.texture.name       = texture->name;
    info.texture.size       = image.size;
    info.texture.data       = image.data;
    error                   = loader->_callback(&info, loader->_userdata);
    nu_image_free(&image, loader->_allocator);
    return error;
}
static nu_error_t
nuext__load_material (nuext_gltf_loader_t  *loader,
                      const cgltf_material *material)
{
    nuext_gltf_asset_t info = { 0 };
    info.type               = NUEXT_GLTF_MATERIAL;
    info.id                 = (nu_size_t)material;
    info.material.name      = material->name;
    info.material.diffuse_id
        = (nu_size_t)
              material->pbr_metallic_roughness.base_color_texture.texture;
    return loader->_callback(&info, loader->_userdata);
}

nu_error_t
nuext_gltf_loader_init (nu_allocator_t       *alloc,
                        nu_logger_t          *logger,
                        nuext_gltf_callback_t callback,
                        void                 *userdata,
                        nuext_gltf_loader_t  *loader)
{
    loader->_allocator = alloc;
    loader->_logger    = logger;
    loader->_callback  = callback;
    loader->_userdata  = userdata;
    return NU_ERROR_NONE;
}
void
nuext_gltf_loader_free (nuext_gltf_loader_t *loader)
{
}
nu_error_t
nuext_gltf_load (nuext_gltf_loader_t *loader, const nu_char_t *filename)
{
    cgltf_options options = { 0 };
    // TODO: custom allocator
    cgltf_data  *data = NU_NULL;
    cgltf_result result;
    nu_error_t   error;

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

    // Load resources
    for (nu_size_t i = 0; i < data->meshes_count; ++i)
    {
        NU_DEBUG(loader->_logger, "loading mesh: %s", data->meshes[i].name);
        error = nuext__load_mesh(loader, data->meshes + i);
        NU_ERROR_CHECK(error, return error);
    }
    for (nu_size_t i = 0; i < data->textures_count; ++i)
    {
        NU_DEBUG(
            loader->_logger, "loading texture: %s", data->textures[i].name);
        error = nuext__load_texture(loader, data->textures + i);
        NU_ERROR_CHECK(error, return error);
    }
    for (nu_size_t i = 0; i < data->materials_count; ++i)
    {
        const cgltf_material *mat = data->materials + i;
        NU_DEBUG(loader->_logger, "loading material: %s", mat->name);
        if (mat->has_pbr_metallic_roughness
            && mat->pbr_metallic_roughness.base_color_texture.texture)
        {
            error = nuext__load_material(loader, mat);
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
                nuext_gltf_asset_t info;
                info.type           = NUEXT_GLTF_NODE;
                info.id             = (nu_size_t)node;
                info.node.name      = node->name;
                info.node.transform = transform;
                info.node.mesh_id   = (nu_size_t)node->mesh;
                NU_DEBUG(
                    loader->_logger, "requested mesh : %lu", info.node.mesh_id);
                info.node.material_id
                    = (nu_size_t)node->mesh->primitives->material;
                error = loader->_callback(&info, loader->_userdata);
                NU_ERROR_CHECK(error, return error);
            }
        }
    }

    cgltf_free(data);
    return NU_ERROR_NONE;
}

static nu_color_t *
nuext__parse_colors (const nu_byte_t *img,
                     nu_uvec2_t       size,
                     nu_size_t        comp,
                     nu_allocator_t  *allocator)
{
    nu_color_t *colors
        = nu_alloc(allocator, sizeof(nu_color_t) * size.x * size.y);
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
nuext_load_image (const nu_char_t *filename,
                  nu_allocator_t  *allocator,
                  nu_image_t      *image)
{
    int            w, h, n;
    unsigned char *img = stbi_load(filename, &w, &h, &n, STBI_default);
    NU_CHECK(img, return NU_ERROR_RESOURCE_LOADING);
    image->size = nu_uvec2(w, h);
    image->data = nuext__parse_colors(img, image->size, n, allocator);
    stbi_image_free(img);
    NU_CHECK(image->data, return NU_ERROR_ALLOCATION);
    return NU_ERROR_NONE;
}
nu_error_t
nuext_load_image_memory (const nu_byte_t *data,
                         nu_size_t        data_size,
                         nu_allocator_t  *allocator,
                         nu_image_t      *image)
{
    int            w, h, n;
    unsigned char *img
        = stbi_load_from_memory(data, data_size, &w, &h, &n, STBI_default);
    NU_CHECK(img, return NU_ERROR_RESOURCE_LOADING);
    image->size = nu_uvec2(w, h);
    image->data = nuext__parse_colors(img, image->size, n, allocator);
    stbi_image_free(img);
    NU_CHECK(image->data, return NU_ERROR_ALLOCATION);
    return NU_ERROR_NONE;
}

#endif

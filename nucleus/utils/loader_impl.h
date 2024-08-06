#ifndef NU_LOADER_IMPL_H
#define NU_LOADER_IMPL_H

#include <nucleus/utils/loader.h>

#define CGLTF_IMPLEMENTATION
#include <nucleus/external/cgltf/cgltf.h>

nu_error_t
nuext_load_gltf (const nu_char_t *filename, nu_logger_t *logger)
{
    cgltf_options options = { 0 };
    cgltf_data   *data    = NU_NULL;
    cgltf_result  result;

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

    NU_DEBUG(logger, "mesh count: %d", data->meshes_count);
    NU_DEBUG(logger, "scene count: %d", data->scenes_count);
    NU_DEBUG(logger, "texture count: %d", data->textures_count);

    for (nu_size_t s = 0; s < data->scenes_count; ++s)
    {
        cgltf_scene *scene = data->scenes + s;
        NU_DEBUG(logger, "scene name: %s", scene->name);
        NU_DEBUG(logger, "scene node count: %d", scene->nodes_count);

        for (nu_size_t n = 0; n < scene->nodes_count; ++n)
        {
            cgltf_node *node = scene->nodes[n];
            NU_DEBUG(logger, "node %d name: %s", n, node->name);

            if (node->mesh)
            {
                cgltf_mesh *mesh = node->mesh;
                NU_DEBUG(logger, "mesh name: %s", mesh->name);
                for (nu_size_t p = 0; p < mesh->primitives_count; ++p)
                {
                    cgltf_primitive *primitive = mesh->primitives + p;
                }
            }
        }
    }

    cgltf_free(data);
    return NU_ERROR_NONE;
}

#endif

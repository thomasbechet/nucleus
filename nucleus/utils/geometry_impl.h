#ifndef NU_GEOMETRY_IMPL_H
#define NU_GEOMETRY_IMPL_H

#include <nucleus/internal.h>

void
nu_generate_cube_mesh (float     side_length,
                       nu_vec3_t positions[NU_CUBE_MESH_VERTEX_COUNT],
                       nu_vec2_t uvs[NU_CUBE_MESH_VERTEX_COUNT],
                       nu_vec3_t normals[NU_CUBE_MESH_VERTEX_COUNT])
{
    float half_side = side_length / 2.0f;

    nu_vec3_t vertex_positions[8] = {
        nu_vec3(-half_side, -half_side, half_side),
        nu_vec3(half_side, -half_side, half_side),
        nu_vec3(half_side, -half_side, -half_side),
        nu_vec3(-half_side, -half_side, -half_side),
        nu_vec3(-half_side, half_side, half_side),
        nu_vec3(half_side, half_side, half_side),
        nu_vec3(half_side, half_side, -half_side),
        nu_vec3(-half_side, half_side, -half_side),
    };
    int faces_positions[12][3] = { // Front face
                                   { 2, 3, 7 },
                                   { 7, 6, 2 },
                                   // Back face
                                   { 0, 1, 5 },
                                   { 5, 4, 0 },
                                   // Left face
                                   { 3, 0, 4 },
                                   { 4, 7, 3 },
                                   // Right face
                                   { 1, 2, 6 },
                                   { 6, 5, 1 },
                                   // Top face
                                   { 4, 5, 6 },
                                   { 6, 7, 4 },
                                   // Bottom face
                                   { 3, 2, 1 },
                                   { 1, 0, 3 }
    };

    nu_vec2_t vertex_uvs[8]
        = { nu_vec2(0, 0), nu_vec2(1, 0), nu_vec2(1, 1), nu_vec2(0, 1) };
    int faces_uvs[12][3] = {
        // Front face
        { 0, 1, 2 },
        { 2, 3, 0 },
        // Back face
        { 0, 1, 2 },
        { 2, 3, 0 },
        // Left face
        { 0, 1, 2 },
        { 2, 3, 0 },
        // Right face
        { 0, 1, 2 },
        { 2, 3, 0 },
        // Top face
        { 0, 1, 2 },
        { 2, 3, 0 },
        // Bottom face
        { 0, 1, 2 },
        { 2, 3, 0 },
    };
    int faces_normals[12][3] = { // Front face
                                 { 0, 0, 0 },
                                 { 0, 0, 0 },
                                 // Back face
                                 { 1, 1, 1 },
                                 { 1, 1, 1 },
                                 // Left face
                                 { 2, 2, 2 },
                                 { 2, 2, 2 },
                                 // Right face
                                 { 3, 3, 3 },
                                 { 3, 3, 3 },
                                 // Top face
                                 { 4, 4, 4 },
                                 { 4, 4, 4 },
                                 // Bottom face
                                 { 5, 5, 5 },
                                 { 5, 5, 5 }
    };
    nu_vec3_t vertex_normals[6]
        = { NU_VEC3_FORWARD, NU_VEC3_BACKWARD, NU_VEC3_LEFT,
            NU_VEC3_RIGHT,   NU_VEC3_UP,       NU_VEC3_DOWN };

    // Flatten the triangles
    nu_size_t index = 0;
    for (nu_size_t i = 0; i < 12; ++i)
    {
        for (nu_size_t j = 0; j < 3; ++j)
        {
            int position_index = faces_positions[i][j];
            positions[index]   = vertex_positions[position_index];
            if (uvs)
            {
                int uv_index = faces_uvs[i][j];
                uvs[index]   = vertex_uvs[uv_index];
            }
            if (normals)
            {
                int normal_index = faces_normals[i][j];
                normals[index]   = vertex_normals[normal_index];
            }
            ++index;
        }
    }
}

#endif

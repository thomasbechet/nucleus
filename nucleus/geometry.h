#ifndef NU_GEOMETRY_H
#define NU_GEOMETRY_H

#include <nucleus/types.h>

#define NU_CUBE_MESH_VERTEX_SIZE   36
#define NU_CUBE_MESH_POSITION_SIZE 108
#define NU_CUBE_MESH_UV_SIZE       72

NU_API void nu_generate_cube_mesh(float side_length,
                                  float positions[NU_CUBE_MESH_POSITION_SIZE],
                                  float uvs[NU_CUBE_MESH_UV_SIZE]);

#ifdef NU_IMPLEMENTATION

void
nu_generate_cube_mesh (float side_length,
                       float positions[NU_CUBE_MESH_POSITION_SIZE],
                       float uvs[NU_CUBE_MESH_UV_SIZE])
{
    float half_side = side_length / 2.0f;

    int faces[12][3] = { // Front face
                         { 0, 1, 2 },
                         { 2, 3, 0 },
                         // Back face
                         { 4, 5, 6 },
                         { 6, 7, 4 },
                         // Left face
                         { 0, 3, 7 },
                         { 7, 4, 0 },
                         // Right face
                         { 1, 5, 6 },
                         { 6, 2, 1 },
                         // Top face
                         { 3, 2, 6 },
                         { 6, 7, 3 },
                         // Bottom face
                         { 0, 1, 5 },
                         { 5, 4, 0 }
    };

    float vertex_positions[8][3] = { { -half_side, -half_side, -half_side },
                                     { half_side, -half_side, -half_side },
                                     { half_side, half_side, -half_side },
                                     { -half_side, half_side, -half_side },
                                     { -half_side, -half_side, half_side },
                                     { half_side, -half_side, half_side },
                                     { half_side, half_side, half_side },
                                     { -half_side, half_side, half_side } };

    float vertex_uvs[8][2]
        = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f },
            { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };

    // Flatten the triangle_vertices array into the vertex_positions array
    nu_size_t position_index = 0;
    nu_size_t uv_index       = 0;
    for (nu_size_t i = 0; i < 12; ++i)
    {
        for (nu_size_t j = 0; j < 3; ++j)
        {
            int vertex_index            = faces[i][j];
            positions[position_index++] = vertex_positions[vertex_index][0];
            positions[position_index++] = vertex_positions[vertex_index][1];
            positions[position_index++] = vertex_positions[vertex_index][2];
            if (uvs)
            {
                uvs[uv_index++] = vertex_uvs[vertex_index][0];
                uvs[uv_index++] = vertex_uvs[vertex_index][1];
            }
        }
    }
}

#endif

#endif

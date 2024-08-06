#ifndef NU_GEOMETRY_H
#define NU_GEOMETRY_H

#include <nucleus/core/math.h>

#define NU_CUBE_MESH_VERTEX_COUNT 36

NU_API void nu_generate_cube_mesh(
    float     side_length,
    nu_vec3_t positions[NU_CUBE_MESH_VERTEX_COUNT],
    nu_vec2_t uvs[NU_CUBE_MESH_VERTEX_COUNT],
    nu_vec3_t normals[NU_CUBE_MESH_VERTEX_COUNT]);

#endif

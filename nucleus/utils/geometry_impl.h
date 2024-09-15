#ifndef NU_GEOMETRY_IMPL_H
#define NU_GEOMETRY_IMPL_H

#include <nucleus/internal.h>

static void
nu__generate_grid_mesh (nu_u32_t   width,
                        nu_u32_t   height,
                        float      unit,
                        float      uv_scale,
                        nu_vec3_t *positions,
                        nu_vec2_t *uvs,
                        nu_vec3_t *normals)
{
    for (nu_u32_t h = 0; h < height; ++h)
    {
        for (nu_u32_t w = 0; w < width; ++w)
        {
            nu_size_t index = (h * (width) + w) * 6;
            nu_vec3_t vertex_positions[4]
                = { nu_vec3(w * unit, 0, h * unit),
                    nu_vec3((w + 1) * unit, 0, h * unit),
                    nu_vec3((w + 1) * unit, 0, (h + 1) * unit),
                    nu_vec3(w * unit, 0, (h + 1) * unit) };
            nu_vec2_t vertex_uvs[4] = {
                nu_vec2((float)w / (float)width, (float)h / (float)height),
                nu_vec2((float)(w + 1) / (float)width,
                        (float)h / (float)height),
                nu_vec2((float)(w + 1) / (float)width,
                        (float)(h + 1) / (float)height),
                nu_vec2((float)w / (float)width,
                        (float)(h + 1) / (float)height),
            };
            nu_u32_t indices[6] = { 0, 3, 2, 2, 1, 0 };
            for (nu_size_t i = 0; i < 6; ++i)
            {
                positions[index + i] = vertex_positions[indices[i]];
                uvs[index + i] = nu_vec2_muls(vertex_uvs[indices[i]], uv_scale);
                normals[index + i] = NU_VEC3_UP;
            }
        }
    }
}
static void
nu__generate_plane_mesh (float     width,
                         float     height,
                         nu_vec3_t positions[NU_PLANE_VERTEX_COUNT],
                         nu_vec2_t uvs[NU_PLANE_VERTEX_COUNT],
                         nu_vec3_t normals[NU_PLANE_VERTEX_COUNT])
{
    nu_vec3_t vertex_positions[4] = { nu_vec3(0, 0, 0),
                                      nu_vec3(width, 0, 0),
                                      nu_vec3(width, 0, height),
                                      nu_vec3(0, 0, height) };
    nu_vec2_t vertex_uvs[4]
        = { nu_vec2(0, 0), nu_vec2(1, 0), nu_vec2(1, 1), nu_vec2(0, 1) };
    nu_u32_t indices[6] = { 0, 3, 2, 2, 1, 0 };
    for (nu_size_t i = 0; i < 6; ++i)
    {
        positions[i] = vertex_positions[indices[i]];
        uvs[i]       = vertex_uvs[indices[i]];
        normals[i]   = NU_VEC3_UP;
    }
}
static void
nu__generate_cube_mesh (float     unit,
                        nu_vec3_t positions[NU_CUBE_VERTEX_COUNT],
                        nu_vec2_t uvs[NU_CUBE_VERTEX_COUNT],
                        nu_vec3_t normals[NU_CUBE_VERTEX_COUNT])
{
    nu_vec3_t vertex_positions[8] = {
        nu_vec3(0, 0, unit),    nu_vec3(unit, 0, unit),
        nu_vec3(unit, 0, 0),    nu_vec3(0, 0, 0),
        nu_vec3(0, unit, unit), nu_vec3(unit, unit, unit),
        nu_vec3(unit, unit, 0), nu_vec3(0, unit, 0),
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

    nu_vec2_t vertex_uvs[4]
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

nu_geometry_t
nu_geometry_create (nu_size_t capacity)
{
    nu_size_t       index;
    nu__geometry_t *g = NU_POOL_ADD(&_ctx.utils.geometries, &index);
    NU_VEC_INIT(capacity, &g->positions);
    NU_VEC_INIT(capacity, &g->uvs);
    NU_VEC_INIT(capacity, &g->normals);
    return nu_handle_make(nu_geometry_t, index);
}
void
nu_geometry_delete (nu_geometry_t geometry)
{
    nu__geometry_t *g = &_ctx.utils.geometries.data[nu_handle_index(geometry)];
    NU_VEC_FREE(&g->positions);
    NU_VEC_FREE(&g->uvs);
    NU_VEC_FREE(&g->normals);
}

void
nu_geometry_cube (nu_geometry_t geometry, float side_length)
{
    nu__geometry_t *g = &_ctx.utils.geometries.data[nu_handle_index(geometry)];
    nu_vec_resize(&g->positions, NU_CUBE_VERTEX_COUNT);
    nu_vec_resize(&g->uvs, NU_CUBE_VERTEX_COUNT);
    nu_vec_resize(&g->normals, NU_CUBE_VERTEX_COUNT);
    nu__generate_cube_mesh(
        side_length, g->positions.data, g->uvs.data, g->normals.data);
}
void
nu_geometry_plane (nu_geometry_t geometry, float width, float height)
{
    nu__geometry_t *g = &_ctx.utils.geometries.data[nu_handle_index(geometry)];
    nu_vec_resize(&g->positions, NU_PLANE_VERTEX_COUNT);
    nu_vec_resize(&g->uvs, NU_PLANE_VERTEX_COUNT);
    nu_vec_resize(&g->normals, NU_PLANE_VERTEX_COUNT);
    nu__generate_plane_mesh(
        width, height, g->positions.data, g->uvs.data, g->normals.data);
}
void
nu_geometry_grid (nu_geometry_t geometry,
                  nu_u32_t      width,
                  nu_u32_t      height,
                  float         unit,
                  float         uv_scale)
{
    nu__geometry_t *g = &_ctx.utils.geometries.data[nu_handle_index(geometry)];
    nu_size_t       vertex_count = width * height * 2 * 3;
    nu_vec_resize(&g->positions, vertex_count);
    nu_vec_resize(&g->uvs, vertex_count);
    nu_vec_resize(&g->normals, vertex_count);
    nu__generate_grid_mesh(width,
                           height,
                           unit,
                           uv_scale,
                           g->positions.data,
                           g->uvs.data,
                           g->normals.data);
}
void
nu_geometry_transform (nu_geometry_t geometry, nu_mat4_t m)
{
    nu__geometry_t *g = &_ctx.utils.geometries.data[nu_handle_index(geometry)];
    for (nu_size_t i = 0; i < g->positions.size; ++i)
    {
        g->positions.data[i] = nu_mat4_mulv3(m, g->positions.data[i]);
    }
}
void
nu_geometry_append (nu_geometry_t dst, nu_geometry_t src)
{
    nu__geometry_t *g0 = &_ctx.utils.geometries.data[nu_handle_index(src)];
    nu__geometry_t *g1 = &_ctx.utils.geometries.data[nu_handle_index(dst)];
    nu_assert(g0 != g1);
    nu_size_t dst_size = g1->positions.size;
    nu_size_t src_size = g0->positions.size;
    nu_size_t new_size = src_size + dst_size;
    nu_vec_resize(&g1->positions, new_size);
    nu_vec_resize(&g1->uvs, new_size);
    nu_vec_resize(&g1->normals, new_size);
    nu_memcpy(g1->positions.data + dst_size,
              g0->positions.data,
              sizeof(*g1->positions.data) * src_size);
    nu_memcpy(g1->uvs.data + dst_size,
              g0->uvs.data,
              sizeof(*g1->uvs.data) * src_size);
    nu_memcpy(g1->normals.data + dst_size,
              g0->normals.data,
              sizeof(*g1->normals.data) * src_size);
}

nu_mesh_t
nu_mesh_create_geometry (nu_geometry_t geometry)
{
    nu__geometry_t *g = &_ctx.utils.geometries.data[nu_handle_index(geometry)];
    nu_mesh_t       handle = nu_mesh_create(g->positions.size);
    nu_check(handle, return handle);
    nu_mesh_update(handle, g->positions.data, g->uvs.data, g->normals.data);
    return handle;
}

#endif

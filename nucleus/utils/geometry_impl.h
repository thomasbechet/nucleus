#ifndef NU_GEOMETRY_IMPL_H
#define NU_GEOMETRY_IMPL_H

#include <nucleus/internal.h>

#define NU__CUBE_VERTEX    36
#define NU__CUBE_TRIANGLE  NU__CUBE_VERTEX / 3
#define NU__PLANE_VERTEX   6
#define NU__PLANE_TRIANGLE NU__PLANE_VERTEX / 3

static void
nu__generate_grid_mesh (nu_u32_t   width,
                        nu_u32_t   height,
                        float      unit,
                        float      uv_scale,
                        nu_vec3_t *positions,
                        nu_vec2_t *uvs)
{
    for (nu_u32_t h = 0; h < height; ++h)
    {
        for (nu_u32_t w = 0; w < width; ++w)
        {
            nu_size_t index = (h * (width) + w) * NU__PLANE_VERTEX;
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
            nu_u32_t indices[NU__PLANE_VERTEX] = { 0, 3, 2, 2, 1, 0 };
            for (nu_size_t i = 0; i < NU__PLANE_VERTEX; ++i)
            {
                positions[index + i] = vertex_positions[indices[i]];
                uvs[index + i] = nu_vec2_muls(vertex_uvs[indices[i]], uv_scale);
            }
        }
    }
}
static void
nu__generate_plane_mesh (float     width,
                         float     height,
                         nu_vec3_t positions[NU__PLANE_VERTEX],
                         nu_vec2_t uvs[NU__PLANE_VERTEX])

{
    nu_vec3_t vertex_positions[4] = { nu_vec3(0, 0, 0),
                                      nu_vec3(width, 0, 0),
                                      nu_vec3(width, 0, height),
                                      nu_vec3(0, 0, height) };
    nu_vec2_t vertex_uvs[4]
        = { nu_vec2(0, 0), nu_vec2(1, 0), nu_vec2(1, 1), nu_vec2(0, 1) };
    nu_u32_t indices[6] = { 0, 3, 2, 2, 1, 0 };
    for (nu_size_t i = 0; i < NU__PLANE_VERTEX; ++i)
    {
        positions[i] = vertex_positions[indices[i]];
        uvs[i]       = vertex_uvs[indices[i]];
    }
}
static void
nu__generate_cube_mesh (float     unit,
                        nu_vec3_t positions[NU__CUBE_VERTEX],
                        nu_vec2_t uvs[NU__CUBE_VERTEX])
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
            ++index;
        }
    }
}

nu_geometry_t
nu_geometry_create (nu_primitive_t primitive, nu_size_t capacity)
{
    NU_ASSERT(primitive == NU_PRIMITIVE_TRIANGLES);
    nu_size_t       index;
    nu__geometry_t *g  = NU_POOL_ADD(&_ctx.utils.geometries, &index);
    g->primitive       = primitive;
    g->primitive_count = 0;
    NU_VEC_INIT(nu__primitive_vertex_count(primitive, capacity), &g->positions);
    if (primitive == NU_PRIMITIVE_TRIANGLES)
    {
        NU_VEC_INIT(nu__primitive_vertex_count(primitive, capacity), &g->uvs);
    }
    return NU_HANDLE_MAKE(nu_geometry_t, index);
}
void
nu_geometry_delete (nu_geometry_t geometry)
{
    nu__geometry_t *g = &_ctx.utils.geometries.data[NU_HANDLE_INDEX(geometry)];
    if (g->primitive == NU_PRIMITIVE_TRIANGLES)
    {
        NU_VEC_FREE(&g->uvs);
    }
    NU_VEC_FREE(&g->positions);
}
nu_size_t
nu_geometry_count (nu_geometry_t geometry)
{
    nu__geometry_t *g = &_ctx.utils.geometries.data[NU_HANDLE_INDEX(geometry)];
    return g->primitive_count;
}

void
nu_geometry_cube (nu_geometry_t geometry, float side_length)
{
    nu__geometry_t *g  = &_ctx.utils.geometries.data[NU_HANDLE_INDEX(geometry)];
    g->primitive_count = NU__CUBE_TRIANGLE;
    NU_VEC_RESIZE(&g->positions, NU__CUBE_VERTEX);
    NU_VEC_RESIZE(&g->uvs, NU__CUBE_VERTEX);
    nu__generate_cube_mesh(side_length, g->positions.data, g->uvs.data);
}
void
nu_geometry_plane (nu_geometry_t geometry, float width, float height)
{
    nu__geometry_t *g  = &_ctx.utils.geometries.data[NU_HANDLE_INDEX(geometry)];
    g->primitive_count = NU__PLANE_TRIANGLE;
    NU_VEC_RESIZE(&g->positions, NU__PLANE_VERTEX);
    NU_VEC_RESIZE(&g->uvs, NU__PLANE_VERTEX);
    nu__generate_plane_mesh(width, height, g->positions.data, g->uvs.data);
}
void
nu_geometry_grid (nu_geometry_t geometry,
                  nu_u32_t      width,
                  nu_u32_t      height,
                  float         unit,
                  float         uv_scale)
{
    nu__geometry_t *g = &_ctx.utils.geometries.data[NU_HANDLE_INDEX(geometry)];
    nu_size_t       triangle_count = width * height * 2;
    nu_size_t       vertex_count   = triangle_count * 3;
    g->primitive_count             = triangle_count;
    NU_VEC_RESIZE(&g->positions, vertex_count);
    NU_VEC_RESIZE(&g->uvs, vertex_count);
    nu__generate_grid_mesh(
        width, height, unit, uv_scale, g->positions.data, g->uvs.data);
}
void
nu_geometry_transform (nu_geometry_t geometry, nu_mat4_t m)
{
    nu__geometry_t *g = &_ctx.utils.geometries.data[NU_HANDLE_INDEX(geometry)];
    for (nu_size_t i = 0; i < g->positions.size; ++i)
    {
        g->positions.data[i] = nu_mat4_mulv3(m, g->positions.data[i]);
    }
}
void
nu_geometry_append (nu_geometry_t dst, nu_geometry_t src)
{
    nu__geometry_t *g0 = &_ctx.utils.geometries.data[NU_HANDLE_INDEX(src)];
    nu__geometry_t *g1 = &_ctx.utils.geometries.data[NU_HANDLE_INDEX(dst)];
    NU_ASSERT(g0 != g1);
    NU_ASSERT(g1->primitive == g0->primitive);
    g1->primitive_count += g0->primitive_count;
    NU_VEC_APPEND(&g1->positions, &g0->positions);
    NU_VEC_APPEND(&g1->uvs, &g0->uvs);
}

nu_mesh_t
nu_geometry_create_mesh (nu_geometry_t geometry)
{
    nu__geometry_t *g = &_ctx.utils.geometries.data[NU_HANDLE_INDEX(geometry)];

    nu_mesh_t mesh = nu_mesh_create(g->primitive, g->primitive_count);
    nu_mesh_vec3(
        mesh, NU_MESH_POSITIONS, 0, g->primitive_count, g->positions.data);
    nu_mesh_vec2(mesh, NU_MESH_UVS, 0, g->primitive_count, g->uvs.data);

    return mesh;
}

#endif

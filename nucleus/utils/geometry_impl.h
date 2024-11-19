#ifndef NU_GEOMETRY_IMPL_H
#define NU_GEOMETRY_IMPL_H

#include <nucleus/internal.h>

static void
nu__append_vertex (nu__geometry_t *g, nu_v3_t pos, nu_v2_t uv)
{
    NU_ASSERT(g->type == NU_GEOMETRY_MESH);
    nu_v3_t *pp = NU_FIXEDVEC_PUSH(&g->mesh.positions);
    nu_v2_t *pu = NU_FIXEDVEC_PUSH(&g->mesh.uvs);
    if (!pp)
    {
        NU_ERROR("out of geometry vertices");
        return;
    }
    *pp = pos;
    *pu = uv;
}
static void
nu__append_index (nu__geometry_t *g, nu_u32_t ip, nu_u32_t iu)
{
    NU_ASSERT(g->type == NU_GEOMETRY_MESH);
    nu_u32_t *pp = NU_FIXEDVEC_PUSH(&g->mesh.positions_indices);
    nu_u32_t *pu = NU_FIXEDVEC_PUSH(&g->mesh.uvs_indices);
    if (!pp)
    {
        NU_ERROR("out of geometry mesh indices");
        return;
    }
    *pp = ip;
    *pu = iu;
}
static void
nu__append_quad (nu__geometry_t *g,
                 nu_u32_t        ip0,
                 nu_u32_t        ip1,
                 nu_u32_t        ip2,
                 nu_u32_t        ip3,
                 nu_u32_t        iu0,
                 nu_u32_t        iu1,
                 nu_u32_t        iu2,
                 nu_u32_t        iu3)
{
    NU_ASSERT(g->type == NU_GEOMETRY_MESH);
    switch (g->mesh.primitive)
    {
        case NU_PRIMITIVE_POINTS: {
            nu__append_index(g, ip0, iu0);
            nu__append_index(g, ip1, iu1);
            nu__append_index(g, ip2, iu2);
            nu__append_index(g, ip3, iu3);
        }
        break;
        case NU_PRIMITIVE_LINES: {
            nu__append_index(g, ip0, iu0);
            nu__append_index(g, ip1, iu1);
            nu__append_index(g, ip1, iu1);
            nu__append_index(g, ip2, iu2);
            nu__append_index(g, ip2, iu2);
            nu__append_index(g, ip3, iu3);
            nu__append_index(g, ip0, iu0);
        }
        break;
        case NU_PRIMITIVE_LINES_STRIP: {
            nu__append_index(g, ip0, iu0);
            nu__append_index(g, ip1, iu1);
            nu__append_index(g, ip2, iu2);
            nu__append_index(g, ip3, iu3);
            nu__append_index(g, ip0, iu0);
        }
        break;
        case NU_PRIMITIVE_TRIANGLES: {
            nu__append_index(g, ip0, iu0);
            nu__append_index(g, ip1, iu1);
            nu__append_index(g, ip2, iu2);
            nu__append_index(g, ip2, iu2);
            nu__append_index(g, ip3, iu3);
            nu__append_index(g, ip0, iu0);
        }
        break;
    }
}
static void
nu__append_triangle (nu__geometry_t *g,
                     nu_u32_t        ip0,
                     nu_u32_t        ip1,
                     nu_u32_t        ip2,
                     nu_u32_t        iu0,
                     nu_u32_t        iu1,
                     nu_u32_t        iu2)
{
    NU_ASSERT(g->type == NU_GEOMETRY_MESH);
    switch (g->mesh.primitive)
    {
        case NU_PRIMITIVE_POINTS: {
            nu__append_index(g, ip0, iu0);
            nu__append_index(g, ip1, iu1);
            nu__append_index(g, ip2, iu2);
        }
        break;
        case NU_PRIMITIVE_LINES: {
            nu__append_index(g, ip0, iu0);
            nu__append_index(g, ip1, iu1);
            nu__append_index(g, ip1, iu1);
            nu__append_index(g, ip2, iu2);
            nu__append_index(g, ip2, iu2);
            nu__append_index(g, ip0, iu0);
        }
        break;
        case NU_PRIMITIVE_LINES_STRIP: {
            nu__append_index(g, ip0, iu0);
            nu__append_index(g, ip1, iu1);
            nu__append_index(g, ip2, iu2);
        }
        break;
        case NU_PRIMITIVE_TRIANGLES: {
            nu__append_index(g, ip0, iu0);
            nu__append_index(g, ip1, iu1);
            nu__append_index(g, ip2, iu2);
        }
        break;
    }
}

static void
nu__generate_grid (nu__geometry_t *g,
                   nu_u32_t        width,
                   nu_u32_t        height,
                   nu_f32_t        unit,
                   nu_f32_t        uv_scale)
{
    // Generate positions and uvs
    nu_size_t pos_offset = g->mesh.positions.size;
    nu_size_t uv_offset  = g->mesh.uvs.size;
    for (nu_u32_t h = 0; h <= height; ++h)
    {
        for (nu_u32_t w = 0; w <= width; ++w)
        {
            nu_v3_t pos = nu_v3(w * unit, 0, h * unit);
            nu_v2_t uv  = nu_v2_muls(nu_v2((nu_f32_t)w / (nu_f32_t)width,
                                          (nu_f32_t)h / (nu_f32_t)height),
                                    uv_scale);
            nu__append_vertex(g, pos, uv);
        }
    }

    // Generate quads
    for (nu_u32_t h = 0; h < height; ++h)
    {
        for (nu_u32_t w = 0; w < width; ++w)
        {
            nu_u32_t v0 = (h + 1) * (width + 1) + w;
            nu_u32_t v1 = (h + 1) * (width + 1) + (w + 1);
            nu_u32_t v2 = h * (width + 1) + (w + 1);
            nu_u32_t v3 = h * (width + 1) + w;
            nu__append_quad(g,
                            pos_offset + v0,
                            pos_offset + v1,
                            pos_offset + v2,
                            pos_offset + v3,
                            uv_offset + v0,
                            uv_offset + v1,
                            uv_offset + v2,
                            uv_offset + v3);
        }
    }
}
static void
nu__generate_plane (nu__geometry_t *g, nu_f32_t width, nu_f32_t height)

{
    switch (g->type)
    {
        case NU_GEOMETRY_MESH: {
            const nu_v3_t positions[4] = {
                nu_v3(0, 0, height),
                nu_v3(width, 0, height),
                nu_v3(width, 0, 0),
                nu_v3(0, 0, 0),
            };
            const nu_v2_t uvs[4]
                = { nu_v2(0, 1), nu_v2(1, 1), nu_v2(1, 0), nu_v2(0, 0) };
            nu_size_t pos_offset = g->mesh.positions.size;
            nu_size_t uv_offset  = g->mesh.uvs.size;
            for (nu_size_t i = 0; i < NU_ARRAY_SIZE(positions); ++i)
            {
                nu__append_vertex(g, positions[i], uvs[i]);
            }
            nu__append_quad(g,
                            pos_offset + 0,
                            pos_offset + 1,
                            pos_offset + 2,
                            pos_offset + 3,
                            uv_offset + 0,
                            uv_offset + 1,
                            uv_offset + 2,
                            uv_offset + 3);
        }
        break;
    }
}
static void
nu__generate_cube (nu__geometry_t *g, nu_f32_t unit)
{
    const nu_v3_t positions[8] = {
        nu_v3(0, 0, unit),    nu_v3(unit, 0, unit), nu_v3(unit, 0, 0),
        nu_v3(0, 0, 0),       nu_v3(0, unit, unit), nu_v3(unit, unit, unit),
        nu_v3(unit, unit, 0), nu_v3(0, unit, 0),
    };
    const nu_v2_t uvs[4]
        = { nu_v2(0, 0), nu_v2(1, 0), nu_v2(1, 1), nu_v2(0, 1) };
    const nu_u32_t position_indices[4 * 6]
        = { 0, 1, 5, 4, 1, 2, 6, 5, 2, 3, 7, 6,
            3, 0, 4, 7, 4, 5, 6, 7, 3, 2, 1, 0 };
    const nu_u32_t uv_indices[4] = { 0, 1, 2, 3 };

    nu_size_t pos_offset = g->mesh.positions.size;
    nu_size_t uv_offset  = g->mesh.uvs.size;
    for (nu_size_t i = 0; i < NU_ARRAY_SIZE(positions); ++i)
    {
        *NU_VEC_PUSH(&g->positions) = positions[i];
    }
    for (nu_size_t i = 0; i < NU_ARRAY_SIZE(uvs); ++i)
    {
        *NU_VEC_PUSH(&g->uvs) = uvs[i];
    }
    nu__primitive_type_t *quads = nu__geometry_get_primitive(g, 4);
    for (nu_size_t i = 0; i < 6; ++i)
    {
        for (nu_size_t j = 0; j < 4; ++j)
        {
            *NU_VEC_PUSH(&quads->positions)
                = pos_offset + position_indices[i * 4 + j];
            *NU_VEC_PUSH(&quads->uvs) = uv_offset + uv_indices[j];
        }
    }
}

static void
nu__geometry_handler (nu_object_hook_t hook, void *data)
{
}
nu_geometry_t
nu_geometry_new_mesh (nu_scope_t     scope,
                      nu_primitive_t primitive,
                      nu_size_t      vertex_capacity,
                      nu_size_t      index_capacity)
{
    nu__geometry_t *g = nu_object_new(scope, _ctx.utils.obj_geometry);

    g->type = NU_GEOMETRY_MESH;

    g->mesh.primitive = primitive;
    NU_FIXEDVEC_ALLOC(scope, &g->positions, vertex_capacity);
    NU_FIXEDVEC_ALLOC(scope, &g->uvs, vertex_capacity);
    NU_FIXEDVEC_ALLOC(scope, &g->positions_indices, index_capacity);
    NU_FIXEDVEC_ALLOC(scope, &g->uvs_indices, index_capacity);

    return (nu_geometry_t)g;
}
void
nu_geometry_reset (nu_geometry_t geometry)
{
    nu__geometry_t *g = (nu__geometry_t *)geometry;
    switch (g->type)
    {
        case NU_GEOMETRY_MESH: {
            NU_FIXEDVEC_CLEAR(&g->positions);
            NU_FIXEDVEC_CLEAR(&g->uvs);
            NU_FIXEDVEC_CLEAR(&g->positions_indices);
            NU_FIXEDVEC_CLEAR(&g->uvs_indices);
        }
        break;
    }
}

void
nu_geometry_cube (nu_geometry_t geometry, nu_f32_t side_length)
{
    nu__geometry_t *g = (nu__geometry_t *)geometry;
    nu_geometry_reset(geometry);
    nu__generate_cube(g, side_length);
}
void
nu_geometry_plane (nu_geometry_t geometry, nu_f32_t width, nu_f32_t height)
{
    nu__geometry_t *g = (nu__geometry_t *)geometry;
    nu_geometry_reset(geometry);
    nu__generate_plane(g, width, height);
}
void
nu_geometry_grid (nu_geometry_t geometry,
                  nu_u32_t      width,
                  nu_u32_t      height,
                  nu_f32_t      unit,
                  nu_f32_t      uv_scale)
{
    nu__geometry_t *g = &_ctx.utils.geometries.data[NU_HANDLE_INDEX(geometry)];
    nu_geometry_reset(geometry);
    nu__generate_grid(g, width, height, unit, uv_scale);
}
void
nu_geometry_transform (nu_geometry_t geometry, nu_m4_t m)
{
    nu__geometry_t *g = &_ctx.utils.geometries.data[NU_HANDLE_INDEX(geometry)];
    for (nu_size_t i = 0; i < g->positions.size; ++i)
    {
        g->positions.data[i] = nu_m4_mulv3(m, g->positions.data[i]);
    }
}
void
nu_geometry_merge (nu_geometry_t dst, nu_geometry_t src)
{
    nu__geometry_t *s = &_ctx.utils.geometries.data[NU_HANDLE_INDEX(src)];
    nu__geometry_t *d = &_ctx.utils.geometries.data[NU_HANDLE_INDEX(dst)];
    NU_ASSERT(s != d);
    nu_size_t pos_offset = d->positions.size;
    nu_size_t uv_offset  = d->uvs.size;
    for (nu_size_t i = 0; i < s->primitives.size; ++i)
    {
        nu__primitive_type_t *ps = s->primitives.data + i;
        nu__primitive_type_t *pd = nu__geometry_get_primitive(d, ps->count);
        // Append indices (destination indices must be updated)
        nu_size_t first_index = pd->positions.size;
        NU_VEC_APPEND(&pd->positions, &ps->positions);
        NU_VEC_APPEND(&pd->uvs, &ps->uvs);
        for (nu_size_t j = first_index; j < pd->positions.size; ++j)
        {
            pd->positions.data[j] += pos_offset;
            pd->uvs.data[j] += uv_offset;
        }
    }
    NU_VEC_APPEND(&d->positions, &s->positions);
    NU_VEC_APPEND(&d->uvs, &s->uvs);
}

static nu_mesh_t
nu__geometry_new_mesh_lines (nu_scope_t scope, nu__geometry_t *g)
{
    nu_size_t max_line_count = 0;
    for (nu_size_t i = 0; i < g->primitives.size; ++i)
    {
        nu__primitive_type_t *p = g->primitives.data + i;
        if (p->count == 2)
        {
            max_line_count += p->positions.size / 2;
        }
        else if (p->count >= 2)
        {
            max_line_count += p->positions.size;
        }
    }
    NU_ASSERT(max_line_count);

    nu_u32_vec_t edges;
    NU_VEC_INIT(max_line_count * 2, &edges);

    nu_size_t vindex = 0;
    for (nu_size_t i = 0; i < g->primitives.size; ++i)
    {
        nu__primitive_type_t *p = g->primitives.data + i;
        // Cannot generate triangles for points and lines
        if (p->count >= 2)
        {
            nu_size_t edge_count = p->count;
            if (p->count == 2)
            {
                edge_count /= 2;
            }
            // Iterate over faces
            const nu_size_t face_count = p->positions.size / p->count;
            for (nu_size_t f = 0; f < face_count; ++f)
            {
                // Iterate over face edges
                for (nu_size_t e = 0; e < edge_count; ++e)
                {
                    // Order edge indices
                    nu_u32_t index0 = p->positions.data[f * p->count + e + 0];
                    nu_u32_t index1
                        = p->positions
                              .data[f * p->count + (e + 1) % edge_count];
                    if (index0 > index1)
                    {
                        nu_u32_t temp = index1;
                        index1        = index0;
                        index0        = temp;
                    }
                    // Add edge to list if missing
                    nu_bool_t found = NU_FALSE;
                    for (nu_size_t j = 0; j < edges.size; j += 2)
                    {
                        if (edges.data[j] == index0
                            && edges.data[j + 1] == index1)
                        {
                            found = NU_TRUE;
                            break;
                        }
                    }
                    if (!found)
                    {
                        *NU_VEC_PUSH(&edges) = index0;
                        *NU_VEC_PUSH(&edges) = index1;
                    }
                }
            }
        }
    }

    // Generate mesh
    nu_v3_vec_t positions;
    NU_VEC_INIT(edges.size, &positions);
    NU_VEC_RESIZE(&positions, edges.size);

    for (nu_size_t e = 0; e < edges.size; e += 2)
    {
        positions.data[e + 0] = g->positions.data[edges.data[e + 0]];
        positions.data[e + 1] = g->positions.data[edges.data[e + 1]];
    }

    nu_mesh_t mesh = nu_mesh_new(scope, NU_PRIMITIVE_LINES, edges.size / 2);
    nu_mesh_set_positions(mesh, 0, edges.size / 2, positions.data);

    NU_VEC_FREE(&edges);
    NU_VEC_FREE(&positions);

    return mesh;
}
static nu_mesh_t
nu__geometry_new_mesh_triangles (nu_scope_t scope, nu__geometry_t *g)
{
    nu_size_t triangle_count = 0;
    for (nu_size_t i = 0; i < g->primitives.size; ++i)
    {
        nu__primitive_type_t *p = g->primitives.data + i;
        if (p->count >= 3)
        {
            triangle_count += p->positions.size / (p->count - 2);
        }
    }
    NU_ASSERT(triangle_count);

    nu_v3_vec_t positions;
    nu_v2_vec_t uvs;
    nu_size_t   vertex_count = triangle_count * 3;
    NU_VEC_INIT(vertex_count, &positions);
    NU_VEC_INIT(vertex_count, &uvs);
    NU_VEC_RESIZE(&positions, vertex_count);
    NU_VEC_RESIZE(&uvs, vertex_count);

    nu_size_t vindex = 0;
    for (nu_size_t i = 0; i < g->primitives.size; ++i)
    {
        nu__primitive_type_t *p = g->primitives.data + i;
        // Cannot generate triangles for points and lines
        if (p->count >= 3)
        {
            // Iterate over faces
            const nu_size_t face_count        = p->positions.size / p->count;
            const nu_size_t triangle_per_face = p->count - 2;
            for (nu_size_t f = 0; f < face_count; ++f)
            {
                // Generate triangles for a given face
                for (nu_size_t t = 0; t < triangle_per_face; ++t)
                {
                    const nu_size_t offset = f * p->count;
                    positions.data[vindex]
                        = g->positions.data[p->positions.data[offset + 0]];
                    uvs.data[vindex] = g->uvs.data[p->uvs.data[offset + 0]];
                    ++vindex;
                    positions.data[vindex]
                        = g->positions.data[p->positions.data[offset + 1 + t]];
                    uvs.data[vindex] = g->uvs.data[p->uvs.data[offset + 1 + t]];
                    ++vindex;
                    positions.data[vindex]
                        = g->positions.data[p->positions.data[offset + 2 + t]];
                    uvs.data[vindex] = g->uvs.data[p->uvs.data[offset + 2 + t]];
                    ++vindex;
                }
            }
        }
    }

    nu_mesh_t mesh = nu_mesh_new(scope, NU_PRIMITIVE_TRIANGLES, triangle_count);
    nu_mesh_set_positions(mesh, 0, triangle_count, positions.data);
    nu_mesh_set_uvs(mesh, 0, triangle_count, uvs.data);

    NU_VEC_FREE(&positions);
    NU_VEC_FREE(&uvs);

    return mesh;
}
nu_mesh_t
nu_mesh_new_geometry (nu_scope_t     scope,
                      nu_geometry_t  geometry,
                      nu_primitive_t primitive)
{
    nu__geometry_t *g = &_ctx.utils.geometries.data[NU_HANDLE_INDEX(geometry)];
    switch (primitive)
    {
        case NU_PRIMITIVE_LINES:
            return nu__geometry_new_mesh_lines(scope, g);
        case NU_PRIMITIVE_TRIANGLES:
            return nu__geometry_new_mesh_triangles(scope, g);
        default:
            break;
    }
    NU_ERROR("unsupported geometry primitive mesh generation");
    return NU_NULL;
}
nu_mesh_t
nu_mesh_new_geometry_normals (nu_scope_t scope, nu_geometry_t geometry)
{
    nu__geometry_t *g = &_ctx.utils.geometries.data[NU_HANDLE_INDEX(geometry)];
    nu_size_t       face_count = 0;
    for (nu_size_t i = 0; i < g->primitives.size; ++i)
    {
        nu__primitive_type_t *p = g->primitives.data + i;
        if (p->count >= 3)
        {
            face_count += p->positions.size / p->count;
        }
    }
    NU_ASSERT(face_count);

    nu_v3_vec_t positions;
    NU_VEC_INIT(face_count * 2, &positions);

    for (nu_size_t i = 0; i < g->primitives.size; ++i)
    {
        nu__primitive_type_t *p = g->primitives.data + i;
        if (p->count >= 3)
        {
            for (nu_size_t f = 0; f < p->positions.size; f += p->count)
            {
                // Computer the face center of mass
                nu_v3_t center = NU_V3_ZEROS;
                for (nu_size_t i = f; i < f + p->count; ++i)
                {
                    center = nu_v3_add(center,
                                       g->positions.data[p->positions.data[i]]);
                }
                center = nu_v3_divs(center, p->count);
                // Compute normal (expect all points to be coplanar)
                nu_v3_t p0 = g->positions.data[p->positions.data[f + 0]];
                nu_v3_t p1 = g->positions.data[p->positions.data[f + 1]];
                nu_v3_t p2 = g->positions.data[p->positions.data[f + 2]];
                nu_v3_t n  = nu_triangle_normal(p0, p1, p2);
                *NU_VEC_PUSH(&positions) = center;
                *NU_VEC_PUSH(&positions) = nu_v3_add(center, n);
            }
        }
    }

    nu_mesh_t mesh = nu_mesh_new(scope, NU_PRIMITIVE_LINES, face_count);
    nu_mesh_set_positions(mesh, 0, face_count, positions.data);

    NU_VEC_FREE(&positions);

    return mesh;
}
nu_b3_t
nu_geometry_bounds (nu_geometry_t geometry)
{
    nu__geometry_t *g = &_ctx.utils.geometries.data[NU_HANDLE_INDEX(geometry)];
    NU_ASSERT(g->positions.size);
    nu_v3_t min, max;
    min = max = g->positions.data[0];
    for (nu_size_t i = 1; i < g->positions.size; ++i)
    {
        min = nu_v3_min(min, g->positions.data[i]);
        max = nu_v3_max(max, g->positions.data[i]);
    }
    return nu_b3(min, max);
}
#ifdef NU_BUILD_UTILS_SERIA
void
nu_geometry_write (nu_geometry_t geometry, nu_seria_t seria)
{
    nu__geometry_t *g = &_ctx.utils.geometries.data[NU_HANDLE_INDEX(geometry)];
}
void
nu_geometry_read (nu_geometry_t geometry, nu_seria_t seria)
{
    nu__geometry_t *g = &_ctx.utils.geometries.data[NU_HANDLE_INDEX(geometry)];
}
#endif

#endif

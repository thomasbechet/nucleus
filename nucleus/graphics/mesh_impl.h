#ifndef NU_MESH_IMPL_H
#define NU_MESH_IMPL_H

#include <nucleus/internal.h>
#include <nucleus/graphics/backend_impl.h>

static void
nu__mesh_cleanup (void *data)
{
    nu__mesh_t *mesh = data;
#ifdef NU_BUILD_GRAPHICS_GL
    nugl__mesh_free(mesh);
#endif
}
nu_object_type_t
nu_mesh (void)
{
    return _ctx.graphics.obj_mesh;
}
nu_mesh_t
nu_mesh_new (nu_primitive_t primitive,
             nu_size_t      vertex_capacity,
             nu_size_t      position_capacity,
             nu_size_t      uv_capacity)
{
    nu__mesh_t *m = nu_object_new(_ctx.graphics.obj_mesh);

    m->primitive = primitive;
    NU_VEC_ALLOC(&m->positions, position_capacity);
    NU_VEC_ALLOC(&m->uvs, uv_capacity);
    NU_VEC_ALLOC(&m->indices.positions, vertex_capacity);
    NU_VEC_ALLOC(&m->indices.uvs, vertex_capacity);
#ifdef NU_BUILD_GRAPHICS_GL
    nugl__mesh_init(m);
#endif

    return (nu_mesh_t)m;
}
size_t
nu_mesh_capacity (nu_mesh_t mesh)
{
    NU_ASSERT(mesh);
    nu__mesh_t *m = (nu__mesh_t *)mesh;
    return m->indices.positions.capacity;
}
nu_size_t
nu_mesh_size (nu_mesh_t mesh)
{
    NU_ASSERT(mesh);
    nu__mesh_t *m = (nu__mesh_t *)mesh;
    return m->indices.positions.size;
}
void
nu_mesh_clear (nu_mesh_t mesh)
{
    nu__mesh_t *m = (nu__mesh_t *)mesh;
    NU_VEC_CLEAR(&m->positions);
    NU_VEC_CLEAR(&m->uvs);
    NU_VEC_CLEAR(&m->indices.positions);
    NU_VEC_CLEAR(&m->indices.uvs);
}
void
nu_mesh_update (nu_mesh_t mesh)
{
    nu__mesh_t *m = (nu__mesh_t *)mesh;
#ifdef NU_BUILD_GRAPHICS_GL
    nugl__mesh_write_positions(m, 0, m->indices.positions.size);
#endif
#ifdef NU_BUILD_GRAPHICS_GL
    nugl__mesh_write_uvs(m, 0, m->indices.uvs.size);
#endif
}
nu_v3_t *
nu_mesh_positions (nu_mesh_t mesh)
{
    return ((nu__mesh_t *)mesh)->positions.data;
}
nu_v2_t *
nu_mesh_uvs (nu_mesh_t mesh)
{
    return ((nu__mesh_t *)mesh)->uvs.data;
}
nu_u16_t *
nu_mesh_position_indices (nu_mesh_t mesh)
{
    return ((nu__mesh_t *)mesh)->indices.positions.data;
}
nu_u16_t *
nu_mesh_uv_indices (nu_mesh_t mesh)
{
    return ((nu__mesh_t *)mesh)->indices.uvs.data;
}

static void
nu__append_position (nu__mesh_t *m, nu_v3_t pos)
{
    nu_v3_t *p = NU_VEC_PUSH(&m->positions);
    NU_CHECK_PANIC(p, "out of mesh positions");
    *p = pos;
}
static void
nu__append_uv (nu__mesh_t *m, nu_v2_t uv)
{
    nu_v2_t *p = NU_VEC_PUSH(&m->uvs);
    NU_CHECK_PANIC(p, "out of mesh uvs");
    *p = uv;
}
static void
nu__append_vertex (nu__mesh_t *m, nu_u16_t ip, nu_u16_t iu)
{
    nu_u16_t *pp = NU_VEC_PUSH(&m->indices.positions);
    nu_u16_t *pu = NU_VEC_PUSH(&m->indices.uvs);
    NU_CHECK_PANIC(pp, "out of mesh indices");
    *pp = ip;
    *pu = iu;
}
static void
nu__append_quad (nu__mesh_t *m,
                 nu_u16_t    ip0,
                 nu_u16_t    ip1,
                 nu_u16_t    ip2,
                 nu_u16_t    ip3,
                 nu_u16_t    iu0,
                 nu_u16_t    iu1,
                 nu_u16_t    iu2,
                 nu_u16_t    iu3)
{
    switch (m->primitive)
    {
        case NU_PRIMITIVE_POINTS: {
            nu__append_vertex(m, ip0, iu0);
            nu__append_vertex(m, ip1, iu1);
            nu__append_vertex(m, ip2, iu2);
            nu__append_vertex(m, ip3, iu3);
        }
        break;
        case NU_PRIMITIVE_LINES: {
            nu__append_vertex(m, ip0, iu0);
            nu__append_vertex(m, ip1, iu1);
            nu__append_vertex(m, ip1, iu1);
            nu__append_vertex(m, ip2, iu2);
            nu__append_vertex(m, ip2, iu2);
            nu__append_vertex(m, ip3, iu3);
            nu__append_vertex(m, ip0, iu0);
        }
        break;
        case NU_PRIMITIVE_LINES_STRIP: {
            nu__append_vertex(m, ip0, iu0);
            nu__append_vertex(m, ip1, iu1);
            nu__append_vertex(m, ip2, iu2);
            nu__append_vertex(m, ip3, iu3);
            nu__append_vertex(m, ip0, iu0);
        }
        break;
        case NU_PRIMITIVE_TRIANGLES: {
            nu__append_vertex(m, ip0, iu0);
            nu__append_vertex(m, ip1, iu1);
            nu__append_vertex(m, ip2, iu2);
            nu__append_vertex(m, ip2, iu2);
            nu__append_vertex(m, ip3, iu3);
            nu__append_vertex(m, ip0, iu0);
        }
        break;
    }
}
static void
nu__append_triangle (nu__mesh_t *m,
                     nu_u16_t    ip0,
                     nu_u16_t    ip1,
                     nu_u16_t    ip2,
                     nu_u16_t    iu0,
                     nu_u16_t    iu1,
                     nu_u16_t    iu2)
{
    switch (m->primitive)
    {
        case NU_PRIMITIVE_POINTS: {
            nu__append_vertex(m, ip0, iu0);
            nu__append_vertex(m, ip1, iu1);
            nu__append_vertex(m, ip2, iu2);
        }
        break;
        case NU_PRIMITIVE_LINES: {
            nu__append_vertex(m, ip0, iu0);
            nu__append_vertex(m, ip1, iu1);
            nu__append_vertex(m, ip1, iu1);
            nu__append_vertex(m, ip2, iu2);
            nu__append_vertex(m, ip2, iu2);
            nu__append_vertex(m, ip0, iu0);
        }
        break;
        case NU_PRIMITIVE_LINES_STRIP: {
            nu__append_vertex(m, ip0, iu0);
            nu__append_vertex(m, ip1, iu1);
            nu__append_vertex(m, ip2, iu2);
        }
        break;
        case NU_PRIMITIVE_TRIANGLES: {
            nu__append_vertex(m, ip0, iu0);
            nu__append_vertex(m, ip1, iu1);
            nu__append_vertex(m, ip2, iu2);
        }
        break;
    }
}

void
nu_mesh_append_vertex (nu_mesh_t mesh, nu_v3_t position, nu_v2_t uv)
{
    nu__mesh_t *m = (nu__mesh_t *)mesh;
    nu__append_vertex(m, m->positions.size, m->uvs.size);
    nu__append_position(m, position);
    nu__append_uv(m, uv);
}
static void
nu__generate_mesh_grid (nu__mesh_t *m,
                        nu_u32_t    width,
                        nu_u32_t    height,
                        nu_f32_t    unit,
                        nu_f32_t    uv_scale)
{
    // Generate positions and uvs
    nu_size_t pos_offset = m->positions.size;
    nu_size_t uv_offset  = m->uvs.size;
    for (nu_u32_t h = 0; h <= height; ++h)
    {
        for (nu_u32_t w = 0; w <= width; ++w)
        {
            nu_v3_t pos = nu_v3(w * unit, 0, h * unit);
            nu_v2_t uv  = nu_v2_muls(nu_v2((nu_f32_t)w / (nu_f32_t)width,
                                          (nu_f32_t)h / (nu_f32_t)height),
                                    uv_scale);
            nu__append_position(m, pos);
            nu__append_uv(m, uv);
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
            nu__append_quad(m,
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
nu__generate_plane (nu__mesh_t *m, nu_f32_t width, nu_f32_t height)

{
    const nu_v3_t positions[4] = {
        nu_v3(0, 0, height),
        nu_v3(width, 0, height),
        nu_v3(width, 0, 0),
        nu_v3(0, 0, 0),
    };
    const nu_v2_t uvs[4]
        = { nu_v2(0, 1), nu_v2(1, 1), nu_v2(1, 0), nu_v2(0, 0) };
    nu_size_t pos_offset = m->positions.size;
    nu_size_t uv_offset  = m->uvs.size;
    for (nu_size_t i = 0; i < NU_ARRAY_SIZE(positions); ++i)
    {
        nu__append_position(m, positions[i]);
        nu__append_uv(m, uvs[i]);
    }
    nu__append_quad(m,
                    pos_offset + 0,
                    pos_offset + 1,
                    pos_offset + 2,
                    pos_offset + 3,
                    uv_offset + 0,
                    uv_offset + 1,
                    uv_offset + 2,
                    uv_offset + 3);
}
static void
nu__generate_cube (nu__mesh_t *m, nu_f32_t unit)
{
    const nu_v3_t positions[8] = {
        nu_v3(0, 0, unit),    nu_v3(unit, 0, unit), nu_v3(unit, 0, 0),
        nu_v3(0, 0, 0),       nu_v3(0, unit, unit), nu_v3(unit, unit, unit),
        nu_v3(unit, unit, 0), nu_v3(0, unit, 0),
    };
    const nu_v2_t uvs[4]
        = { nu_v2(0, 0), nu_v2(1, 0), nu_v2(1, 1), nu_v2(0, 1) };
    nu_size_t pos_offset = m->positions.size;
    nu_size_t uv_offset  = m->uvs.size;
    for (nu_size_t i = 0; i < NU_ARRAY_SIZE(positions); ++i)
    {
        nu__append_position(m, positions[i]);
    }
    for (nu_size_t i = 0; i < NU_ARRAY_SIZE(uvs); ++i)
    {
        nu__append_uv(m, uvs[i]);
    }
    switch (m->primitive)
    {
        case NU_PRIMITIVE_POINTS: {
            for (nu_size_t i = 0; i < NU_ARRAY_SIZE(positions); ++i)
            {
                nu__append_vertex(m, i, i);
            }
        }
        break;
        case NU_PRIMITIVE_LINES: {
            const nu_u16_t position_indices[]
                = { 0, 1, 1, 2, 2, 3, 3, 0, 4, 5, 5, 6,
                    6, 7, 7, 4, 0, 4, 1, 5, 2, 6, 3, 7 };
            for (nu_size_t i = 0; i < NU_ARRAY_SIZE(position_indices); ++i)
            {
                nu__append_vertex(
                    m, pos_offset + position_indices[i], uv_offset);
            }
        }
        break;
        case NU_PRIMITIVE_LINES_STRIP: {
        }
        break;
        case NU_PRIMITIVE_TRIANGLES: {
            const nu_u16_t position_indices[4 * 6]
                = { 0, 1, 5, 4, 1, 2, 6, 5, 2, 3, 7, 6,
                    3, 0, 4, 7, 4, 5, 6, 7, 3, 2, 1, 0 };
            const nu_u16_t uv_indices[4] = { 0, 1, 2, 3 };

            for (nu_size_t i = 0; i < 6; ++i)
            {
                nu_u16_t ip[4];
                nu_u16_t iu[4];
                for (nu_size_t j = 0; j < 4; ++j)
                {
                    ip[j] = pos_offset + position_indices[i * 4 + j];
                    iu[j] = uv_offset + uv_indices[j];
                }
                nu__append_quad(
                    m, ip[0], ip[1], ip[2], ip[3], iu[0], iu[1], iu[2], iu[3]);
            }
        }
        break;
    }
}

void
nu_mesh_cube (nu_mesh_t mesh, nu_f32_t side_length)
{
    nu__mesh_t *m = (nu__mesh_t *)mesh;
    nu_mesh_clear(mesh);
    nu__generate_cube(m, side_length);
}
void
nu_mesh_plane (nu_mesh_t mesh, nu_f32_t width, nu_f32_t height)
{
    nu__mesh_t *m = (nu__mesh_t *)mesh;
    nu_mesh_clear(mesh);
    nu__generate_plane(m, width, height);
}
void
nu_mesh_grid (nu_mesh_t mesh,
              nu_u32_t  width,
              nu_u32_t  height,
              nu_f32_t  unit,
              nu_f32_t  uv_scale)
{
    nu__mesh_t *m = (nu__mesh_t *)mesh;
    nu_mesh_clear(mesh);
    nu__generate_mesh_grid(m, width, height, unit, uv_scale);
}
void
nu_mesh_transform (nu_mesh_t mesh, nu_m4_t transform)
{
    nu__mesh_t *m = (nu__mesh_t *)mesh;
    for (nu_size_t i = 0; i < m->positions.size; ++i)
    {
        m->positions.data[i] = nu_m4_mulv3(transform, m->positions.data[i]);
    }
}
void
nu_mesh_merge (nu_mesh_t dst, nu_mesh_t src)
{
    nu__mesh_t *s = (nu__mesh_t *)src;
    nu__mesh_t *d = (nu__mesh_t *)dst;
    NU_ASSERT(s != d);
    NU_ASSERT(s->primitive == d->primitive);
    nu_size_t pos_offset = d->positions.size;
    nu_size_t uv_offset  = d->uvs.size;

    NU_ASSERT(d->positions.size + s->positions.size <= d->positions.capacity);
    NU_ASSERT(d->uvs.size + s->uvs.size <= d->uvs.capacity);
    NU_ASSERT(d->indices.positions.size + s->indices.positions.size
              <= d->indices.positions.capacity);

    // append indices (destination indices must be updated)
    nu_size_t first_pos_index = d->indices.positions.size;
    nu_size_t first_uv_index  = d->indices.uvs.size;
    NU_VEC_APPEND(&d->indices.positions, &s->indices.positions);
    NU_VEC_APPEND(&d->indices.uvs, &s->indices.uvs);
    for (nu_size_t j = first_pos_index; j < d->indices.positions.size; ++j)
    {
        d->indices.positions.data[j] += pos_offset;
        d->indices.uvs.data[j] += uv_offset;
    }

    // append data
    NU_VEC_APPEND(&d->positions, &s->positions);
    NU_VEC_APPEND(&d->uvs, &s->uvs);
}
// nu_mesh_t
// nu_mesh_new_mesh (nu_mesh_t mesh)
// {
//     nu__mesh_t *m    = (nu__mesh_t *)mesh;
//     nu_mesh_t   mesh = nu_mesh_new(g->primitive, g->indices.positions.size);
//     NU_ASSERT(mesh);
//     const nu_size_t bufsize = 256;
//     for (nu_size_t c = 0; c < m->mesh.indices.positions.size; c += bufsize)
//     {
//         nu_size_t size = NU_MIN(bufsize, m->mesh.indices.positions.size - c);
//         NU_ASSERT(size);
//         nu_v3_t positions[256];
//         nu_v2_t uvs[256];
//         for (nu_size_t v = 0; v < size; ++v)
//         {
//             positions[v]
//                 = m->mesh.positions.data[m->mesh.indices.positions.data[c +
//                 v]];
//             uvs[v] = m->mesh.uvs.data[m->mesh.indices.uvs.data[c + v]];
//         }
//         nu_mesh_set_positions(mesh, c, size, positions);
//         nu_mesh_set_uvs(mesh, c, size, uvs);
//     }
//
//     return mesh;
// }
nu_mesh_t
nu_mesh_new_normals (nu_mesh_t mesh)
{
    nu__mesh_t *m = (nu__mesh_t *)mesh;
    NU_ASSERT(m->primitive == NU_PRIMITIVE_TRIANGLES);

    nu_size_t tricount = m->indices.positions.size / 3;
    NU_ASSERT(tricount);
    nu_size_t vertex_count = tricount * 2;
    nu_mesh_t normals      = nu_mesh_new(
        NU_PRIMITIVE_LINES, vertex_count, vertex_count, vertex_count);
    NU_ASSERT(normals);
    for (nu_size_t i = 0; i < tricount; ++i)
    {
        nu_v3_t p0 = m->positions.data[m->indices.positions.data[i * 3 + 0]];
        nu_v3_t p1 = m->positions.data[m->indices.positions.data[i * 3 + 1]];
        nu_v3_t p2 = m->positions.data[m->indices.positions.data[i * 3 + 2]];
        nu_v3_t n  = nu_triangle_normal(p0, p1, p2);
        nu_v3_t line[2];
        line[0] = nu_v3_divs(nu_v3_add(nu_v3_add(p0, p1), p2), 3.0);
        line[1] = nu_v3_add(line[0], n);
        // nu_mesh_set_positions(mesh, i * 2, 2, line);
    }

    return normals;
}
nu_b3_t
nu_mesh_bounds (nu_mesh_t mesh)
{
    nu__mesh_t *m = (nu__mesh_t *)mesh;
    NU_ASSERT(m->positions.size);
    nu_v3_t min, max;
    min = max = m->positions.data[0];
    for (nu_size_t i = 1; i < m->positions.size; ++i)
    {
        min = nu_v3_min(min, m->positions.data[i]);
        max = nu_v3_max(max, m->positions.data[i]);
    }
    return nu_b3(min, max);
}

#endif

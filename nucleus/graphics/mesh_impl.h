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
static nu_object_t
nu__mesh_load (nu_seria_t seria)
{
    return nu_mesh_load(seria);
}
static void
nu__mesh_save (nu_seria_t seria, nu_object_t mesh)
{
    nu_mesh_save(mesh, seria);
}
nu_object_type_t
nu_mesh (void)
{
    return _ctx.graphics.obj_mesh;
}
nu_mesh_t
nu_mesh_new (nugfx_primitive_t      primitive,
             nu_size_t           capacity,
             nugfx_attribute_t attributes)
{
    nu__mesh_t *m = nu_object_new(_ctx.graphics.obj_mesh);

    m->primitive = primitive;
    m->size      = 0;
    m->capacity  = capacity;
    m->positions = NU_NULL;
    m->uvs       = NU_NULL;
    if (attributes & NU_MESH_POSITION)
    {
        m->positions = nu_malloc(sizeof(*m->positions) * capacity);
    }
    if (attributes & NUGFX_UV)
    {
        m->uvs = nu_malloc(sizeof(*m->uvs) * capacity);
    }
    m->dirty = NU_FALSE;
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
    return m->capacity;
}
void
nu_mesh_update (nu_mesh_t mesh)
{
    nu__mesh_t *m = (nu__mesh_t *)mesh;
    if (m->dirty)
    {
#ifdef NU_BUILD_GRAPHICS_GL
        nugl__mesh_write_positions(m, 0, m->size);
        if (m->uvs)
        {
            nugl__mesh_write_uvs(m, 0, m->size);
        }
#endif
        m->dirty = NU_FALSE;
    }
}
void
nu_mesh_invalidate (nu_mesh_t mesh)
{
    nu__mesh_t *m = (nu__mesh_t *)mesh;
    m->dirty      = NU_TRUE;
}

static void
nu__append_vertices (nu__mesh_t    *m,
                     nu_size_t      count,
                     const nu_v3_t *positions,
                     const nu_v2_t *uvs)
{
    if (m->size + count > m->capacity)
    {
        NU_PANIC("out of mesh vertices");
    }
    if (positions && m->positions)
    {
        nu_memcpy(
            m->positions + m->size, positions, sizeof(*positions) * count);
    }
    if (uvs && m->uvs)
    {
        nu_memcpy(m->uvs + m->size, uvs, sizeof(*uvs) * count);
    }
    m->size += count;
}
static void
nu__append_quad (nu__mesh_t *m, const nu_v3_t *positions, const nu_v2_t *uvs)
{
    switch (m->primitive)
    {
        case NUGFX_POINTS: {
            nu__append_vertices(m, 4, positions, uvs);
        }
        break;
        case NUGFX_LINES: {
            const nu_size_t indices[] = { 0, 1, 1, 2, 2, 3, 3, 0 };
            for (nu_size_t i = 0; i < NU_ARRAY_SIZE(indices); ++i)
            {
                nu__append_vertices(
                    m, 1, positions + indices[i], uvs + indices[i]);
            }
        }
        break;
        case NUGFX_LINES_STRIP: {
            nu__append_vertices(m, 4, positions, uvs);
            nu__append_vertices(m, 1, positions, uvs);
        }
        break;
        case NUGFX_TRIANGLES: {
            const nu_size_t indices[] = { 0, 1, 2, 2, 3, 0 };
            for (nu_size_t i = 0; i < NU_ARRAY_SIZE(indices); ++i)
            {
                nu__append_vertices(
                    m, 1, positions + indices[i], uvs + indices[i]);
            }
        }
        break;
    }
}
static void
nu__append_triangle (nu__mesh_t    *m,
                     const nu_v3_t *positions,
                     const nu_v2_t *uvs)
{
    switch (m->primitive)
    {
        case NUGFX_POINTS:
            nu__append_vertices(m, 3, positions, uvs);
            break;
        case NUGFX_LINES: {
            const nu_size_t indices[] = { 0, 1, 1, 2, 2, 0 };
            for (nu_size_t i = 0; i < NU_ARRAY_SIZE(indices); ++i)
            {
                nu__append_vertices(
                    m, 1, positions + indices[i], uvs + indices[i]);
            }
        }
        break;
        case NUGFX_LINES_STRIP:
            nu__append_vertices(m, 3, positions, uvs);
            break;
        case NUGFX_TRIANGLES:
            nu__append_vertices(m, 3, positions, uvs);
            break;
    }
}

void
nu_mesh_push (nu_mesh_t      mesh,
              nu_size_t      count,
              const nu_v3_t *positions,
              const nu_v2_t *uvs)
{
    nu__mesh_t *m = (nu__mesh_t *)mesh;
    nu__append_vertices(m, count, positions, uvs);
    nu_mesh_invalidate(mesh);
}
static void
nu__generate_mesh_grid (nu__mesh_t *m,
                        nu_u32_t    width,
                        nu_u32_t    height,
                        nu_f32_t    unit,
                        nu_f32_t    uv_scale)
{
    // Generate positions and uvs
    for (nu_u32_t h = 0; h <= height; ++h)
    {
        for (nu_u32_t w = 0; w <= width; ++w)
        {
            const nu_u32_t widths[]  = { w, w + 1, w + 1, w };
            const nu_u32_t heights[] = { h + 1, h + 1, h, h };

            nu_v3_t pos[4];
            nu_v2_t uvs[4];
            for (nu_size_t i = 0; i < 4; ++i)
            {
                pos[i] = nu_v3(widths[i] * unit, 0, heights[i] * unit);
                uvs[i]
                    = nu_v2_muls(nu_v2((nu_f32_t)widths[i] / (nu_f32_t)width,
                                       (nu_f32_t)heights[i] / (nu_f32_t)height),
                                 uv_scale);
            }
            nu__append_quad(m, pos, uvs);
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
    nu__append_quad(m, positions, uvs);
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
    switch (m->primitive)
    {
        case NUGFX_POINTS:
            nu__append_vertices(
                m, NU_ARRAY_SIZE(positions), positions, NU_NULL);
            break;
        case NUGFX_LINES: {
            const nu_u16_t position_indices[]
                = { 0, 1, 1, 2, 2, 3, 3, 0, 4, 5, 5, 6,
                    6, 7, 7, 4, 0, 4, 1, 5, 2, 6, 3, 7 };
            for (nu_size_t i = 0; i < NU_ARRAY_SIZE(position_indices); ++i)
            {
                nu__append_vertices(
                    m, 1, &positions[position_indices[i]], NU_NULL);
            }
        }
        break;
        case NUGFX_LINES_STRIP: {
        }
        break;
        case NUGFX_TRIANGLES: {
            const nu_u16_t position_indices[4 * 6]
                = { 0, 1, 5, 4, 1, 2, 6, 5, 2, 3, 7, 6,
                    3, 0, 4, 7, 4, 5, 6, 7, 3, 2, 1, 0 };
            const nu_u16_t uv_indices[4] = { 0, 1, 2, 3 };

            for (nu_size_t i = 0; i < 6; ++i)
            {
                nu_v3_t vpos[4];
                nu_v2_t vuvs[4];
                for (nu_size_t j = 0; j < 4; ++j)
                {
                    vpos[j] = positions[position_indices[i * 4 + j]];
                    vuvs[j] = uvs[uv_indices[j]];
                }
                nu__append_quad(m, vpos, vuvs);
            }
        }
        break;
    }
}

nu_v3_t *
nu_mesh_positions (nu_mesh_t mesh)
{
    return ((nu__mesh_t *)mesh)->positions;
}
nu_v2_t *
nu_mesh_uvs (nu_mesh_t mesh)
{
    return ((nu__mesh_t *)mesh)->uvs;
}
void
nu_mesh_cube (nu_mesh_t mesh, nu_f32_t side_length)
{
    nu__mesh_t *m = (nu__mesh_t *)mesh;
    nu_mesh_clear(mesh);
    nu__generate_cube(m, side_length);
    nu_mesh_invalidate(mesh);
}
void
nu_mesh_plane (nu_mesh_t mesh, nu_f32_t width, nu_f32_t height)
{
    nu__mesh_t *m = (nu__mesh_t *)mesh;
    nu_mesh_clear(mesh);
    nu__generate_plane(m, width, height);
    nu_mesh_invalidate(mesh);
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
    nu_mesh_invalidate(mesh);
}
void
nu_mesh_transform (nu_mesh_t mesh, nu_m4_t transform)
{
    nu__mesh_t *m = (nu__mesh_t *)mesh;
    for (nu_size_t i = 0; i < m->size; ++i)
    {
        m->positions[i] = nu_m4_mulv3(transform, m->positions[i]);
    }
    nu_mesh_invalidate(mesh);
}
void
nu_mesh_merge (nu_mesh_t dst, nu_mesh_t src)
{
    nu__mesh_t *s = (nu__mesh_t *)src;
    nu__mesh_t *d = (nu__mesh_t *)dst;
    NU_ASSERT(s != d);
    NU_ASSERT(s->primitive == d->primitive);
    NU_ASSERT(d->size + s->size <= d->capacity);

    // append data
    nu_memcpy(
        d->positions + d->size, s->positions, sizeof(*s->positions) * s->size);
    nu_memcpy(d->uvs + d->size, s->uvs, sizeof(*s->uvs) * s->size);
    d->size += s->size;

    nu_mesh_invalidate(dst);
}
nu_mesh_t
nu_mesh_new_normals (nu_mesh_t mesh)
{
    nu__mesh_t *m = (nu__mesh_t *)mesh;
    NU_ASSERT(m->primitive == NUGFX_TRIANGLES);

    nu_size_t tricount = m->size / 3;
    NU_ASSERT(tricount);
    nu_size_t vertex_count = tricount * 2;
    nu_mesh_t normals
        = nu_mesh_new(NU_PRIMITIVE_LINES, vertex_count, NU_MESH_POSITION);
    NU_ASSERT(normals);
    for (nu_size_t i = 0; i < tricount; ++i)
    {
        nu_v3_t p0 = m->positions[i * 3 + 0];
        nu_v3_t p1 = m->positions[i * 3 + 1];
        nu_v3_t p2 = m->positions[i * 3 + 2];
        nu_v3_t n  = nu_triangle_normal(p0, p1, p2);
        nu_v3_t v0 = nu_v3_divs(nu_v3_add(nu_v3_add(p0, p1), p2), 3.0);
        nu_v3_t v1 = nu_v3_add(v0, n);

        nu_mesh_push(normals, 1, &v0, NU_NULL);
        nu_mesh_push(normals, 1, &v1, NU_NULL);
    }

    return normals;
}
nu_b3_t
nu_mesh_bounds (nu_mesh_t mesh)
{
    nu__mesh_t *m = (nu__mesh_t *)mesh;
    NU_ASSERT(m->size);
    nu_v3_t min, max;
    min = max = m->positions[0];
    for (nu_size_t i = 1; i < m->size; ++i)
    {
        min = nu_v3_min(min, m->positions[i]);
        max = nu_v3_max(max, m->positions[i]);
    }
    return nu_b3(min, max);
}
#ifdef NU_BUILD_SERIA
static nugfx_primitive_t nu__primitive_seria[] = { NUGFX_POINTS,
                                                NU_PRIMITIVE_LINES,
                                                NU_PRIMITIVE_LINES_STRIP,
                                                NU_PRIMITIVE_TRIANGLES };
nu_mesh_t
nu_mesh_load (nu_seria_t seria)
{
    nugfx_primitive_t primitive;
    {
        nu_u32_t value = 0;
        nu_seria_read_u32(seria, 1, &primitive);
        NU_ASSERT(value < NU_ARRAY_SIZE(nu__primitive_seria));
        primitive = nu__primitive_seria[value];
    }
    nugfx_attribute_t attributes;
    {
        nu_u32_t value;
        nu_seria_read_u32(seria, 1, &value);
        attributes = value;
    }
    nu_u32_t size;
    nu_seria_read_u32(seria, 1, &size);
    nu_mesh_t mesh = nu_mesh_new(primitive, size, attributes);
    nu_mesh_resize(mesh, size);
    if (attributes & NU_MESH_POSITION)
    {
        nu_seria_read_v3(seria, size, nu_mesh_positions(mesh));
    }
    if (attributes & NUGFX_UV)
    {
        nu_seria_read_v2(seria, size, nu_mesh_uvs(mesh));
    }
    return mesh;
}
void
nu_mesh_save (nu_mesh_t mesh, nu_seria_t seria)
{
    const nu__mesh_t *m = (nu__mesh_t *)mesh;
    {
        nu_u32_t value;
        for (nu_size_t i = 0; i < NU_ARRAY_SIZE(nu__primitive_seria); ++i)
        {
            if (nu__primitive_seria[i] == m->primitive)
            {
                value = i;
                break;
            }
        }
        nu_seria_write_u32(seria, 1, &value);
    }
    {
        nu_u32_t value = 0;
        value |= nu_mesh_positions(mesh) ? NU_MESH_POSITION : 0;
        value |= nu_mesh_uvs(mesh) ? NUGFX_UV : 0;
        nu_seria_write_u32(seria, 1, &value);
    }
    {
        nu_u32_t value = nu_mesh_size(mesh);
        nu_seria_write_u32(seria, 1, &value);
    }
    {
        if (nu_mesh_positions(mesh))
        {
            nu_seria_write_v3(
                seria, nu_mesh_size(mesh), nu_mesh_positions(mesh));
        }
        if (nu_mesh_uvs(mesh))
        {
            nu_seria_write_v2(seria, nu_mesh_size(mesh), nu_mesh_uvs(mesh));
        }
    }
}
#endif

#endif

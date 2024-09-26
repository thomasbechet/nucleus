#ifndef NU_IMMEDIATE_IMPL_H
#define NU_IMMEDIATE_IMPL_H

#include <nucleus/internal.h>

static void
nu__graphics_immediate_init (void)
{
    nu__graphics_immediate_reset();

    _ctx.graphics.im.points = nu_mesh_create(NU_PRIMITIVE_POINTS, 1000);
    _ctx.graphics.im.lines  = nu_mesh_create(NU_PRIMITIVE_LINES, 1000);
    _ctx.graphics.im.lines_strip
        = nu_mesh_create(NU_PRIMITIVE_LINES_STRIP, 1000);
    _ctx.graphics.im.triangles = nu_mesh_create(NU_PRIMITIVE_TRIANGLES, 1000);
}
static void
nu__graphics_immediate_free (void)
{
    if (_ctx.graphics.im.points)
    {
        nu_mesh_delete(_ctx.graphics.im.points);
    }
    if (_ctx.graphics.im.lines)
    {
        nu_mesh_delete(_ctx.graphics.im.lines);
    }
    if (_ctx.graphics.im.lines_strip)
    {
        nu_mesh_delete(_ctx.graphics.im.lines_strip);
    }
    if (_ctx.graphics.im.triangles)
    {
        nu_mesh_delete(_ctx.graphics.im.triangles);
    }
}
static void
nu__graphics_immediate_reset (void)
{
    _ctx.graphics.im.points_count      = 0;
    _ctx.graphics.im.lines_count       = 0;
    _ctx.graphics.im.lines_strip_count = 0;
    _ctx.graphics.im.triangles_count   = 0;
}

void
nu_draw_points (nu_renderpass_t  pass,
                const nu_vec3_t *positions,
                nu_size_t        count,
                nu_mat4_t        transform)
{
    nu__graphics_immediate_t *im = &_ctx.graphics.im;

    nu_size_t first = im->points_count;
    im->points_count += count;
    nu_mesh_vec3(im->points, NU_MESH_POSITIONS, first, count, positions);
    nu_draw_submesh(pass, im->points, first, count, transform);
}
void
nu_draw_lines (nu_renderpass_t  pass,
               const nu_vec3_t *positions,
               nu_size_t        count,
               nu_mat4_t        transform)
{
    nu__graphics_immediate_t *im = &_ctx.graphics.im;

    nu_size_t first = im->lines_count;
    im->lines_count += count;
    nu_mesh_vec3(im->lines, NU_MESH_POSITIONS, first, count, positions);
    nu_draw_submesh(pass, im->lines, first, count, transform);
}
void
nu_draw_lines_strip (nu_renderpass_t  pass,
                     const nu_vec3_t *positions,
                     nu_size_t        count,
                     nu_mat4_t        transform)
{
    nu__graphics_immediate_t *im = &_ctx.graphics.im;

    nu_size_t first = im->lines_strip_count;
    im->lines_strip_count += count;
    nu_mesh_vec3(im->lines_strip, NU_MESH_POSITIONS, first, count, positions);
    nu_draw_submesh(pass, im->lines_strip, first, count, transform);
}
void
nu_draw_box (nu_renderpass_t pass, nu_box3_t box, nu_mat4_t transform)
{
    nu__graphics_immediate_t *im = &_ctx.graphics.im;

    const nu_vec3_t v0 = nu_vec3(box.p.x, box.p.y, box.p.z);
    const nu_vec3_t v1 = nu_vec3(box.p.x + box.s.x, box.p.y, box.p.z);
    const nu_vec3_t v2 = nu_vec3(box.p.x + box.s.x, box.p.y, box.p.z + box.s.z);
    const nu_vec3_t v3 = nu_vec3(box.p.x, box.p.y, box.p.z + box.s.z);

    const nu_vec3_t v4 = nu_vec3(box.p.x, box.p.y + box.s.y, box.p.z);
    const nu_vec3_t v5 = nu_vec3(box.p.x + box.s.x, box.p.y + box.s.y, box.p.z);
    const nu_vec3_t v6
        = nu_vec3(box.p.x + box.s.x, box.p.y + box.s.y, box.p.z + box.s.z);
    const nu_vec3_t v7 = nu_vec3(box.p.x, box.p.y + box.s.y, box.p.z + box.s.z);

    const nu_vec3_t positions[]
        = { v0, v1, v1, v2, v2, v3, v3, v0, v4, v5, v5, v6,
            v6, v7, v7, v4, v0, v4, v1, v5, v2, v6, v3, v7 };

    nu_draw_lines(pass, positions, 12, transform);
}

#endif

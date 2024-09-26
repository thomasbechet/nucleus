#ifndef NU_IMMEDIATE_IMPL_H
#define NU_IMMEDIATE_IMPL_H

#include <nucleus/internal.h>

static void
nu__graphics_immediate_init (void)
{
    nu__graphics_immediate_reset();

    _ctx.graphics.im.points    = nu_mesh_create(NU_PRIMITIVE_POINTS, 1000);
    _ctx.graphics.im.lines     = nu_mesh_create(NU_PRIMITIVE_LINES, 1000);
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
    if (_ctx.graphics.im.triangles)
    {
        nu_mesh_delete(_ctx.graphics.im.triangles);
    }
}
static void
nu__graphics_immediate_reset (void)
{
    _ctx.graphics.im.points_count    = 0;
    _ctx.graphics.im.lines_count     = 0;
    _ctx.graphics.im.triangles_count = 0;
}

void
nu_draw_lines (nu_renderpass_t  pass,
               const nu_vec3_t *points,
               nu_size_t        count,
               nu_mat4_t        transform,
               nu_color_t       color)
{
    nu__graphics_immediate_t *im = &_ctx.graphics.im;

    nu_size_t first = im->lines_count;
    im->lines_count += count;
    nu_mesh_vec3(im->lines, NU_MESH_POSITIONS, first, count, points);
    nu_draw_submesh(pass, im->lines, first, count, transform);
}

#endif

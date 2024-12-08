#ifndef NU_IMMEDIATE_IMPL_H
#define NU_IMMEDIATE_IMPL_H

#include <nucleus/internal.h>

static void
nu__graphics_immediate_init (void)
{
    nu__graphics_immediate_reset();

    _ctx.graphics.im.points
        = nu_mesh_new(NU_PRIMITIVE_POINTS, 1000, 1000, 1000);
    _ctx.graphics.im.lines = nu_mesh_new(NU_PRIMITIVE_LINES, 5000, 5000, 5000);
    _ctx.graphics.im.lines_strip
        = nu_mesh_new(NU_PRIMITIVE_LINES_STRIP, 1000, 1000, 1000);
    _ctx.graphics.im.triangles
        = nu_mesh_new(NU_PRIMITIVE_TRIANGLES, 1000, 1000, 1000);
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
nu_draw_points (nu_renderpass_t pass,
                const nu_v3_t  *positions,
                nu_size_t       count,
                nu_material_t   material,
                nu_m4_t         transform)
{
    nu__graphics_immediate_t *im    = &_ctx.graphics.im;
    nu_size_t                 first = im->points_count;
    im->points_count += count;
    nu_mesh_set_positions(im->points, first, count, positions);
    nu_draw_submesh(pass, im->points, first, count, material, transform);
}
void
nu_draw_lines (nu_renderpass_t pass,
               const nu_v3_t  *positions,
               nu_size_t       count,
               nu_material_t   material,
               nu_m4_t         transform)
{
    nu__graphics_immediate_t *im    = &_ctx.graphics.im;
    nu_size_t                 first = im->lines_count;
    im->lines_count += count;
    nu_mesh_set_positions(im->lines, first, count, positions);
    nu_draw_submesh(pass, im->lines, first, count, material, transform);
}
void
nu_draw_lines_strip (nu_renderpass_t pass,
                     const nu_v3_t  *positions,
                     nu_size_t       count,
                     nu_material_t   material,
                     nu_m4_t         transform)
{
    nu__graphics_immediate_t *im    = &_ctx.graphics.im;
    nu_size_t                 first = im->lines_strip_count;
    im->lines_strip_count += count;
    nu_mesh_set_positions(im->lines_strip, first, count, positions);
    nu_draw_submesh(pass, im->lines_strip, first, count, material, transform);
}
void
nu_draw_triangles (nu_renderpass_t pass,
                   const nu_v3_t  *positions,
                   const nu_v2_t  *uvs,
                   nu_size_t       count,
                   nu_material_t   material,
                   nu_m4_t         transform)
{
    nu__graphics_immediate_t *im    = &_ctx.graphics.im;
    nu_size_t                 first = im->triangles_count;
    im->triangles_count += count;
    nu_mesh_set_positions(im->triangles, first, count, positions);
    nu_mesh_set_uvs(im->triangles, first, count, uvs);
    nu_draw_submesh(pass, im->triangles, first, count, material, transform);
}
static void
nu__draw_box_lines (nu_renderpass_t pass,
                    nu_b3_t         box,
                    nu_material_t   material,
                    nu_m4_t         transform)
{
    const nu_v3_t v0 = nu_v3(box.min.x, box.min.y, box.min.z);
    const nu_v3_t v1 = nu_v3(box.max.x, box.min.y, box.min.z);
    const nu_v3_t v2 = nu_v3(box.max.x, box.min.y, box.max.z);
    const nu_v3_t v3 = nu_v3(box.min.x, box.min.y, box.max.z);

    const nu_v3_t v4 = nu_v3(box.min.x, box.max.y, box.min.z);
    const nu_v3_t v5 = nu_v3(box.max.x, box.max.y, box.min.z);
    const nu_v3_t v6 = nu_v3(box.max.x, box.max.y, box.max.z);
    const nu_v3_t v7 = nu_v3(box.min.x, box.max.y, box.max.z);

    const nu_v3_t positions[]
        = { v0, v1, v1, v2, v2, v3, v3, v0, v4, v5, v5, v6,
            v6, v7, v7, v4, v0, v4, v1, v5, v2, v6, v3, v7 };

    nu_draw_lines(pass, positions, 12 * 2, material, transform);
}
static void
nu__draw_box_triangles (nu_renderpass_t pass,
                        nu_b3_t         box,
                        nu_material_t   material,
                        nu_m4_t         transform)
{
    const nu_v3_t v0 = nu_v3(box.min.x, box.min.y, box.min.z);
    const nu_v3_t v1 = nu_v3(box.max.x, box.min.y, box.min.z);
    const nu_v3_t v2 = nu_v3(box.max.x, box.min.y, box.max.z);
    const nu_v3_t v3 = nu_v3(box.min.x, box.min.y, box.max.z);

    const nu_v3_t v4 = nu_v3(box.min.x, box.max.y, box.min.z);
    const nu_v3_t v5 = nu_v3(box.max.x, box.max.y, box.min.z);
    const nu_v3_t v6 = nu_v3(box.max.x, box.max.y, box.max.z);
    const nu_v3_t v7 = nu_v3(box.min.x, box.max.y, box.max.z);

    const nu_v2_t uv0 = nu_v2(0, 0);
    const nu_v2_t uv1 = nu_v2(1, 0);
    const nu_v2_t uv2 = nu_v2(1, 1);
    const nu_v2_t uv3 = nu_v2(0, 1);

    const nu_v3_t positions[]
        = { v0, v1, v2, v3, v0, v2, v5, v4, v6, v6, v4, v7,
            v0, v3, v7, v0, v7, v4, v1, v5, v6, v1, v6, v2,
            v0, v4, v5, v0, v5, v1, v3, v2, v6, v3, v6, v7 };
    const nu_v2_t uvs[]
        = { uv0, uv1, uv2, uv3, uv0, uv2, uv0, uv3, uv1, uv3, uv1, uv2,
            uv0, uv1, uv2, uv0, uv2, uv3, uv0, uv1, uv2, uv0, uv2, uv3,
            uv0, uv1, uv2, uv0, uv2, uv3, uv0, uv1, uv2, uv0, uv2, uv3 };

    nu_draw_triangles(pass, positions, uvs, 6 * 2 * 3, material, transform);
}
void
nu_draw_box (nu_renderpass_t pass,
             nu_b3_t         box,
             nu_material_t   material,
             nu_m4_t         transform)
{

    nu__renderpass_t *p = (nu__renderpass_t *)pass;
    switch (p->type)
    {
        case NU_RENDERPASS_FORWARD: {
            switch (p->forward.mode)
            {
                case NU_SHADE_UNLIT:
                case NU_SHADE_LIT:
                    nu__draw_box_triangles(pass, box, material, transform);
                    break;
                case NU_SHADE_WIREFRAME:
                    nu__draw_box_lines(pass, box, material, transform);
                    break;
            }
        }
        break;
        case NU_RENDERPASS_SHADOW:
            nu__draw_box_triangles(pass, box, material, transform);
            break;
        default:
            break;
    }
}

#endif

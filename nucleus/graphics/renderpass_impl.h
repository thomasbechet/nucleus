#ifndef NU_RENDERPASS_IMPL_H
#define NU_RENDERPASS_IMPL_H

#include <nucleus/internal.h>
nu_renderpass_t
nu_renderpass_create (nu_renderpass_type_t type)
{
#ifdef NU_BUILD_GL
    return nugl__renderpass_create(type);
#endif
}
void
nu_renderpass_delete (nu_renderpass_t pass)
{
#ifdef NU_BUILD_GL
    nugl__renderpass_delete(pass);
#endif
}
void
nu_renderpass_reset (nu_renderpass_t pass)
{
#ifdef NU_BUILD_GL
    nugl__renderpass_reset(pass);
#endif
}
void
nu_renderpass_submit (nu_renderpass_t pass)
{
#ifdef NU_BUILD_GL
    nugl__renderpass_submit(pass);
#endif
#ifdef NU_BUILD_UTILS
    _ctx.utils.stats.graphics_current.renderpass_count += 1;
#endif
}

void
nu_renderpass_set_reset_after_submit (nu_renderpass_t pass, nu_bool_t bool)
{
#ifdef NU_BUILD_GL
    nugl__renderpass_set_reset_after_submit(pass, bool);
#endif
}
void
nu_renderpass_set_clear_color (nu_renderpass_t pass, nu_color_t *color)
{
#ifdef NU_BUILD_GL
    nugl__renderpass_set_clear_color(pass, color);
#endif
}
void
nu_renderpass_set_camera (nu_renderpass_t pass, nu_camera_t camera)
{
#ifdef NU_BUILD_GL
    nugl__renderpass_set_camera(pass, camera);
#endif
}
void
nu_renderpass_set_color_target (nu_renderpass_t pass, nu_texture_t texture)
{
#ifdef NU_BUILD_GL
    nugl__renderpass_set_color_target(pass, texture);
#endif
}
void
nu_renderpass_set_depth_target (nu_renderpass_t pass, nu_texture_t texture)
{
#ifdef NU_BUILD_GL
    nugl__renderpass_set_depth_target(pass, texture);
#endif
}
void
nu_renderpass_set_shade (nu_renderpass_t pass, nu_shademode_t mode)
{
#ifdef NU_BUILD_GL
    nugl__renderpass_set_shade(pass, mode);
#endif
}
void
nu_renderpass_set_lightenv (nu_renderpass_t pass, nu_lightenv_t env)
{
#ifdef NU_BUILD_GL
    nugl__renderpass_set_lightenv(pass, env);
#endif
}

void
nu_draw_submesh_instanced (nu_renderpass_t pass,
                           nu_mesh_t       mesh,
                           nu_size_t       first,
                           nu_size_t       count,
                           nu_material_t   material,
                           const nu_m4_t  *transforms,
                           nu_size_t       instance_count)
{
#ifdef NU_BUILD_GL
    nugl__draw_submesh_instanced(
        pass, mesh, first, count, material, transforms, instance_count);
#endif
#ifdef NU_BUILD_UTILS
    nu_primitive_t primitive = nugl__mesh_primitive(mesh);
    switch (primitive)
    {
        case NU_PRIMITIVE_POINTS:
            _ctx.utils.stats.graphics_current.point_count += count;
            break;
        case NU_PRIMITIVE_LINES:
            _ctx.utils.stats.graphics_current.line_count += count;
            break;
        case NU_PRIMITIVE_LINES_STRIP:
            _ctx.utils.stats.graphics_current.line_count += count;
            break;
        case NU_PRIMITIVE_TRIANGLES:
            _ctx.utils.stats.graphics_current.triangle_count += count;
            break;
    }
#endif
}
void
nu_draw_blit (nu_renderpass_t pass,
              nu_b2i_t        extent,
              nu_b2i_t        tex_extent,
              nu_material_t   material)
{
#ifdef NU_BUILD_GL
    nugl__draw_blit(pass, extent, tex_extent, material);
#endif
}

#endif

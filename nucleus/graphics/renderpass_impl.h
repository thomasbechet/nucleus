#ifndef NU_RENDERPASS_IMPL_H
#define NU_RENDERPASS_IMPL_H

#include <nucleus/internal.h>
#include <nucleus/graphics/backend_impl.h>

static void
nu__renderpass_handler (nu_object_hook_t hook, void *data)
{
    switch (hook)
    {
        case NU_OBJECT_CLEANUP: {
            nu__renderpass_t *p = data;
#ifdef NU_BUILD_GRAPHICS_GL
            nugl__renderpass_free(p);
#endif
        }
        break;
        case NU_OBJECT_SAVE:
        case NU_OBJECT_LOAD:
            break;
    }
}
nu_renderpass_t
nu_renderpass_new (nu_scope_t scope, nu_renderpass_type_t type)
{
    nu__renderpass_t *pass = nu_object_new(scope, _ctx.graphics.obj_renderpass);
    pass->type             = type;
    pass->reset_after_submit = NU_TRUE;
    pass->has_clear_color    = NU_FALSE;
    pass->clear_color        = NU_COLOR_BLACK;
    pass->color_target       = NU_NULL;
    pass->depth_target       = NU_NULL;
    switch (pass->type)
    {
        case NU_RENDERPASS_FORWARD:
            pass->forward.camera   = NU_NULL;
            pass->forward.mode     = NU_SHADE_LIT;
            pass->forward.lightenv = NU_NULL;
            break;
        case NU_RENDERPASS_CANVAS:
            break;
        case NU_RENDERPASS_SHADOW:
            pass->shadow.camera = NU_NULL;
            break;
    }
#ifdef NU_BUILD_GRAPHICS_GL
    nugl__renderpass_init(pass);
#endif
    return (nu_renderpass_t)pass;
}
void
nu_renderpass_reset (nu_renderpass_t pass)
{
    nu__renderpass_t *p = (nu__renderpass_t *)pass;
#ifdef NU_BUILD_GRAPHICS_GL
    nugl__renderpass_reset(p);
#endif
}
void
nu_renderpass_submit (nu_renderpass_t pass)
{
#ifdef NU_BUILD_GRAPHICS_GL
    nugl__renderpass_submit(pass);
#endif
#ifdef NU_BUILD_UTILS
    _ctx.utils.stats.graphics_current.renderpass_count += 1;
#endif
}

void
nu_renderpass_set_reset_after_submit (nu_renderpass_t pass, nu_bool_t bool)
{
    nu__renderpass_t *p   = (nu__renderpass_t *)pass;
    p->reset_after_submit = bool;
}
void
nu_renderpass_set_clear_color (nu_renderpass_t pass, nu_color_t *color)
{
    nu__renderpass_t *p = (nu__renderpass_t *)pass;
    if (color)
    {
        p->clear_color     = *color;
        p->has_clear_color = NU_TRUE;
    }
    else
    {
        p->has_clear_color = NU_FALSE;
    }
}
void
nu_renderpass_set_camera (nu_renderpass_t pass, nu_camera_t camera)
{
    nu__renderpass_t *p = (nu__renderpass_t *)pass;
    NU_ASSERT(camera);
    switch (p->type)
    {
        case NU_RENDERPASS_FORWARD:
            p->forward.camera = camera;
            break;
        case NU_RENDERPASS_SHADOW:
            p->shadow.camera = camera;
            break;
        default:
            NU_ERROR("invalid");
            break;
    }
}
void
nu_renderpass_set_color_target (nu_renderpass_t pass, nu_texture_t texture)
{
    nu__renderpass_t *p = (nu__renderpass_t *)pass;
    p->color_target     = texture;
}
void
nu_renderpass_set_depth_target (nu_renderpass_t pass, nu_texture_t texture)
{
    nu__renderpass_t *p = (nu__renderpass_t *)pass;
    p->depth_target     = texture;
#ifdef NU_BUILD_GRAPHICS_GL
    if (p->type == NU_RENDERPASS_SHADOW)
    {
        nugl__shadow_set_depth_map(p, texture);
    }
#endif
}
void
nu_renderpass_set_shade (nu_renderpass_t pass, nu_shademode_t mode)
{
    nu__renderpass_t *p = (nu__renderpass_t *)pass;
    NU_ASSERT(p->type == NU_RENDERPASS_FORWARD);
    p->forward.mode = mode;
#ifdef NU_BUILD_GRAPHICS_GL
    nugl__renderpass_set_shade(p);
#endif
}
void
nu_renderpass_set_lightenv (nu_renderpass_t pass, nu_lightenv_t env)
{
    nu__renderpass_t *p = (nu__renderpass_t *)pass;
    NU_ASSERT(p->type == NU_RENDERPASS_FORWARD);
    p->forward.lightenv = env;
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
    nu__renderpass_t *p = (nu__renderpass_t *)pass;
#ifdef NU_BUILD_GRAPHICS_GL
    nugl__draw_submesh_instanced(
        p, mesh, first, count, material, transforms, instance_count);
#endif
#ifdef NU_BUILD_UTILS
    nu__mesh_t *pmesh = (nu__mesh_t *)mesh;
    switch (pmesh->primitive)
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
    nu__renderpass_t *p = (nu__renderpass_t *)pass;
#ifdef NU_BUILD_GRAPHICS_GL
    nugl__draw_blit(p, extent, tex_extent, material);
#endif
}

#endif

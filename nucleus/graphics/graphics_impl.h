#ifndef NU_GRAPHICS_IMPL_H
#define NU_GRAPHICS_IMPL_H

#include <nucleus/graphics/backend_impl.h>

#include <nucleus/graphics/camera_impl.h>
#include <nucleus/graphics/texture_impl.h>
#include <nucleus/graphics/material_impl.h>
#include <nucleus/graphics/mesh_impl.h>
#include <nucleus/graphics/light_impl.h>
#include <nucleus/graphics/renderpass_impl.h>
#include <nucleus/graphics/font_impl.h>
#include <nucleus/graphics/image_impl.h>
#include <nucleus/graphics/geometry_impl.h>
#include <nucleus/graphics/model_impl.h>
#include <nucleus/graphics/immediate_impl.h>

static nu_error_t
nu__graphics_init (void)
{
    nu__graphics_t *gfx = &_ctx.graphics;

    gfx->obj_font
        = nu_object_register(NU_STR(NU_FONT), sizeof(nu__font_t), NU_NULL);
    gfx->obj_image
        = nu_object_register(NU_STR(NU_IMAGE), sizeof(nu__image_t), NU_NULL);
    gfx->obj_geometry = nu_object_register(
        NU_STR(NU_GEOMETRY), sizeof(nu__geometry_t), NU_NULL);
    gfx->obj_model
        = nu_object_register(NU_STR(NU_MODEL), sizeof(nu__model_t), NU_NULL);
    gfx->obj_camera
        = nu_object_register(NU_STR(NU_CAMERA), sizeof(nu__camera_t), NU_NULL);
    gfx->obj_texture = nu_object_register(
        NU_STR(NU_TEXTURE), sizeof(nu__texture_t), nu__texture_cleanup);
    gfx->obj_material = nu_object_register(
        NU_STR(NU_MATERIAL), sizeof(nu__material_t), NU_NULL);
    gfx->obj_mesh = nu_object_register(
        NU_STR(NU_MESH), sizeof(nu__mesh_t), nu__mesh_cleanup);
    gfx->obj_light
        = nu_object_register(NU_STR(NU_LIGHT), sizeof(nu__light_t), NU_NULL);
    gfx->obj_lightenv = nu_object_register(
        NU_STR(NU_LIGHTENV), sizeof(nu__lightenv_t), NU_NULL);
    gfx->obj_renderpass = nu_object_register(NU_STR(NU_RENDERPASS),
                                             sizeof(nu__renderpass_t),
                                             nu__renderpass_cleanup);

    // Initialize backend
    nu__renderer_init();

    // Create surface texture
    {
        nu__texture_t *tex = nu_object_new(gfx->obj_texture);
        tex->type          = NU_TEXTURE_COLORMAP_TARGET;
        tex->size = nu_v3u(_ctx.platform.size.x, _ctx.platform.size.y, 0);
        nugl__init_surface_texture(tex);
        _ctx.graphics.surface_color = (nu_texture_t)tex;
    }

    // Initialize immediate context
    nu__graphics_immediate_init();

    return NU_ERROR_NONE;
}
static nu_error_t
nu__graphics_free (void)
{
    nu__renderer_free();

    return NU_ERROR_NONE;
}
static nu_error_t
nu__graphics_render (void)
{
    nu__renderer_render(_ctx.platform.viewport.extent,
                        _ctx.platform.viewport.viewport);
    nu__graphics_immediate_reset();
    return NU_ERROR_NONE;
}

void
nu_draw_blit_sliced (nu_renderpass_t pass,
                     nu_b2i_t        extent,
                     nu_b2i_t        tex_extent,
                     nu_b2i_t        inner,
                     nu_material_t   material)
{
    nu_u32_t margin_left   = inner.min.x - tex_extent.min.x;
    nu_u32_t margin_right  = tex_extent.max.x - inner.max.x;
    nu_u32_t margin_top    = inner.min.y - tex_extent.min.y;
    nu_u32_t margin_bottom = tex_extent.max.y - inner.max.y;

    nu_u32_t tex_mid_width
        = nu_b2i_size(tex_extent).x - margin_left - margin_right;
    nu_u32_t tex_mid_height
        = nu_b2i_size(tex_extent).y - margin_top - margin_bottom;
    nu_u32_t ext_mid_width = nu_b2i_size(extent).x - margin_left - margin_right;
    nu_u32_t ext_mid_height
        = nu_b2i_size(extent).y - margin_top - margin_bottom;

    // Top-Left
    if (margin_top && margin_left)
    {
        nu_draw_blit(
            pass,
            nu_b2i_xywh(extent.min.x, extent.min.y, margin_left, margin_top),
            nu_b2i_xywh(
                tex_extent.min.x, tex_extent.min.y, margin_left, margin_top),
            material);
    }

    // Top-Mid
    if (margin_top)
    {
        nu_draw_blit(pass,
                     nu_b2i_xywh(extent.min.x + margin_left,
                                 extent.min.y,
                                 ext_mid_width,
                                 margin_top),
                     nu_b2i_xywh(tex_extent.min.x + margin_left,
                                 tex_extent.min.y,
                                 tex_mid_width,
                                 margin_top),
                     material);
    }

    // Top-Right
    if (margin_top && margin_right)
    {
        nu_draw_blit(pass,
                     nu_b2i_xywh(extent.max.x - margin_right + 1,
                                 extent.min.y,
                                 margin_right,
                                 margin_top),
                     nu_b2i_xywh(tex_extent.max.x - margin_right + 1,
                                 tex_extent.min.y,
                                 margin_right,
                                 margin_top),
                     material);
    }

    // Mid-Left
    if (margin_left)
    {
        nu_draw_blit(pass,
                     nu_b2i_xywh(extent.min.x,
                                 extent.min.y + margin_top,
                                 margin_right,
                                 ext_mid_height),
                     nu_b2i_xywh(tex_extent.min.x,
                                 tex_extent.min.y + margin_top,
                                 margin_right,
                                 tex_mid_height),
                     material);
    }

    // Mid-Mid
    nu_draw_blit(pass,
                 nu_b2i_xywh(extent.min.x + margin_left,
                             extent.min.y + margin_top,
                             ext_mid_width,
                             ext_mid_height),
                 nu_b2i_xywh(tex_extent.min.x + margin_left,
                             tex_extent.min.y + margin_top,
                             tex_mid_width,
                             tex_mid_height),
                 material);

    // Mid-Right
    if (margin_right)
    {
        nu_draw_blit(pass,
                     nu_b2i_xywh(extent.max.x - margin_right + 1,
                                 extent.min.y + margin_top,
                                 margin_right,
                                 ext_mid_height),
                     nu_b2i_xywh(tex_extent.max.x - margin_right + 1,
                                 tex_extent.min.y + margin_top,
                                 margin_right,
                                 tex_mid_height),
                     material);
    }

    // Bottom-Left
    if (margin_bottom && margin_left)
    {
        nu_draw_blit(pass,
                     nu_b2i_xywh(extent.min.x,
                                 extent.max.y - margin_bottom + 1,
                                 margin_left,
                                 margin_bottom),
                     nu_b2i_xywh(tex_extent.min.x,
                                 tex_extent.max.y - margin_bottom + 1,
                                 margin_right,
                                 margin_bottom),
                     material);
    }

    // Bottom-Mid
    if (margin_bottom)
    {
        nu_draw_blit(pass,
                     nu_b2i_xywh(extent.min.x + margin_left,
                                 extent.max.y - margin_bottom + 1,
                                 ext_mid_width,
                                 margin_bottom),
                     nu_b2i_xywh(tex_extent.min.x + margin_left,
                                 tex_extent.max.y - margin_bottom + 1,
                                 tex_mid_width,
                                 margin_bottom),
                     material);
    }

    // Bottom-Right
    if (margin_bottom && margin_right)
    {
        nu_draw_blit(pass,
                     nu_b2i_xywh(extent.max.x - margin_right + 1,
                                 extent.max.y - margin_bottom + 1,
                                 margin_right,
                                 margin_bottom),
                     nu_b2i_xywh(tex_extent.max.x - margin_right + 1,
                                 tex_extent.max.y - margin_bottom + 1,
                                 margin_right,
                                 margin_bottom),
                     material);
    }
}
void
nu_draw_mesh (nu_renderpass_t pass,
              nu_mesh_t       mesh,
              nu_material_t   material,
              nu_m4_t         transform)
{
    nu_draw_mesh_instanced(pass, mesh, material, &transform, 1);
}
void
nu_draw_submesh (nu_renderpass_t pass,
                 nu_mesh_t       mesh,
                 nu_size_t       first,
                 nu_size_t       count,
                 nu_material_t   material,
                 nu_m4_t         transform)
{
    nu_draw_submesh_instanced(
        pass, mesh, first, count, material, &transform, 1);
}
void
nu_draw_mesh_instanced (nu_renderpass_t pass,
                        nu_mesh_t       mesh,
                        nu_material_t   material,
                        const nu_m4_t  *transforms,
                        nu_size_t       instance_count)
{
    nu_size_t capacity = nu_mesh_capacity(mesh);
    nu_draw_submesh_instanced(
        pass, mesh, 0, capacity, material, transforms, instance_count);
}

#endif

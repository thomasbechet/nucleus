#ifndef NU_GRAPHICS_IMPL_H
#define NU_GRAPHICS_IMPL_H

#include <nucleus/graphics/backend_impl.h>
#include <nucleus/graphics/font_impl.h>
#include <nucleus/graphics/image_impl.h>
#include <nucleus/graphics/model_impl.h>
#include <nucleus/graphics/immediate_impl.h>

static nu_error_t
nu__graphics_init (void)
{
    NU_POOL_INIT(10, &_ctx.graphics.fonts);
    NU_POOL_INIT(10, &_ctx.graphics.images);
    NU_POOL_INIT(10, &_ctx.graphics.models);

    // Initialize backend
    nu__renderer_init();

    // Create surface texture
    _ctx.graphics.surface_color
        = nugl__create_surface_color(_ctx.platform.size);

    // Initialize immediate context
    nu__graphics_immediate_init();

    return NU_ERROR_NONE;
}
static nu_error_t
nu__graphics_free (void)
{
    // Terminate immediate context
    nu__graphics_immediate_free();

    nu__renderer_free();

    // TODO: free fonts and models
    NU_POOL_FREE(&_ctx.graphics.models);
    NU_POOL_FREE(&_ctx.graphics.images);
    NU_POOL_FREE(&_ctx.graphics.fonts);
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

nu_texture_t
nu_texture_create_color (nu_color_t color)
{
    nu_texture_t tex = nu_texture_create(NU_TEXTURE_COLOR, NU_V3U_ONES);
    NU_CHECK(tex, return tex);
    nu_texture_write_colors(tex, &color);
    return tex;
}
nu_texture_t
nu_image_create_texture (nu_image_t image)
{
    nu__image_t *ima = &_ctx.graphics.images.data[NU_HANDLE_INDEX(image)];
    nu_texture_t tex
        = nu_texture_create(NU_TEXTURE_COLOR, nu_v3u_v2u(ima->size, 0));
    NU_CHECK(tex, return tex);
    nu_texture_write_colors(tex, ima->colors);
    return tex;
}

nu_material_t
nu_material_create_color (nu_material_type_t type, nu_color_t color)
{
    nu_material_t mat = nu_material_create(type);
    nu_material_set_color(mat, color);
    return mat;
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
#ifdef NU_BUILD_GL
    nu_size_t capacity = nugl__mesh_capacity(mesh);
    nu_draw_submesh_instanced(
        pass, mesh, 0, capacity, material, transforms, instance_count);
#endif
}

#endif

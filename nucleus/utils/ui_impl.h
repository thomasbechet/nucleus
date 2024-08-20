#ifndef NU_UI_IMPL_H
#define NU_UI_IMPL_H

#include <nucleus/utils/ui.h>

void
nu_blit_sliced (nu_renderer_t         *renderer,
                nu_renderpass_handle_t pass,
                nu_material_handle_t   material,
                nu_rect_t              extent,
                nu_rect_t              tex_extent,
                nu_margin_t            margin)
{
    nu_u32_t tex_mid_width  = tex_extent.s.x - margin.left - margin.right;
    nu_u32_t tex_mid_height = tex_extent.s.y - margin.top - margin.bottom;
    nu_u32_t ext_mid_width  = extent.s.x - margin.left - margin.right;
    nu_u32_t ext_mid_height = extent.s.y - margin.top - margin.bottom;

    // Top-Left
    if (margin.top && margin.left)
    {
        nu_blit(
            renderer,
            pass,
            material,
            nu_rect(extent.p.x, extent.p.y, margin.left, margin.top),
            nu_rect(tex_extent.p.x, tex_extent.p.y, margin.left, margin.top));
    }

    // Top-Mid
    if (margin.top)
    {
        nu_blit(renderer,
                pass,
                material,
                nu_rect(extent.p.x + margin.left,
                        extent.p.y,
                        ext_mid_width,
                        margin.top),
                nu_rect(tex_extent.p.x + margin.left,
                        tex_extent.p.y,
                        tex_mid_width,
                        margin.top));
    }

    // Top-Right
    if (margin.top && margin.right)
    {
        nu_blit(renderer,
                pass,
                material,
                nu_rect(extent.p.x + extent.s.x - margin.right,
                        extent.p.y,
                        margin.right,
                        margin.top),
                nu_rect(tex_extent.p.x + tex_extent.s.x - margin.right,
                        tex_extent.p.y,
                        margin.right,
                        margin.top));
    }

    // Mid-Left
    if (margin.left)
    {
        nu_blit(renderer,
                pass,
                material,
                nu_rect(extent.p.x,
                        extent.p.y + margin.top,
                        margin.right,
                        ext_mid_height),
                nu_rect(tex_extent.p.x,
                        tex_extent.p.y + margin.top,
                        margin.right,
                        tex_mid_height));
    }

    // Mid-Mid
    nu_blit(renderer,
            pass,
            material,
            nu_rect(extent.p.x + margin.left,
                    extent.p.y + margin.top,
                    ext_mid_width,
                    ext_mid_height),
            nu_rect(tex_extent.p.x + margin.left,
                    tex_extent.p.y + margin.top,
                    tex_mid_width,
                    tex_mid_height));

    // Mid-Right
    if (margin.right)
    {
        nu_blit(renderer,
                pass,
                material,
                nu_rect(extent.p.x + extent.s.x - margin.right,
                        extent.p.y + margin.top,
                        margin.right,
                        ext_mid_height),
                nu_rect(tex_extent.p.x + tex_extent.s.x - margin.right,
                        tex_extent.p.y + margin.top,
                        margin.right,
                        tex_mid_height));
    }

    // Bottom-Left
    if (margin.bottom && margin.left)
    {
        nu_blit(renderer,
                pass,
                material,
                nu_rect(extent.p.x,
                        extent.p.y + extent.s.y - margin.bottom,
                        margin.left,
                        margin.bottom),
                nu_rect(tex_extent.p.x,
                        tex_extent.p.y + tex_extent.s.y - margin.bottom,
                        margin.right,
                        margin.bottom));
    }

    // Bottom-Mid
    if (margin.bottom)
    {
        nu_blit(renderer,
                pass,
                material,
                nu_rect(extent.p.x + margin.left,
                        extent.p.y + extent.s.y - margin.bottom,
                        ext_mid_width,
                        margin.bottom),
                nu_rect(tex_extent.p.x + margin.left,
                        tex_extent.p.y + tex_extent.s.y - margin.bottom,
                        tex_mid_width,
                        margin.bottom));
    }

    // Bottom-Right
    if (margin.bottom && margin.right)
    {
        nu_blit(renderer,
                pass,
                material,
                nu_rect(extent.p.x + extent.s.x - margin.right,
                        extent.p.y + extent.s.y - margin.bottom,
                        margin.right,
                        margin.bottom),
                nu_rect(tex_extent.p.x + tex_extent.s.x - margin.right,
                        tex_extent.p.y + tex_extent.s.y - margin.bottom,
                        margin.right,
                        margin.bottom));
    }
}

nu_error_t
nu_ui_init (nu_allocator_t *alloc, nu_ui_t *ui)
{
    nu_slotmap_init(&ui->_objects, alloc, 32);
    ui->_first_controller  = NU_NULL;
    ui->_active_renderpass = NU_NULL;
    ui->_building          = NU_FALSE;
    return NU_ERROR_NONE;
}
void
nu_ui_free (nu_ui_t *ui, nu_allocator_t *alloc)
{
    nu_slotmap_free(&ui->_objects, alloc);
}

static nu_renderpass_handle_t
nu__ui_active_renderpass (nu_ui_t *ui, nu_slot_t slot)
{
    if (slot)
    {
        return nu_slotmap_get(&ui->_objects, slot)->renderpass;
    }
    nu_renderpass_info_t info;
    info.type               = NU_RENDERPASS_CANVAS;
    info.reset_after_submit = NU_FALSE;
    nu_renderpass_handle_t handle;
    nu_error_t error = nu_renderpass_create(ui->_renderer, &info, &handle);
    NU_ERROR_ASSERT(error);

    return handle;
}

void
nu_ui_begin (nu_ui_t *ui, nu_platform_t *platform, nu_renderer_t *renderer)
{
    ui->_building          = NU_TRUE;
    ui->_renderer          = renderer;
    ui->_active_renderpass = ui->_active_renderpass;

    // Reset passes
}
void
nu_ui_end (nu_ui_t *ui)
{
    NU_ASSERT(ui->_building);
}
void
nu_ui_submit_renderpasses (const nu_ui_t                *ui,
                           nu_renderer_t                *renderer,
                           const nu_renderpass_submit_t *info)
{
}

nu_renderpass_handle_t
nu_ui_active_renderpass (const nu_ui_t *ui)
{
    NU_ASSERT(ui->_building);
    return nu_slotmap_get(&ui->_objects, ui->_active_renderpass)->renderpass;
}

nu_bool_t
nu_ui_button (nu_ui_t *ui, nu_rect_t r)
{
    return NU_FALSE;
}

#endif

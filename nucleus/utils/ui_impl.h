#ifndef NU_UI_IMPL_H
#define NU_UI_IMPL_H

#include <nucleus/utils/ui.h>

void
nu_blit_sliced (nu_renderer_t         *renderer,
                nu_renderpass_handle_t pass,
                nu_material_handle_t   material,
                nu_rect_t              extent,
                nu_rect_t              tex_extent,
                nu_ui_margin_t         margin)
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
nu_ui_init (nu_renderer_t *renderer, nu_allocator_t *alloc, nu_ui_t *ui)
{
    ui->_allocator = alloc;

    nu_vec_init(&ui->_styles, alloc, 10);
    ui->_button_style   = NU_NULL;
    ui->_checkbox_style = NU_NULL;

    ui->_active_widget     = 0;
    ui->_hot_widget        = 0;
    ui->_active_controller = 0;
    ui->_hot_controller    = 0;
    ui->_renderer          = NU_NULL;

    nu_vec_init(&ui->_passes, alloc, 1);

    // Create main renderpass
    nu_renderpass_info_t info;
    info.type = NU_RENDERPASS_CANVAS;
    nu_error_t error
        = nu_renderpass_create(renderer, &info, &ui->active_renderpass);
    NU_ERROR_CHECK(error, return error);
    nu_vec_push(&ui->_passes, alloc);
    nu__ui_pass_t *pass = nu_vec_last(&ui->_passes);
    pass->renderpass    = ui->active_renderpass;

    // Initialize controllers
    for (nu_size_t i = 0; i < NU_UI_MAX_CONTROLLER; ++i)
    {
        ui->controllers[i].active = NU_FALSE;
    }

    return NU_ERROR_NONE;
}
void
nu_ui_free (nu_ui_t *ui, nu_renderer_t *renderer)
{
    for (nu_size_t i = 0; i < ui->_passes.size; ++i)
    {
        nu_renderpass_reset(renderer, ui->_passes.data[i].renderpass);
    }
    nu_vec_free(&ui->_passes, ui->_allocator);
    nu_vec_free(&ui->_styles, ui->_allocator);
}

static nu_renderpass_handle_t
nu__ui_active_renderpass (nu_ui_t *ui, nu_slot_t slot)
{
    // if (slot)
    // {
    //     return nu_slotmap_get(&ui->_objects, slot)->renderpass;
    // }
    nu_renderpass_info_t info;
    info.type               = NU_RENDERPASS_CANVAS;
    info.reset_after_submit = NU_FALSE;
    nu_renderpass_handle_t handle;
    nu_error_t error = nu_renderpass_create(ui->_renderer, &info, &handle);
    NU_ERROR_ASSERT(error);

    return handle;
}

void
nu_ui_begin (nu_ui_t             *ui,
             const nu_platform_t *platform,
             nu_renderer_t       *renderer)
{
    // Reset renderpass
    ui->_renderer = renderer;
    for (nu_size_t i = 0; i < ui->_passes.size; ++i)
    {
        nu_renderpass_reset(renderer, ui->_passes.data[i].renderpass);
    }

    // Update controllers
    for (nu_size_t i = 0; i < NU_UI_MAX_CONTROLLER; ++i)
    {
        if (ui->controllers[i].active)
        {
        }
    }
}
void
nu_ui_end (nu_ui_t *ui)
{
    NU_ASSERT(ui->_renderer);
}
void
nu_ui_submit_renderpasses (const nu_ui_t                *ui,
                           nu_renderer_t                *renderer,
                           const nu_renderpass_submit_t *info)
{
    for (nu_size_t i = ui->_passes.size; i > 0; --i)
    {
        nu_renderpass_submit(
            renderer, ui->_passes.data[i - 1].renderpass, info);
    }
}

void
nu_ui_push_style (nu_ui_t *ui, nu_ui_style_t *style)
{
    nu_vec_push(&ui->_styles, ui->_allocator);
    nu__ui_style_t *s = nu_vec_last(&ui->_styles);

    s->data = style;
    switch (style->type)
    {
        case NU_UI_BUTTON:
            s->prev           = ui->_button_style;
            ui->_button_style = style;
            break;
        case NU_UI_CHECKBOX:
            s->prev             = ui->_checkbox_style;
            ui->_checkbox_style = style;
            break;
    }
}
void
nu_ui_pop_style (nu_ui_t *ui)
{
    nu__ui_style_t *last = nu_vec_last(&ui->_styles);
    if (last)
    {
        switch (last->data->type)
        {
            case NU_UI_BUTTON:
                ui->_button_style = last->prev;
                break;
            case NU_UI_CHECKBOX:
                ui->_checkbox_style = last->prev;
                break;
        }
        nu_vec_pop(&ui->_styles);
    }
}

nu_u32_t
nu_ui_controller (const nu_ui_t *ui)
{
    return ui->_active_controller;
}

nu_bool_t
nu_ui_button (nu_ui_t *ui, nu_rect_t extent)
{
    nu_blit_sliced(ui->_renderer,
                   ui->active_renderpass,
                   ui->_button_style->button.hovered.material,
                   extent,
                   ui->_button_style->button.hovered.extent,
                   ui->_button_style->button.hovered.margin);
    return NU_FALSE;
}

#endif

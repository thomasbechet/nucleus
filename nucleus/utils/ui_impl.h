#ifndef NU_UI_IMPL_H
#define NU_UI_IMPL_H

#include <nucleus/utils/ui.h>

static void
nu__blit_sliced (nu_renderer_t          renderer,
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
        nu_draw_blit(
            renderer,
            pass,
            material,
            nu_rect(extent.p.x, extent.p.y, margin.left, margin.top),
            nu_rect(tex_extent.p.x, tex_extent.p.y, margin.left, margin.top));
    }

    // Top-Mid
    if (margin.top)
    {
        nu_draw_blit(renderer,
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
        nu_draw_blit(renderer,
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
        nu_draw_blit(renderer,
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
    nu_draw_blit(renderer,
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
        nu_draw_blit(renderer,
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
        nu_draw_blit(renderer,
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
        nu_draw_blit(renderer,
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
        nu_draw_blit(renderer,
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

static void
nu__draw_image (nu_ui_t *ui, nu_rect_t extent, const nu_ui_image_style_t *style)
{
    nu__blit_sliced(ui->_renderer,
                    ui->active_renderpass,
                    style->material,
                    extent,
                    style->extent,
                    style->margin);
}

nu_error_t
nu_ui_init (nu_renderer_t renderer, nu_allocator_t *alloc, nu_ui_t *ui)
{
    ui->_allocator = alloc;

    nu_vec_init(&ui->_styles, alloc, 10);
    ui->_button_style   = NU_NULL;
    ui->_checkbox_style = NU_NULL;
    ui->_cursor_style   = NU_NULL;

    ui->_active_id         = 0;
    ui->_hot_id            = 0;
    ui->_active_controller = 0;
    ui->_hot_controller    = 0;
    ui->_renderer          = renderer;

    nu_vec_init(&ui->_passes, alloc, 1);

    // Create main renderpass
    nu_renderpass_info_t info;
    info.type = NU_RENDERPASS_CANVAS;
    nu_error_t error
        = nu_renderpass_create(renderer, &info, &ui->active_renderpass);
    NU_ERROR_CHECK(error, return error);
    nu__ui_pass_t *pass = nu_vec_push(&ui->_passes, alloc);
    pass->renderpass    = ui->active_renderpass;

    // Initialize controllers
    for (nu_size_t i = 0; i < NU_UI_MAX_CONTROLLER; ++i)
    {
        ui->controllers[i].active       = NU_FALSE;
        ui->controllers[i].cursor       = NU_IVEC2_ZERO;
        ui->controllers[i].main_pressed = NU_FALSE;
        ui->controllers[i].mode         = NU_UI_CONTROLLER_CURSOR;
    }

    return NU_ERROR_NONE;
}
void
nu_ui_free (nu_ui_t *ui, nu_renderer_t renderer)
{
    for (nu_size_t i = 0; i < ui->_passes.size; ++i)
    {
        nu_renderpass_reset(renderer, ui->_passes.data[i].renderpass);
    }
    nu_vec_free(&ui->_passes, ui->_allocator);
    nu_vec_free(&ui->_styles, ui->_allocator);
}

void
nu_ui_begin (nu_ui_t *ui, nu_renderer_t renderer)
{
    // Reset renderpass
    ui->_renderer = renderer;
    for (nu_size_t i = 0; i < ui->_passes.size; ++i)
    {
        nu_renderpass_reset(renderer, ui->_passes.data[i].renderpass);
    }

    ui->_next_id = 1;
    ui->_hot_id  = 0;

    NU_ASSERT(ui->_button_style);
    NU_ASSERT(ui->_checkbox_style);
}
void
nu_ui_end (nu_ui_t *ui)
{
    NU_ASSERT(ui->_cursor_style);

    // Draw cursor
    nu_ivec2_t cursor = ui->controllers[0].cursor;
    nu__draw_image(ui,
                   nu_rect(cursor.x - 3, cursor.y - 3, 6, 5),
                   &ui->_cursor_style->cursor.image);
}
void
nu_ui_submit_renderpasses (const nu_ui_t                *ui,
                           nu_renderer_t                 renderer,
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
    nu__ui_style_t *s = nu_vec_push(&ui->_styles, ui->_allocator);

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
        case NU_UI_CURSOR:
            s->prev           = ui->_cursor_style;
            ui->_cursor_style = style;
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
            case NU_UI_CURSOR:
                ui->_cursor_style = last->prev;
                break;
        }
        (void)nu_vec_pop(&ui->_styles);
    }
}

nu_u32_t
nu_ui_controller (const nu_ui_t *ui)
{
    return ui->_active_controller;
}

static nu_bool_t
nu__ui_hit (const nu_ui_t *ui, nu_rect_t extent, nu_u32_t *controller)
{
    for (nu_size_t i = 0; i < NU_UI_MAX_CONTROLLER; ++i)
    {
        if (nu_rect_containsi(extent, ui->controllers[i].cursor))
        {
            *controller = i;
            return NU_TRUE;
        }
    }
    return NU_FALSE;
}

nu_bool_t
nu_ui_button (nu_ui_t *ui, nu_rect_t extent)
{
    nu_u32_t  id = ui->_next_id++;
    nu_u32_t  controller;
    nu_bool_t result  = NU_FALSE;
    nu_bool_t inside  = nu__ui_hit(ui, extent, &controller);
    nu_bool_t pressed = ui->controllers[0].main_pressed;

    if (inside)
    {
        ui->_hot_id = id;
    }

    if (ui->_active_id == id && !pressed)
    {
        if (inside)
        {
            ui->_hot_id = id;
        }
        else
        {
            ui->_hot_id = 0;
        }
        ui->_active_id = 0;
    }
    else if (ui->_hot_id == id)
    {
        if (pressed)
        {
            if (ui->_active_id != id)
            {
                result = NU_TRUE;
            }
            ui->_active_id = id;
        }
    }

    if (ui->_hot_id == id)
    {
        if (ui->_active_id == id)
        {
            nu__draw_image(ui, extent, &ui->_button_style->button.pressed);
        }
        else
        {
            nu__draw_image(ui, extent, &ui->_button_style->button.hovered);
        }
    }
    else
    {
        nu__draw_image(ui, extent, &ui->_button_style->button.released);
    }

    return result;
}
nu_bool_t
nu_ui_checkbox (nu_ui_t *ui, nu_rect_t extent, nu_bool_t *state)
{
    NU_ASSERT(state);

    nu_bool_t result = NU_FALSE;
    nu_u32_t  id     = ui->_next_id++;
    nu_u32_t  controller;
    nu_bool_t inside  = nu__ui_hit(ui, extent, &controller);
    nu_bool_t pressed = ui->controllers[0].main_pressed;

    if (inside)
    {
        ui->_hot_id = id;
        if (ui->_active_id != id && pressed)
        {
            ui->_active_id = id;
            *state         = !(*state);
            result         = NU_TRUE;
        }
        else if (!pressed)
        {
            ui->_active_id = 0;
        }
    }
    else if (ui->_active_id == id)
    {
        ui->_active_id = 0;
        ui->_hot_id    = 0;
    }

    if ((*state))
    {
        nu__draw_image(ui, extent, &ui->_checkbox_style->checkbox.checked);
    }
    else
    {
        nu__draw_image(ui, extent, &ui->_checkbox_style->checkbox.unchecked);
    }

    return result;
}

#endif

#ifndef NU_UI_IMPL_H
#define NU_UI_IMPL_H

#include <nucleus/internal.h>

static void
nu__blit_sliced (nu_renderpass_t pass,
                 nu_material_t   material,
                 nu_rect_t       extent,
                 nu_rect_t       tex_extent,
                 nu_ui_margin_t  margin)
{
    nu_u32_t tex_mid_width  = tex_extent.s.x - margin.left - margin.right;
    nu_u32_t tex_mid_height = tex_extent.s.y - margin.top - margin.bottom;
    nu_u32_t ext_mid_width  = extent.s.x - margin.left - margin.right;
    nu_u32_t ext_mid_height = extent.s.y - margin.top - margin.bottom;

    // Top-Left
    if (margin.top && margin.left)
    {
        nu_draw_blit(
            pass,
            material,
            nu_rect(extent.p.x, extent.p.y, margin.left, margin.top),
            nu_rect(tex_extent.p.x, tex_extent.p.y, margin.left, margin.top));
    }

    // Top-Mid
    if (margin.top)
    {
        nu_draw_blit(pass,
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
        nu_draw_blit(pass,
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
        nu_draw_blit(pass,
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
    nu_draw_blit(pass,
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
        nu_draw_blit(pass,
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
        nu_draw_blit(pass,
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
        nu_draw_blit(pass,
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
        nu_draw_blit(pass,
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
nu__draw_image (nu__ui_t                  *ui,
                nu_rect_t                  extent,
                const nu_ui_image_style_t *style)
{
    nu__blit_sliced(ui->active_renderpass,
                    style->material,
                    extent,
                    style->extent,
                    style->margin);
}

nu_ui_t
nu_ui_create (void)
{
    nu_ui_t   handle;
    nu__ui_t *ui = nu_pool_add(&_ctx.utils.uis, &handle.id);

    nu_vec_init(10, &ui->styles);
    ui->button_style   = NU_NULL;
    ui->checkbox_style = NU_NULL;
    ui->cursor_style   = NU_NULL;

    ui->active_id         = 0;
    ui->hot_id            = 0;
    ui->active_controller = 0;
    ui->hot_controller    = 0;

    nu_vec_init(1, &ui->passes);

    // Create main renderpass
    nu_renderpass_info_t pinfo;
    pinfo.type            = NU_RENDERPASS_CANVAS;
    ui->active_renderpass = nu_renderpass_create(&pinfo);
    nu__ui_pass_t *pass   = nu_vec_push(&ui->passes);
    pass->renderpass      = ui->active_renderpass;

    // Initialize controllers
    for (nu_size_t i = 0; i < NU_UI_MAX_CONTROLLER; ++i)
    {
        ui->controllers[i].active       = NU_FALSE;
        ui->controllers[i].cursor       = NU_IVEC2_ZERO;
        ui->controllers[i].main_pressed = NU_FALSE;
        ui->controllers[i].mode         = NU_UI_CONTROLLER_CURSOR;
    }

    ui->controllers[0].active = NU_TRUE;

    return handle;
}
void
nu_ui_delete (nu_ui_t handle)
{
    nu__ui_t *ui = &_ctx.utils.uis.data[handle.id];
    for (nu_size_t i = 0; i < ui->passes.size; ++i)
    {
        nu_renderpass_reset(ui->passes.data[i].renderpass);
    }
    nu_vec_free(&ui->passes);
    nu_vec_free(&ui->styles);
}

void
nu_ui_set_cursor (nu_ui_t handle, nu_u32_t controller, nu_ivec2_t pos)
{
    nu__ui_t *ui                       = &_ctx.utils.uis.data[handle.id];
    ui->controllers[controller].cursor = pos;
}
void
nu_ui_set_pressed (nu_ui_t handle, nu_u32_t controller, nu_bool_t pressed)
{
    nu__ui_t *ui                             = &_ctx.utils.uis.data[handle.id];
    ui->controllers[controller].main_pressed = pressed;
}

void
nu_ui_begin (nu_ui_t handle)
{
    nu__ui_t *ui = &_ctx.utils.uis.data[handle.id];
    // Reset renderpass
    for (nu_size_t i = 0; i < ui->passes.size; ++i)
    {
        nu_renderpass_reset(ui->passes.data[i].renderpass);
    }

    ui->next_id = 1;
    ui->hot_id  = 0;

    nu_assert(ui->button_style);
    nu_assert(ui->checkbox_style);
}
void
nu_ui_end (nu_ui_t handle)
{
    nu__ui_t *ui = &_ctx.utils.uis.data[handle.id];
    nu_assert(ui->cursor_style);

    // Draw cursor
    nu_ivec2_t cursor = ui->controllers[0].cursor;
    nu__draw_image(ui,
                   nu_rect(cursor.x - 3, cursor.y - 3, 6, 5),
                   &ui->cursor_style->cursor.image);
}
void
nu_ui_submit_renderpasses (nu_ui_t handle, const nu_renderpass_submit_t *info)
{
    nu__ui_t *ui = &_ctx.utils.uis.data[handle.id];
    for (nu_size_t i = ui->passes.size; i > 0; --i)
    {
        nu_renderpass_submit(ui->passes.data[i - 1].renderpass, info);
    }
}

void
nu_ui_push_style (nu_ui_t handle, nu_ui_style_t *style)
{
    nu__ui_t       *ui = &_ctx.utils.uis.data[handle.id];
    nu__ui_style_t *s  = nu_vec_push(&ui->styles);

    s->data = style;
    switch (style->type)
    {
        case NU_UI_BUTTON:
            s->prev          = ui->button_style;
            ui->button_style = style;
            break;
        case NU_UI_CHECKBOX:
            s->prev            = ui->checkbox_style;
            ui->checkbox_style = style;
            break;
        case NU_UI_CURSOR:
            s->prev          = ui->cursor_style;
            ui->cursor_style = style;
            break;
    }
}
void
nu_ui_pop_style (nu_ui_t handle)
{
    nu__ui_t       *ui   = &_ctx.utils.uis.data[handle.id];
    nu__ui_style_t *last = nu_vec_last(&ui->styles);
    if (last)
    {
        switch (last->data->type)
        {
            case NU_UI_BUTTON:
                ui->button_style = last->prev;
                break;
            case NU_UI_CHECKBOX:
                ui->checkbox_style = last->prev;
                break;
            case NU_UI_CURSOR:
                ui->cursor_style = last->prev;
                break;
        }
        (void)nu_vec_pop(&ui->styles);
    }
}

nu_u32_t
nu_ui_controller (nu_ui_t handle)
{
    nu__ui_t *ui = &_ctx.utils.uis.data[handle.id];
    return ui->active_controller;
}

static nu_bool_t
nu__ui_hit (nu__ui_t *ui, nu_rect_t extent, nu_u32_t *controller)
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
nu_ui_button (nu_ui_t handle, nu_rect_t extent)
{
    nu__ui_t *ui = &_ctx.utils.uis.data[handle.id];
    nu_u32_t  id = ui->next_id++;
    nu_u32_t  controller;
    nu_bool_t result  = NU_FALSE;
    nu_bool_t inside  = nu__ui_hit(ui, extent, &controller);
    nu_bool_t pressed = ui->controllers[0].main_pressed;

    if (inside)
    {
        ui->hot_id = id;
    }

    if (ui->active_id == id && !pressed)
    {
        if (inside)
        {
            ui->hot_id = id;
        }
        else
        {
            ui->hot_id = 0;
        }
        ui->active_id = 0;
    }
    else if (ui->hot_id == id)
    {
        if (pressed)
        {
            if (ui->active_id != id)
            {
                result = NU_TRUE;
            }
            ui->active_id = id;
        }
    }

    if (ui->hot_id == id)
    {
        if (ui->active_id == id)
        {
            nu__draw_image(ui, extent, &ui->button_style->button.pressed);
        }
        else
        {
            nu__draw_image(ui, extent, &ui->button_style->button.hovered);
        }
    }
    else
    {
        nu__draw_image(ui, extent, &ui->button_style->button.released);
    }

    return result;
}
nu_bool_t
nu_ui_checkbox (nu_ui_t handle, nu_rect_t extent, nu_bool_t *state)
{
    nu__ui_t *ui = &_ctx.utils.uis.data[handle.id];
    nu_assert(state);

    nu_bool_t result = NU_FALSE;
    nu_u32_t  id     = ui->next_id++;
    nu_u32_t  controller;
    nu_bool_t inside  = nu__ui_hit(ui, extent, &controller);
    nu_bool_t pressed = ui->controllers[0].main_pressed;

    if (inside)
    {
        ui->hot_id = id;
        if (ui->active_id != id && pressed)
        {
            ui->active_id = id;
            *state        = !(*state);
            result        = NU_TRUE;
        }
        else if (!pressed)
        {
            ui->active_id = 0;
        }
    }
    else if (ui->active_id == id)
    {
        ui->active_id = 0;
        ui->hot_id    = 0;
    }

    if ((*state))
    {
        nu__draw_image(ui, extent, &ui->checkbox_style->checkbox.checked);
    }
    else
    {
        nu__draw_image(ui, extent, &ui->checkbox_style->checkbox.unchecked);
    }

    return result;
}

#endif

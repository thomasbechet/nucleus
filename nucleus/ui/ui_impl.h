#ifndef NU_UI_IMPL_H
#define NU_UI_IMPL_H

#include <nucleus/internal.h>

static void
nu__draw_image (nu__ui_instance_t          *ui,
                nu_b2i_t                  extent,
                const nu__ui_image_style_t *style)
{
    nu_draw_blit_sliced(ui->active_renderpass,
                        extent,
                        style->extent,
                        style->inner,
                        style->material);
}

nu_ui_style_t
nu_ui_style_create (void)
{
    nu_size_t       index;
    nu__ui_style_t *style = NU_POOL_ADD(&_ctx.ui.styles, &index);
    return NU_HANDLE_MAKE(nu_ui_style_t, index);
}
void
nu_ui_style_delete (nu_ui_style_t style)
{
}
void
nu_ui_style (nu_ui_style_t          style,
             nu_ui_style_property_t property,
             nu_material_t          material,
             nu_b2i_t             extent,
             nu_b2i_t             inner)
{
    nu__ui_style_t *s = _ctx.ui.styles.data + NU_HANDLE_INDEX(style);
    switch (property)
    {
        case NU_UI_STYLE_BUTTON_PRESSED:
            s->button.pressed.material = material;
            s->button.pressed.extent   = extent;
            s->button.pressed.inner    = inner;
            break;
        case NU_UI_STYLE_BUTTON_RELEASED:
            s->button.released.material = material;
            s->button.released.extent   = extent;
            s->button.released.inner    = inner;
            break;
        case NU_UI_STYLE_BUTTON_HOVERED:
            s->button.hovered.material = material;
            s->button.hovered.extent   = extent;
            s->button.hovered.inner    = inner;
            break;
        case NU_UI_STYLE_CHECKBOX_CHECKED:
            s->checkbox.checked.material = material;
            s->checkbox.checked.extent   = extent;
            s->checkbox.checked.inner    = inner;
            break;
        case NU_UI_STYLE_CHECKBOX_UNCHECKED:
            s->checkbox.unchecked.material = material;
            s->checkbox.unchecked.extent   = extent;
            s->checkbox.unchecked.inner    = inner;
            break;
        case NU_UI_STYLE_CURSOR:
            s->cursor.image.material = material;
            s->cursor.image.extent   = extent;
            s->cursor.image.inner    = inner;
            break;
    }
}

nu_ui_t
nu_ui_create (void)
{
    nu_size_t          index;
    nu__ui_instance_t *ui = NU_POOL_ADD(&_ctx.ui.uis, &index);

    NU_VEC_INIT(10, &ui->styles);
    ui->active_style      = NU_NULL;
    ui->active_id         = 0;
    ui->hot_id            = 0;
    ui->active_controller = 0;
    ui->hot_controller    = 0;

    // Create main renderpass
    ui->active_renderpass = nu_renderpass_create(NU_RENDERPASS_CANVAS);
    nu_renderpass_set_reset_after_submit(ui->active_renderpass, NU_FALSE);

    // Initialize controllers
    for (nu_size_t i = 0; i < NU_UI_MAX_CONTROLLER; ++i)
    {
        ui->controllers[i].active       = NU_FALSE;
        ui->controllers[i].cursor       = NU_VEC2I_ZEROS;
        ui->controllers[i].main_pressed = NU_FALSE;
        ui->controllers[i].mode         = NU_UI_CONTROLLER_CURSOR;
    }

    ui->controllers[0].active = NU_TRUE;

    return NU_HANDLE_MAKE(nu_ui_t, index);
}
void
nu_ui_delete (nu_ui_t handle)
{
    nu__ui_instance_t *ui = &_ctx.ui.uis.data[NU_HANDLE_INDEX(handle)];
    NU_VEC_FREE(&ui->styles);
}

void
nu_ui_set_cursor (nu_ui_t handle, nu_u32_t controller, nu_v2i_t pos)
{
    nu__ui_instance_t *ui = &_ctx.ui.uis.data[NU_HANDLE_INDEX(handle)];
    ui->controllers[controller].cursor = pos;
}
void
nu_ui_set_pressed (nu_ui_t handle, nu_u32_t controller, nu_bool_t pressed)
{
    nu__ui_instance_t *ui = &_ctx.ui.uis.data[NU_HANDLE_INDEX(handle)];
    ui->controllers[controller].main_pressed = pressed;
}

void
nu_ui_begin (nu_ui_t handle, nu_renderpass_t renderpass)
{
    nu__ui_instance_t *ui = &_ctx.ui.uis.data[NU_HANDLE_INDEX(handle)];
    ui->next_id           = 1;
    ui->hot_id            = 0;
    NU_ASSERT(ui->active_style);
    ui->active_renderpass = renderpass;
}
void
nu_ui_end (nu_ui_t handle)
{
    nu__ui_instance_t *ui = &_ctx.ui.uis.data[NU_HANDLE_INDEX(handle)];
    nu__ui_style_t *s = _ctx.ui.styles.data + NU_HANDLE_INDEX(ui->active_style);

    // Draw cursor
    nu_v2i_t cursor = ui->controllers[0].cursor;
    nu__draw_image(
        ui, nu_b2i_xywh(cursor.x - 4, cursor.y - 4, 8, 7), &s->cursor.image);
}

void
nu_ui_push_style (nu_ui_t handle, nu_ui_style_t style)
{
    nu__ui_instance_t *ui     = &_ctx.ui.uis.data[NU_HANDLE_INDEX(handle)];
    *NU_VEC_PUSH(&ui->styles) = ui->active_style;
    ui->active_style          = style;
}
void
nu_ui_pop_style (nu_ui_t handle)
{
    nu__ui_instance_t *ui = &_ctx.ui.uis.data[NU_HANDLE_INDEX(handle)];
    ui->active_style      = *NU_VEC_POP(&ui->styles);
}

nu_u32_t
nu_ui_controller (nu_ui_t handle)
{
    nu__ui_instance_t *ui = &_ctx.ui.uis.data[NU_HANDLE_INDEX(handle)];
    return ui->active_controller;
}

static nu_bool_t
nu__ui_hit (nu__ui_instance_t *ui, nu_b2i_t extent, nu_u32_t *controller)
{
    for (nu_size_t i = 0; i < NU_UI_MAX_CONTROLLER; ++i)
    {
        if (nu_b2i_containsi(extent, ui->controllers[i].cursor))
        {
            *controller = i;
            return NU_TRUE;
        }
    }
    return NU_FALSE;
}

nu_bool_t
nu_ui_button (nu_ui_t handle, nu_b2i_t extent)
{
    nu__ui_instance_t *ui = &_ctx.ui.uis.data[NU_HANDLE_INDEX(handle)];
    nu__ui_style_t *s = _ctx.ui.styles.data + NU_HANDLE_INDEX(ui->active_style);
    nu_u32_t        id = ui->next_id++;
    nu_u32_t        controller;
    nu_bool_t       result  = NU_FALSE;
    nu_bool_t       inside  = nu__ui_hit(ui, extent, &controller);
    nu_bool_t       pressed = ui->controllers[0].main_pressed;

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
            nu__draw_image(ui, extent, &s->button.pressed);
        }
        else
        {
            nu__draw_image(ui, extent, &s->button.hovered);
        }
    }
    else
    {
        nu__draw_image(ui, extent, &s->button.released);
    }

    return result;
}
nu_bool_t
nu_ui_checkbox (nu_ui_t handle, nu_b2i_t extent, nu_bool_t *state)
{
    nu__ui_instance_t *ui = &_ctx.ui.uis.data[NU_HANDLE_INDEX(handle)];
    nu__ui_style_t *s = _ctx.ui.styles.data + NU_HANDLE_INDEX(ui->active_style);
    NU_ASSERT(state);

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
        nu__draw_image(ui, extent, &s->checkbox.checked);
    }
    else
    {
        nu__draw_image(ui, extent, &s->checkbox.unchecked);
    }

    return result;
}

static nu_error_t
nu__ui_init (void)
{
    NU_POOL_INIT(1, &_ctx.ui.uis);
    NU_POOL_INIT(1, &_ctx.ui.styles);
    return NU_ERROR_NONE;
}
static nu_error_t
nu__ui_free (void)
{
    NU_POOL_FREE(&_ctx.ui.styles);
    NU_POOL_FREE(&_ctx.ui.uis);
    return NU_ERROR_NONE;
}

#endif

#ifndef NU_UI_IMPL_H
#define NU_UI_IMPL_H

#include <nucleus/utils/ui.h>

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

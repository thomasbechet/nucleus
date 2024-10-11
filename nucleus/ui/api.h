#ifndef NU_UI_API_H
#define NU_UI_API_H

#include <nucleus/graphics/api.h>

NU_DEFINE_HANDLE(nu_ui_t);
NU_DEFINE_HANDLE(nu_ui_style_t);

typedef enum
{
    NU_UI_BUTTON,
    NU_UI_CHECKBOX,
    NU_UI_CURSOR,
} nu_ui_element_t;

typedef enum
{
    NU_UI_CONTROLLER_DISABLED,
    NU_UI_CONTROLLER_SELECTION,
    NU_UI_CONTROLLER_CURSOR,
} nu_ui_controller_mode_t;

typedef enum
{
    NU_UI_STYLE_BUTTON_PRESSED,
    NU_UI_STYLE_BUTTON_RELEASED,
    NU_UI_STYLE_BUTTON_HOVERED,
    NU_UI_STYLE_CHECKBOX_CHECKED,
    NU_UI_STYLE_CHECKBOX_UNCHECKED,
    NU_UI_STYLE_CURSOR
} nu_ui_style_property_t;

NU_API void nu_blit_sliced(nu_renderpass_t pass,
                           nu_material_t   handle,
                           nu_b2i_t      extent,
                           nu_b2i_t      tex_extent,
                           nu_b2i_t      inner);

NU_API nu_ui_style_t nu_ui_style_create(void);
NU_API void          nu_ui_style_delete(nu_ui_style_t style);
NU_API void          nu_ui_style(nu_ui_style_t          style,
                                 nu_ui_style_property_t property,
                                 nu_material_t          material,
                                 nu_b2i_t             extent,
                                 nu_b2i_t             inner);

NU_API nu_ui_t nu_ui_create(void);
NU_API void    nu_ui_delete(nu_ui_t ui);

NU_API void nu_ui_set_cursor(nu_ui_t ui, nu_u32_t controller, nu_v2i_t pos);
NU_API void nu_ui_set_pressed(nu_ui_t   ui,
                              nu_u32_t  controller,
                              nu_bool_t pressed);

NU_API void nu_ui_begin(nu_ui_t ui, nu_renderpass_t pass);
NU_API void nu_ui_end(nu_ui_t ui);

NU_API void nu_ui_push_style(nu_ui_t ui, nu_ui_style_t style);
NU_API void nu_ui_pop_style(nu_ui_t ui);

NU_API nu_u32_t nu_ui_controller(nu_ui_t ui);

NU_API nu_bool_t nu_ui_button(nu_ui_t ui, nu_b2i_t extent);
NU_API nu_bool_t nu_ui_checkbox(nu_ui_t    ui,
                                nu_b2i_t extent,
                                nu_bool_t *state);

#endif

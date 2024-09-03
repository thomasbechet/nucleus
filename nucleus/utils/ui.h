#ifndef NU_UI_H
#define NU_UI_H

#include <nucleus/graphics/graphics.h>

typedef enum
{
    NU_UI_BUTTON,
    NU_UI_CHECKBOX,
    NU_UI_CURSOR,
} nu_ui_element_t;

typedef struct
{
    nu_u32_t top;
    nu_u32_t bottom;
    nu_u32_t left;
    nu_u32_t right;
} nu_ui_margin_t;

typedef struct
{
    nu_material_t  material;
    nu_rect_t      extent;
    nu_ui_margin_t margin;
} nu_ui_image_style_t;

typedef struct
{
    nu_ui_element_t type;
    union
    {
        struct
        {
            nu_ui_image_style_t pressed;
            nu_ui_image_style_t released;
            nu_ui_image_style_t hovered;
        } button;
        struct
        {
            nu_ui_image_style_t checked;
            nu_ui_image_style_t unchecked;
        } checkbox;
        struct
        {
            nu_ui_image_style_t image;
        } cursor;
    };
} nu_ui_style_t;

typedef nu_vec(nu_ui_style_t) nu_ui_style_vec_t;

typedef enum
{
    NU_UI_CONTROLLER_DISABLED,
    NU_UI_CONTROLLER_SELECTION,
    NU_UI_CONTROLLER_CURSOR,
} nu_ui_controller_mode_t;

NU_DEFINE_HANDLE(nu_ui_t);

NU_API void nu_blit_sliced(nu_renderpass_t pass,
                           nu_material_t   handle,
                           nu_rect_t       extent,
                           nu_rect_t       tex_extent,
                           nu_ui_margin_t  margin);

NU_API nu_error_t nu_ui_create(nu_ui_t *ui);
NU_API void       nu_ui_delete(nu_ui_t ui);

NU_API void nu_ui_set_cursor(nu_ui_t ui, nu_u32_t controller, nu_ivec2_t pos);
NU_API void nu_ui_set_pressed(nu_ui_t   ui,
                              nu_u32_t  controller,
                              nu_bool_t pressed);

NU_API void nu_ui_begin(nu_ui_t ui);
NU_API void nu_ui_end(nu_ui_t ui);
NU_API void nu_ui_submit_renderpasses(nu_ui_t                       ui,
                                      const nu_renderpass_submit_t *info);

NU_API void nu_ui_push_style(nu_ui_t ui, nu_ui_style_t *style);
NU_API void nu_ui_pop_style(nu_ui_t ui);

NU_API nu_u32_t nu_ui_controller(nu_ui_t ui);

NU_API nu_bool_t nu_ui_button(nu_ui_t ui, nu_rect_t extent);
NU_API nu_bool_t nu_ui_checkbox(nu_ui_t ui, nu_rect_t extent, nu_bool_t *state);

#endif

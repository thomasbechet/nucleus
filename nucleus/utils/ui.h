#ifndef NU_UI_H
#define NU_UI_H

#include <nucleus/graphics.h>

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

typedef struct
{
    nu_ui_controller_mode_t mode;
    nu_bool_t               active;
    nu_ivec2_t              cursor;
    nu_bool_t               main_pressed;
} nu_ui_controller_t;

typedef nu_vec(nu_ui_controller_t *) nu__ui_controller_vec_t;

typedef struct
{
    nu_ui_style_t *data;
    nu_ui_style_t *prev;
} nu__ui_style_t;

typedef nu_vec(nu__ui_style_t) nu__ui_style_vec_t;

typedef struct
{
    nu_renderpass_t renderpass;
} nu__ui_pass_t;

typedef nu_vec(nu__ui_pass_t) nu__ui_pass_vec_t;

#define NU_UI_MAX_CONTROLLER 4

typedef struct
{
    nu_renderpass_t    _active_renderpass;
    nu_ui_controller_t _controllers[NU_UI_MAX_CONTROLLER];

    nu_allocator_t _allocator;

    nu__ui_pass_vec_t _passes;

    nu__ui_style_vec_t _styles;
    nu_ui_style_t     *_button_style;
    nu_ui_style_t     *_checkbox_style;
    nu_ui_style_t     *_cursor_style;

    nu_u32_t      _next_id;
    nu_u32_t      _active_id;
    nu_u32_t      _hot_id;
    nu_u32_t      _active_controller;
    nu_u32_t      _hot_controller;
    nu_renderer_t _renderer;
} nu__ui_t;

typedef struct
{
    nu_allocator_t allocator;
    nu_renderer_t  renderer;
} nu_ui_info_t;

NU_DEFINE_HANDLE_POINTER(nu_ui_t, nu__ui_t);

NU_API void nu_blit_sliced(nu_renderer_t   renderer,
                           nu_renderpass_t pass,
                           nu_material_t   handle,
                           nu_rect_t       extent,
                           nu_rect_t       tex_extent,
                           nu_ui_margin_t  margin);

NU_API nu_error_t nu_ui_create(const nu_ui_info_t *info, nu_ui_t *ui);
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

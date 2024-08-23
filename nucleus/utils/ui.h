#ifndef NU_UI_H
#define NU_UI_H

#include <nucleus/graphics.h>

typedef enum
{
    NU_UI_BUTTON,
    NU_UI_CHECKBOX,
} nu_ui_widget_t;

typedef struct
{
    nu_u32_t top;
    nu_u32_t bottom;
    nu_u32_t left;
    nu_u32_t right;
} nu_ui_margin_t;

typedef struct
{
    nu_material_handle_t material;
    nu_rect_t            extent;
    nu_ui_margin_t       margin;
} nu_ui_image_style_t;

typedef struct
{
    nu_ui_widget_t type;
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
    };
} nu_ui_style_t;

typedef nu_vec(nu_ui_style_t) nu_ui_style_vec_t;

typedef enum
{
    NU_UI_CONTROLLER_DISABLED,
    NU_UI_CONTROLLER_SELECTION,
    NU_UI_CONTROLLER_CURSOR,
} nu_ui_controller_mode_t;

typedef enum
{
    NU_UI_NONE,
    NU_UI_CLICK,
    NU_UI_UP,
    NU_UI_DOWN,
    NU_UI_LEFT,
    NU_UI_RIGHT,
} nu_ui_action_t;

typedef struct
{
    nu_ui_controller_mode_t mode;
    nu_bool_t               active;
    nu_ui_action_t          action;
    nu_ivec2_t              cursor;
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
    nu_renderpass_handle_t renderpass;
} nu__ui_pass_t;

typedef nu_vec(nu__ui_pass_t) nu__ui_pass_vec_t;

#define NU_UI_MAX_CONTROLLER 4

typedef struct
{
    nu_renderpass_handle_t active_renderpass;
    nu_ui_controller_t     controllers[NU_UI_MAX_CONTROLLER];

    nu_allocator_t *_allocator;

    nu__ui_pass_vec_t _passes;

    nu__ui_style_vec_t _styles;
    nu_ui_style_t     *_button_style;
    nu_ui_style_t     *_checkbox_style;

    nu_u32_t       _active_widget;
    nu_u32_t       _hot_widget;
    nu_u32_t       _active_controller;
    nu_u32_t       _hot_controller;
    nu_renderer_t *_renderer; // Not null in build phase
} nu_ui_t;

NU_API void nu_blit_sliced(nu_renderer_t         *renderer,
                           nu_renderpass_handle_t pass,
                           nu_material_handle_t   handle,
                           nu_rect_t              extent,
                           nu_rect_t              tex_extent,
                           nu_ui_margin_t         margin);

NU_API nu_error_t nu_ui_init(nu_renderer_t  *renderer,
                             nu_allocator_t *alloc,
                             nu_ui_t        *ui);
NU_API void       nu_ui_free(nu_ui_t *ui, nu_renderer_t *renderer);

NU_API void nu_ui_begin(nu_ui_t *ui, nu_renderer_t *renderer);
NU_API void nu_ui_end(nu_ui_t *ui);
NU_API void nu_ui_submit_renderpasses(const nu_ui_t                *ui,
                                      nu_renderer_t                *renderer,
                                      const nu_renderpass_submit_t *info);

NU_API void nu_ui_push_style(nu_ui_t *ui, nu_ui_style_t *style);
NU_API void nu_ui_pop_style(nu_ui_t *ui);

NU_API nu_u32_t nu_ui_controller(const nu_ui_t *ui);

NU_API nu_bool_t nu_ui_button(nu_ui_t *ui, nu_rect_t extent);

#endif

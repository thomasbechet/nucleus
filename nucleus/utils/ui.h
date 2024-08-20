#ifndef NU_UI_H
#define NU_UI_H

#include <nucleus/graphics.h>

typedef enum
{
    NU_UI_CONTROLLER_DISABLED,
    NU_UI_CONTROLLER_SELECTION,
    NU_UI_CONTROLLER_CURSOR,
} nu_ui_controller_mode_t;

typedef enum
{
    NU_UI_BUTTON,
    NU_UI_CHECKBOX,
} nu_ui_element_t;

typedef struct
{
    nu_u32_t top;
    nu_u32_t bottom;
    nu_u32_t left;
    nu_u32_t right;
} nu_margin_t;

typedef struct
{
    nu_material_handle_t material;
    nu_ui_element_t      type;
    union
    {
        struct
        {
            struct
            {
                nu_rect_t   extent;
                nu_margin_t margin;
            } pressed;
            struct
            {
                nu_rect_t   extent;
                nu_margin_t margin;
            } released;
            struct
            {
                nu_rect_t   extent;
                nu_margin_t margin;
            } hovered;
        } button;
    };
} nu_ui_style_t;

typedef struct
{
    nu_input_handle_t       click;
    nu_input_handle_t       move_up;
    nu_input_handle_t       move_down;
    nu_input_handle_t       move_left;
    nu_input_handle_t       move_right;
    nu_ui_controller_mode_t mode;
    nu_ivec2_t              cursor;
} nu__ui_controller_t;

typedef union
{
    nu_renderpass_handle_t renderpass;
    nu__ui_controller_t    controller;
    nu_ui_style_t          style;
} nu__ui_object_t;

typedef nu_slotmap(nu__ui_object_t) nu__ui_object_slotmap_t;

typedef struct
{
    nu__ui_object_slotmap_t _objects;
    nu_slot_t               _main_renderpass;
    nu_slot_t               _first_controller;
    nu_slot_t               _button_style;
    nu_slot_t               _checkbox_style;

    nu_bool_t      _building;
    nu_renderer_t *_renderer; // Not null in build phase
    nu_slot_t      _active_renderpass;
} nu_ui_t;

NU_API void nu_blit_sliced(nu_renderer_t         *renderer,
                           nu_renderpass_handle_t pass,
                           nu_material_handle_t   handle,
                           nu_rect_t              extent,
                           nu_rect_t              tex_extent,
                           nu_margin_t            margin);

NU_API nu_error_t nu_ui_init(nu_allocator_t *alloc, nu_ui_t *ui);
NU_API void       nu_ui_free(nu_ui_t *ui, nu_allocator_t *alloc);

NU_API void nu_ui_begin(nu_ui_t       *ui,
                        nu_platform_t *platform,
                        nu_renderer_t *renderer);
NU_API void nu_ui_end(nu_ui_t *ui);
NU_API void nu_ui_submit_renderpasses(const nu_ui_t                *ui,
                                      nu_renderer_t                *renderer,
                                      const nu_renderpass_submit_t *info);

NU_API nu_renderpass_handle_t nu_ui_active_renderpass(const nu_ui_t *ui);

NU_API nu_bool_t nu_ui_button(nu_ui_t *ui, nu_rect_t r);

#endif

#ifndef NU_UI_H
#define NU_UI_H

#include <nucleus/ui/api.h>

typedef struct
{
    nu_ui_controller_mode_t mode;
    nu_bool_t               active;
    nu_v2i_t                cursor;
    nu_bool_t               main_pressed;
} nu_ui_controller_t;

typedef struct
{
    nu_material_t material;
    nu_b2i_t      extent;
    nu_b2i_t      inner;
} nu__ui_image_style_t;

typedef struct
{
    struct
    {
        nu__ui_image_style_t pressed;
        nu__ui_image_style_t released;
        nu__ui_image_style_t hovered;
    } button;
    struct
    {
        nu__ui_image_style_t checked;
        nu__ui_image_style_t unchecked;
    } checkbox;
    struct
    {
        nu__ui_image_style_t image;
    } cursor;
} nu__ui_style_t;

typedef struct
{
    nu_renderpass_t    active_renderpass;
    nu_ui_controller_t controllers[NU_UI_CONTROLLER_MAX];

    NU_FIXEDVEC(nu_ui_style_t) styles;
    nu_ui_style_t active_style;

    nu_u32_t next_id;
    nu_u32_t active_id;
    nu_u32_t hot_id;
    nu_u32_t active_controller;
    nu_u32_t hot_controller;
} nu__ui_instance_t;

typedef struct
{
    nu_object_type_t obj_ui;
    nu_object_type_t obj_ui_style;
} nu__ui_t;

static nu_error_t nu__ui_init(void);
static nu_error_t nu__ui_free(void);

#endif

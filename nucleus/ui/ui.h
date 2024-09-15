#ifndef NU_UI_H
#define NU_UI_H

#include <nucleus/ui/api.h>

typedef struct
{
    nu_ui_controller_mode_t mode;
    nu_bool_t               active;
    nu_ivec2_t              cursor;
    nu_bool_t               main_pressed;
} nu_ui_controller_t;

typedef NU_VEC(nu_ui_controller_t *) nu__ui_controller_vec_t;

typedef struct
{
    nu_ui_style_t *data;
    nu_ui_style_t *prev;
} nu__ui_style_t;

typedef NU_VEC(nu__ui_style_t) nu__ui_style_vec_t;

typedef struct
{
    nu_renderpass_t renderpass;
} nu__ui_pass_t;

typedef NU_VEC(nu__ui_pass_t) nu__ui_pass_vec_t;

#define NU_UI_MAX_CONTROLLER 4

typedef struct
{
    nu_renderpass_t    active_renderpass;
    nu_ui_controller_t controllers[NU_UI_MAX_CONTROLLER];

    nu__ui_pass_vec_t passes;

    nu__ui_style_vec_t styles;
    nu_ui_style_t     *button_style;
    nu_ui_style_t     *checkbox_style;
    nu_ui_style_t     *cursor_style;

    nu_u32_t next_id;
    nu_u32_t active_id;
    nu_u32_t hot_id;
    nu_u32_t active_controller;
    nu_u32_t hot_controller;
} nu__ui_instance_t;

typedef NU_POOL(nu__ui_instance_t) nu__ui_pool_t;

typedef struct
{
    nu__ui_pool_t uis;
} nu__ui_t;

static nu_error_t nu__ui_init(void);
static nu_error_t nu__ui_free(void);

#endif

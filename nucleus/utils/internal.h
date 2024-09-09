#ifndef NU_UTILS_INTERNAL_H
#define NU_UTILS_INTERNAL_H

#include <nucleus/utils/controller.h>
#include <nucleus/utils/geometry.h>
#include <nucleus/utils/ui.h>

//////////////////////////////////////////////////////////////////////////
//////                               UI                             //////
//////////////////////////////////////////////////////////////////////////

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
} nu__ui_t;

typedef nu_pool(nu__ui_t) nu__ui_pool_t;

//////////////////////////////////////////////////////////////////////////
//////                       Camera Controller                      //////
//////////////////////////////////////////////////////////////////////////

typedef struct
{
    nu_vec3_t pos;
    nu_vec3_t vel;
    nu_vec3_t acc;
    nu_quat_t rot;
    float     yaw;
    float     pitch;
    nu_bool_t free_mode;

    float fov;
    float speed;

    nu_camera_controller_info_t info;
} nu__camera_controller_t;

typedef nu_pool(nu__camera_controller_t) nu__camera_controller_pool_t;

//////////////////////////////////////////////////////////////////////////
//////                           Geometry                           //////
//////////////////////////////////////////////////////////////////////////

typedef struct
{
    nu_vec3_vec_t positions;
    nu_vec2_vec_t uvs;
    nu_vec3_vec_t normals;
} nu__geometry_t;

typedef nu_pool(nu__geometry_t) nu__geometry_pool_t;

//////////////////////////////////////////////////////////////////////////
//////                          Module                              //////
//////////////////////////////////////////////////////////////////////////

typedef struct
{
    nu__ui_pool_t                uis;
    nu__camera_controller_pool_t controllers;
    nu__geometry_pool_t          geometries;
} nu__utils_t;

static nu_error_t nu__utils_init(void);
static nu_error_t nu__utils_free(void);

#endif

#ifndef NU_RENDERPASS_H
#define NU_RENDERPASS_H

#include <nucleus/graphics/texture.h>
#include <nucleus/graphics/camera.h>
#include <nucleus/graphics/cubemap.h>

typedef enum
{
    NU_RENDERPASS_UNLIT,
    NU_RENDERPASS_FLAT,
    NU_RENDERPASS_TRANSPARENT,
    NU_RENDERPASS_WIREFRAME,
    NU_RENDERPASS_SKYBOX,
    NU_RENDERPASS_CANVAS,
} nu_renderpass_type_t;

typedef struct
{
    nu_renderpass_type_t type;
    nu_bool_t            reset_after_submit;
    union
    {
        nu_u32_t todo;
    };
} nu_renderpass_info_t;

typedef union
{
    struct
    {
        nu_camera_t         camera;
        const nu_texture_t *color_target;
        const nu_texture_t *depth_target;
        nu_color_t         *clear_color;
        nu_cubemap_t       *clear_skybox;
    } unlit;
    struct
    {
        nu_camera_t         camera;
        const nu_texture_t *color_target;
        const nu_texture_t *depth_target;
        nu_color_t         *clear_color;
    } flat;
    struct
    {
        nu_camera_t         camera;
        const nu_texture_t *color_target;
        const nu_texture_t *depth_target;
        nu_cubemap_t        cubemap;
        nu_quat_t           rotation;
    } skybox;
    struct
    {
        const nu_texture_t *color_target;
    } canvas;
} nu_renderpass_submit_t;

NU_DEFINE_HANDLE(nu_renderpass_t);

#endif

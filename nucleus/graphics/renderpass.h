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
    int todo;
} nu_renderpass_unlit_info_t;

typedef struct
{
    int todo;
} nu_renderpass_flat_info_t;

typedef struct
{
    int todo;
} nu_renderpass_transparent_info_t;

typedef struct
{
    int todo;
} nu_renderpass_skybox_info_t;

typedef struct
{
    nu_renderpass_type_t type;
    nu_bool_t            reset_after_submit;
    union
    {
        nu_renderpass_unlit_info_t       unlit;
        nu_renderpass_flat_info_t        flat;
        nu_renderpass_transparent_info_t transparent;
    };
} nu_renderpass_info_t;

typedef struct
{
    nu_camera_handle_t         camera;
    const nu_texture_handle_t *color_target;
    const nu_texture_handle_t *depth_target;
    nu_color_t                *clear_color;
    nu_cubemap_handle_t       *clear_skybox;
} nu_renderpass_submit_unlit_t;

typedef struct
{
    nu_camera_handle_t         camera;
    const nu_texture_handle_t *color_target;
    const nu_texture_handle_t *depth_target;
    nu_color_t                *clear_color;
} nu_renderpass_submit_flat_t;

typedef struct
{
    nu_camera_handle_t         camera;
    const nu_texture_handle_t *color_target;
    const nu_texture_handle_t *depth_target;
    nu_cubemap_handle_t        cubemap;
    nu_quat_t                  rotation;
} nu_renderpass_submit_skybox_t;

typedef struct
{
    const nu_texture_handle_t *color_target;
} nu_renderpass_submit_canvas_t;

typedef union
{
    nu_renderpass_submit_unlit_t  unlit;
    nu_renderpass_submit_flat_t   flat;
    nu_renderpass_submit_skybox_t skybox;
    nu_renderpass_submit_canvas_t canvas;
} nu_renderpass_submit_t;

typedef union
{
#ifdef NU_BUILD_RENDERER_GL
    nugl__handle_t _gl;
#endif
} nu_renderpass_handle_t;

#endif

#ifndef NU_RENDERPASS_H
#define NU_RENDERPASS_H

#include <nucleus/graphics/backend.h>

typedef struct
{
    nu_camera_t    camera;
    nu_shademode_t mode;
    nu_lightenv_t  lightenv;
} nu__renderpass_forward_t;

typedef struct
{
    nu_texture_t depthmap;
    nu_camera_t  camera;
} nu__renderpass_shadow_t;

typedef struct
{
#ifdef NU_BUILD_GL
    nugl__renderpass_t gl;
#endif
    nu_renderpass_type_t type;
    nu_bool_t            reset_after_submit;
    nu_texture_t         depth_target;
    nu_texture_t         color_target;
    nu_color_t           clear_color;
    nu_bool_t            has_clear_color;
    union
    {
        nu__renderpass_forward_t forward;
        nu__renderpass_shadow_t  shadow;
    };
} nu__renderpass_t;

typedef NU_POOL(nu__renderpass_t) nu__renderpass_pool_t;

#endif

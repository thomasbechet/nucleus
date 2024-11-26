#ifndef NU_LIGHT_IMPL_H
#define NU_LIGHT_IMPL_H

#include <nucleus/internal.h>

nu_light_t
nu_light_new (nu_light_type_t type)
{
    nu__light_t *p = nu_object_new(_ctx.graphics.obj_light);
    p->type        = type;
    return (nu_light_t)p;
}
void
nu_light_set_position (nu_light_t light, nu_v3_t v)
{
    nu__light_t *p = (nu__light_t *)light;
    p->position    = v;
}
void
nu_light_set_rotation (nu_light_t light, nu_q4_t q)
{
    nu__light_t *p = (nu__light_t *)light;
    p->rotation    = q;
}

nu_lightenv_t
nu_lightenv_new (void)
{
    nu__lightenv_t *p   = nu_object_new(_ctx.graphics.obj_lightenv);
    p->shadowmap        = NU_NULL;
    p->shadowmap_camera = NU_NULL;
    p->skybox           = NU_NULL;
    p->skybox_rotation  = nu_m3_identity();
    return (nu_lightenv_t)p;
}
void
nu_lightenv_add_light (nu_lightenv_t env, nu_light_t light)
{
}
void
nu_lightenv_add_shadowmap (nu_lightenv_t env,
                           nu_texture_t  shadowmap,
                           nu_camera_t   camera)
{
    nu__lightenv_t *p   = (nu__lightenv_t *)env;
    p->shadowmap        = shadowmap;
    p->shadowmap_camera = camera;
}
void
nu_lightenv_set_skybox (nu_lightenv_t env,
                        nu_texture_t  cubemap,
                        nu_q4_t       rotation)
{
    nu__lightenv_t *p  = (nu__lightenv_t *)env;
    p->skybox          = cubemap;
    p->skybox_rotation = nu_q4_mat3(rotation);
}

#endif

#ifndef NU_LIGHT_IMPL_H
#define NU_LIGHT_IMPL_H

#include <nucleus/internal.h>

nu_light_t
nu_light_create (nu_light_type_t type)
{
    nu_size_t    index;
    nu__light_t *p = NU_POOL_ADD(&_ctx.graphics.lights, &index);
    p->type        = type;
    return NU_HANDLE_MAKE(nu_light_t, index);
}
void
nu_light_delete (nu_light_t light)
{
    NU_POOL_REMOVE(&_ctx.graphics.lights, NU_HANDLE_INDEX(light));
}
void
nu_light_set_position (nu_light_t light, nu_v3_t v)
{
    nu__light_t *p = _ctx.graphics.lights.data + NU_HANDLE_INDEX(light);
    p->position    = v;
}
void
nu_light_set_rotation (nu_light_t light, nu_q4_t q)
{
    nu__light_t *p = _ctx.graphics.lights.data + NU_HANDLE_INDEX(light);
    p->rotation    = q;
}

nu_lightenv_t
nu_lightenv_create (void)
{
    nu_size_t       index;
    nu__lightenv_t *p   = NU_POOL_ADD(&_ctx.graphics.lightenvs, &index);
    p->shadowmap        = NU_NULL;
    p->shadowmap_camera = NU_NULL;
    p->skybox           = NU_NULL;
    p->skybox_rotation  = nu_m3_identity();
    return NU_HANDLE_MAKE(nu_lightenv_t, index);
}
void
nu_lightenv_delete (nu_lightenv_t env)
{
    NU_POOL_REMOVE(&_ctx.graphics.lightenvs, NU_HANDLE_INDEX(env));
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
    nu__lightenv_t *p   = _ctx.graphics.lightenvs.data + NU_HANDLE_INDEX(env);
    p->shadowmap        = shadowmap;
    p->shadowmap_camera = camera;
}
void
nu_lightenv_set_skybox (nu_lightenv_t env,
                        nu_texture_t  cubemap,
                        nu_q4_t       rotation)
{
    nu__lightenv_t *p  = _ctx.graphics.lightenvs.data + NU_HANDLE_INDEX(env);
    p->skybox          = cubemap;
    p->skybox_rotation = nu_q4_mat3(rotation);
}

#endif

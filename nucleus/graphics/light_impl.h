#ifndef NU_LIGHT_IMPL_H
#define NU_LIGHT_IMPL_H

#include <nucleus/internal.h>

nu_light_t
nu_light_create (nu_light_type_t type)
{
#ifdef NU_BUILD_GL
    return nugl__light_create(type);
#endif
}
void
nu_light_delete (nu_light_t light)
{
#ifdef NU_BUILD_GL
    nugl__light_delete(light);
#endif
}
void
nu_light_set_position (nu_light_t light, nu_v3_t v)
{
#ifdef NU_BUILD_GL
    nugl__light_set_position(light, v);
#endif
}
void
nu_light_set_rotation (nu_light_t light, nu_q4_t q)
{
#ifdef NU_BUILD_GL
    nugl__light_set_rotation(light, q);
#endif
}

nu_lightenv_t
nu_lightenv_create (void)
{
#ifdef NU_BUILD_GL
    return nugl__lightenv_create();
#endif
}
void
nu_lightenv_delete (nu_lightenv_t env)
{
#ifdef NU_BUILD_GL
    nugl__lightenv_delete(env);
#endif
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
#ifdef NU_BUILD_GL
    nugl__lightenv_add_shadowmap(env, shadowmap, camera);
#endif
}
void
nu_lightenv_set_skybox (nu_lightenv_t env,
                        nu_texture_t  cubemap,
                        nu_q4_t       rotation)
{
#ifdef NU_BUILD_GL
    nugl__lightenv_set_skybox(env, cubemap, rotation);
#endif
}

#endif

#ifndef NUGL_LIGHT_IMPL_H
#define NUGL_LIGHT_IMPL_H

#include <nucleus/internal.h>

static nu_light_t
nugl__light_create (nu_light_type_t type)
{
    nu__gl_t      *gl     = &_ctx.gl;
    nugl__light_t *p      = NU_VEC_PUSH(&gl->lights);
    nu_light_t     handle = NU_HANDLE_MAKE(nu_light_t, gl->lights.size - 1);
    p->type               = type;
    return handle;
}
static void
nugl__light_delete (nu_light_t light)
{
}
static void
nugl__light_set_position (nu_light_t light, nu_v3_t v)
{
    nugl__light_t *p = _ctx.gl.lights.data + NU_HANDLE_INDEX(light);
    p->position      = v;
}
static void
nugl__light_set_rotation (nu_light_t light, nu_q4_t q)
{
    nugl__light_t *p = _ctx.gl.lights.data + NU_HANDLE_INDEX(light);
    p->rotation      = q;
}
static void
nugl__light_set_color (nu_light_t light, nu_color_t v)
{
}

static nu_lightenv_t
nugl__lightenv_create (void)
{
    nu__gl_t         *gl = &_ctx.gl;
    nugl__lightenv_t *p  = NU_VEC_PUSH(&gl->lightenvs);
    nu_lightenv_t     handle
        = NU_HANDLE_MAKE(nu_lightenv_t, gl->lightenvs.size - 1);
    p->shadowmap        = NU_NULL;
    p->shadowmap_camera = NU_NULL;
    p->skybox           = NU_NULL;
    p->skybox_rotation  = nu_m3_identity();
    return handle;
}
static void
nugl__lightenv_delete (nu_lightenv_t env)
{
}
static void
nugl__lightenv_add_shadowmap (nu_lightenv_t env,
                              nu_texture_t  shadowmap,
                              nu_camera_t   camera)
{
    nugl__lightenv_t *p = _ctx.gl.lightenvs.data + NU_HANDLE_INDEX(env);
    p->shadowmap        = shadowmap;
    p->shadowmap_camera = camera;
}
void
nugl__lightenv_set_skybox (nu_lightenv_t env,
                           nu_texture_t  cubemap,
                           nu_q4_t       rotation)
{
    nugl__lightenv_t *p = _ctx.gl.lightenvs.data + NU_HANDLE_INDEX(env);
    p->skybox           = cubemap;
    p->skybox_rotation  = nu_q4_mat3(rotation);
}

#endif

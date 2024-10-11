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
}
static void
nugl__light_set_rotation (nu_light_t light, nu_q4_t q)
{
}
static void
nugl__light_set_color (nu_light_t light, nu_color_t v)
{
}

#endif

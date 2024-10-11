#ifndef NUGL_CAMERA_IMPL_H
#define NUGL_CAMERA_IMPL_H

#include <nucleus/internal.h>

static nu_camera_t
nugl__camera_create (void)
{
    nu__gl_t *gl = &_ctx.gl;
    (void)NU_VEC_PUSH(&gl->cameras);
    return NU_HANDLE_MAKE(nu_camera_t, gl->cameras.size - 1);
}
static void
nugl__camera_delete (nu_camera_t camera)
{
}
static void
nugl__camera_set_proj (nu_camera_t camera, nu_m4_t proj)
{
    nu__gl_t       *gl   = &_ctx.gl;
    nugl__camera_t *pcam = gl->cameras.data + NU_HANDLE_INDEX(camera);

    pcam->projection = proj;
    pcam->vp         = nu_m4_mul(proj, pcam->view);
}
static void
nugl__camera_set_view (nu_camera_t camera, nu_m4_t view)
{
    nu__gl_t       *gl   = &_ctx.gl;
    nugl__camera_t *pcam = gl->cameras.data + NU_HANDLE_INDEX(camera);

    pcam->view = view;
    pcam->vp   = nu_m4_mul(pcam->projection, view);
}

#endif

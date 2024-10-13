#ifndef NU_CAMERA_IMPL_H
#define NU_CAMERA_IMPL_H

#include <nucleus/internal.h>

nu_camera_t
nu_camera_create (void)
{
    nu_size_t     index;
    nu__camera_t *camera = NU_POOL_ADD(&_ctx.graphics.cameras, &index);
    nu_camera_t   handle = NU_HANDLE_MAKE(nu_camera_t, index);
    nu_camera_set_view(handle, nu_lookat(NU_V3_UP, NU_V3_FORWARD, NU_V3_ZEROS));
    nu_f32_t aspect
        = (nu_f32_t)_ctx.platform.size.x / (nu_f32_t)_ctx.platform.size.y;
    nu_camera_set_proj(handle,
                       nu_perspective(nu_radian(70), aspect, 0.01, 100));
    return handle;
}
void
nu_camera_delete (nu_camera_t camera)
{
    NU_POOL_REMOVE(&_ctx.graphics.cameras, NU_HANDLE_INDEX(camera));
}
void
nu_camera_set_view (nu_camera_t camera, nu_m4_t view)
{
    nu__camera_t *pcam = _ctx.graphics.cameras.data + NU_HANDLE_INDEX(camera);
    pcam->view         = view;
    pcam->vp           = nu_m4_mul(pcam->projection, view);
}
void
nu_camera_set_proj (nu_camera_t camera, nu_m4_t proj)
{
    nu__camera_t *pcam = _ctx.graphics.cameras.data + NU_HANDLE_INDEX(camera);
    pcam->projection   = proj;
    pcam->vp           = nu_m4_mul(proj, pcam->view);
}

#endif

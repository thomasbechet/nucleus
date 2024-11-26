#ifndef NU_CAMERA_IMPL_H
#define NU_CAMERA_IMPL_H

#include <nucleus/internal.h>

nu_camera_t
nu_camera_new (void)
{
    nu__camera_t *camera = nu_object_new(_ctx.graphics.obj_camera);
    nu_camera_t   handle = (nu_camera_t)camera;
    nu_camera_set_view(handle, nu_lookat(NU_V3_UP, NU_V3_FORWARD, NU_V3_ZEROS));
    nu_f32_t aspect
        = (nu_f32_t)_ctx.platform.size.x / (nu_f32_t)_ctx.platform.size.y;
    nu_camera_set_proj(handle,
                       nu_perspective(nu_radian(70), aspect, 0.01, 100));
    return handle;
}
void
nu_camera_set_view (nu_camera_t camera, nu_m4_t view)
{
    nu__camera_t *cam = (nu__camera_t *)camera;
    cam->view         = view;
    cam->vp           = nu_m4_mul(cam->projection, view);
}
void
nu_camera_set_proj (nu_camera_t camera, nu_m4_t proj)
{
    nu__camera_t *cam = (nu__camera_t *)camera;
    cam->projection   = proj;
    cam->vp           = nu_m4_mul(proj, cam->view);
}

#endif

#ifndef NU_CAMERA_IMPL_H
#define NU_CAMERA_IMPL_H

#include <nucleus/internal.h>

nu_camera_t
nu_camera_create (void)
{
#ifdef NU_BUILD_GL
    nu_camera_t handle = nugl__camera_create();
    NU_CHECK(handle, return handle);
    nu_camera_set_view(handle, nu_lookat(NU_V3_UP, NU_V3_FORWARD, NU_V3_ZEROS));
    nu_f32_t aspect
        = (nu_f32_t)_ctx.platform.size.x / (nu_f32_t)_ctx.platform.size.y;
    nu_camera_set_proj(handle,
                       nu_perspective(nu_radian(70), aspect, 0.01, 100));
    return handle;
#endif
}
void
nu_camera_delete (nu_camera_t camera)
{
#ifdef NU_BUILD_GL
    nugl__camera_delete(camera);
#endif
}
void
nu_camera_set_view (nu_camera_t camera, nu_m4_t view)
{
#ifdef NU_BUILD_GL
    nugl__camera_set_view(camera, view);
#endif
}
void
nu_camera_set_proj (nu_camera_t camera, nu_m4_t proj)
{
#ifdef NU_BUILD_GL
    nugl__camera_set_proj(camera, proj);
#endif
}

#endif

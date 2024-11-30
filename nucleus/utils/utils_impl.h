#ifndef NU_UTILS_IMPL_H
#define NU_UTILS_IMPL_H

#include <nucleus/utils/controller_impl.h>
#include <nucleus/utils/stats_impl.h>

static nu_error_t
nu__utils_init (void)
{
    _ctx.utils.obj_camera_controller = nu_object_type_new(
        NU_STR("controller"), sizeof(nu__camera_controller_t), NU_NULL);
    nu__stats_init();
    return NU_ERROR_NONE;
}

#endif

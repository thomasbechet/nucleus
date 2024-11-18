#ifndef NU_UTILS_IMPL_H
#define NU_UTILS_IMPL_H

#include <nucleus/utils/geometry_impl.h>
#include <nucleus/utils/controller_impl.h>
#include <nucleus/utils/stats_impl.h>

static nu_error_t
nu__utils_init (void)
{
    _ctx.utils.obj_camera_controller
        = nu_object_register(NU_STR("camera_controller"),
                             sizeof(nu__camera_controller_t),
                             nu__camera_controller_handler);
    NU_POOL_INIT(1, &_ctx.utils.geometries);
    nu__stats_init();
    return NU_ERROR_NONE;
}

#endif

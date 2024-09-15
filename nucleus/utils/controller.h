#ifndef NU_CONTROLLER_H
#define NU_CONTROLLER_H

#include <nucleus/utils/api.h>

typedef struct
{
    nu_vec3_t pos;
    nu_vec3_t vel;
    nu_vec3_t acc;
    nu_quat_t rot;
    float     yaw;
    float     pitch;

    nu_controller_mode_t mode;

    float     fov;
    float     speed;
    nu_bool_t on_ground;

    nu_controller_info_t info;
} nu__camera_controller_t;

typedef NU_POOL(nu__camera_controller_t) nu__camera_controller_pool_t;

#endif

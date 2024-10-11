#ifndef NU_CONTROLLER_H
#define NU_CONTROLLER_H

#include <nucleus/utils/api.h>

typedef struct
{
    nu_input_t view_pitch;
    nu_input_t view_yaw;
    nu_input_t view_roll;
    nu_input_t move_x;
    nu_input_t move_y;
    nu_input_t move_z;
    nu_input_t switch_mode;

    nu_v3_t  pos;
    nu_v3_t  vel;
    nu_v3_t  acc;
    nu_q4_t  rot;
    nu_f32_t yaw;
    nu_f32_t pitch;

    nu_controller_mode_t mode;

    nu_f32_t  fov;
    nu_f32_t  speed;
    nu_bool_t on_ground;
} nu__camera_controller_t;

typedef NU_POOL(nu__camera_controller_t) nu__camera_controller_pool_t;

#endif

#ifndef NU_CONTROLLER_H
#define NU_CONTROLLER_H

#include <nucleus/graphics.h>

typedef struct
{
    nu_vec3_t _pos;
    nu_vec3_t _vel;
    nu_vec3_t _acc;
    nu_quat_t _rotation;
    float     _yaw;
    float     _pitch;

    float     fov;
    nu_bool_t free_mode;
    float     speed;
    float     view_pitch_neg;
    float     view_pitch_pos;
    float     view_yaw_neg;
    float     view_yaw_pos;
    float     view_roll_neg;
    float     view_roll_pos;
    float     move_up;
    float     move_down;
    float     move_left;
    float     move_right;
    float     move_forward;
    float     move_backward;
} nu_camera_controller_t;

NU_API void nu_camera_controller_init(nu_camera_controller_t *controller);
NU_API void nu_camera_controller_update(nu_camera_controller_t *controller,
                                        float                   dt,
                                        nu_camera_info_t       *info);

#endif

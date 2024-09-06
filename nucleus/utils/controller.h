#ifndef NU_CONTROLLER_H
#define NU_CONTROLLER_H

#include <nucleus/platform/platform.h>
#include <nucleus/graphics/graphics.h>

typedef struct
{
    nu_input_t view_pitch_neg;
    nu_input_t view_pitch_pos;
    nu_input_t view_yaw_neg;
    nu_input_t view_yaw_pos;
    nu_input_t view_roll_neg;
    nu_input_t view_roll_pos;
    nu_input_t move_up;
    nu_input_t move_down;
    nu_input_t move_left;
    nu_input_t move_right;
    nu_input_t move_forward;
    nu_input_t move_backward;
    nu_input_t switch_mode;
} nu_camera_controller_info_t;

NU_DEFINE_HANDLE(nu_camera_controller_t);

NU_API nu_camera_controller_t
nu_camera_controller_create(const nu_camera_controller_info_t *info);
NU_API void nu_camera_controller_delete(nu_camera_controller_t controller);
NU_API void nu_camera_controller_update(nu_camera_controller_t controller,
                                        float                  dt,
                                        nu_camera_info_t      *info);

#endif

#ifndef NU_CONTROLLER_H
#define NU_CONTROLLER_H

#include <nucleus/platform/platform.h>
#include <nucleus/graphics/graphics.h>

typedef enum
{
    NU_CONTROLLER_FREEFLY_ALIGNED,
    NU_CONTROLLER_FREEFLY,
    NU_CONTROLLER_CHARACTER
} nu_controller_mode_t;

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
} nu_controller_info_t;

NU_DEFINE_HANDLE(nu_controller_t);

NU_API nu_controller_t nu_controller_create(const nu_controller_info_t *info);
NU_API void            nu_controller_delete(nu_controller_t controller);
NU_API void            nu_controller_update(nu_controller_t   controller,
                                            float             dt,
                                            nu_camera_info_t *info);

#endif

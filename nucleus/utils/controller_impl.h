#ifndef NU_CONTROLLER_IMPL_H
#define NU_CONTROLLER_IMPL_H

#include <nucleus/utils/controller.h>

void
nu_camera_controller_init (nu_camera_controller_t *controller)
{
    controller->fov            = 90.0f;
    controller->view_pitch_neg = 0;
    controller->view_pitch_pos = 0;
    controller->view_yaw_neg   = 0;
    controller->view_yaw_pos   = 0;
    controller->move_forward   = 0;
    controller->move_backward  = 0;
    controller->move_left      = 0;
    controller->move_right     = 0;
    controller->move_up        = 0;
    controller->move_down      = 0;
    controller->speed          = 5.0f;
    controller->free_mode      = NU_FALSE;
    controller->_pos           = NU_VEC3_ZERO;
    controller->_vel           = NU_VEC3_ZERO;
    controller->_acc           = NU_VEC3_ZERO;
    controller->_rotation      = nu_quat_identity();
}
void
nu_camera_controller_update (nu_camera_controller_t *controller,
                             float                   dt,
                             nu_camera_info_t       *info)
{
    float look_x
        = NU_CLAMP(controller->view_yaw_pos - controller->view_yaw_neg, -1, 1);
    float look_y = NU_CLAMP(
        controller->view_pitch_pos - controller->view_pitch_neg, -1, 1);
    float look_z = NU_CLAMP(
        controller->view_roll_pos - controller->view_roll_neg, -1, 1);

    float move_x
        = NU_CLAMP(controller->move_right - controller->move_left, -1, 1);
    float move_y = NU_CLAMP(controller->move_up - controller->move_down, -1, 1);
    float move_z
        = NU_CLAMP(controller->move_forward - controller->move_backward, -1, 1);

    // Translation
    nu_vec3_t direction = NU_VEC3_ZERO;

    direction = nu_vec3_add(
        direction,
        nu_vec3_muls(nu_quat_mulv3(controller->_rotation, NU_VEC3_FORWARD),
                     move_z));
    direction = nu_vec3_add(
        direction,
        nu_vec3_muls(nu_quat_mulv3(controller->_rotation, NU_VEC3_RIGHT),
                     move_x));
    if (controller->free_mode)
    {
        direction = nu_vec3_add(
            direction,
            nu_vec3_muls(nu_quat_mulv3(controller->_rotation, NU_VEC3_UP),
                         move_y));
    }
    else
    {
        direction = nu_vec3_add(direction, nu_vec3_muls(NU_VEC3_UP, move_y));
    }

    direction = nu_vec3_normalize(direction);

    // Rotation
    if (controller->free_mode)
    {
        if (look_x != 0)
        {
            controller->_rotation = nu_quat_mul_axis(
                controller->_rotation, NU_VEC3_UP, -nu_radian(look_x) * dt);
        }
        if (look_y != 0)
        {
            controller->_rotation = nu_quat_mul_axis(
                controller->_rotation, NU_VEC3_RIGHT, -nu_radian(look_y) * dt);
        }
        if (look_z != 0)
        {
            controller->_rotation = nu_quat_mul_axis(controller->_rotation,
                                                     NU_VEC3_FORWARD,
                                                     -nu_radian(look_z) * dt);
        }
    }
    else
    {
        if (look_x != 0)
        {
            controller->_yaw += look_x * dt;
        }
        if (look_y != 0)
        {
            controller->_pitch += look_y * dt;
        }
        controller->_pitch = NU_CLAMP(controller->_pitch, -90.0, 90.0);
        controller->_rotation
            = nu_quat_axis(NU_VEC3_UP, -nu_radian(controller->_yaw));
        controller->_rotation = nu_quat_mul(
            controller->_rotation,
            nu_quat_axis(NU_VEC3_RIGHT, -nu_radian(controller->_pitch)));
    }

    dt = dt * 0.001;

    // Compute acceleration
    const float mass  = 10.0;
    nu_vec3_t   force = NU_VEC3_ZERO;
    if (nu_vec3_norm(direction) > 0.001)
    {
        force = nu_vec3_add(force, nu_vec3_muls(direction, 3));
    }
    force = nu_vec3_add(force, nu_vec3_muls(controller->_vel, -0.5));

    // Integrate
    controller->_pos = nu_vec3_add(
        controller->_pos,
        nu_vec3_add(nu_vec3_muls(controller->_vel, dt),
                    nu_vec3_muls(controller->_acc, 0.5 * dt * dt)));
    nu_vec3_t new_acc = nu_vec3_muls(force, mass);
    nu_vec3_t new_vel = nu_vec3_add(
        controller->_vel,
        nu_vec3_muls(nu_vec3_add(controller->_acc, new_acc), 0.5 * dt));
    controller->_acc = new_acc;
    controller->_vel = new_vel;

    nu_vec3_t forward = nu_quat_mulv3(controller->_rotation, NU_VEC3_FORWARD);
    nu_vec3_t up
        = nu_vec3_normalize(nu_quat_mulv3(controller->_rotation, NU_VEC3_UP));
    info->eye    = controller->_pos;
    info->up     = up;
    info->center = nu_vec3_add(controller->_pos, forward);
    info->fov    = controller->fov;
}

#endif

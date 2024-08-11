#ifndef NU_CONTROLLER_IMPL_H
#define NU_CONTROLLER_IMPL_H

#include <nucleus/utils/controller.h>

void
nu_camera_controller_init (nu_camera_controller_t *controller)
{
    controller->_pos       = NU_VEC3_ZERO;
    controller->_vel       = NU_VEC3_ZERO;
    controller->_acc       = NU_VEC3_ZERO;
    controller->_rot       = nu_quat_identity();
    controller->_free_mode = NU_FALSE;
    controller->_pitch     = 0;
    controller->_yaw       = 0;

    controller->fov   = 90;
    controller->speed = 5;
}
void
nu_camera_controller_update (nu_camera_controller_t *controller,
                             const nu_platform_t    *platform,
                             float                   dt,
                             nu_camera_info_t       *info)
{
    nu_vec3_t look = nu_input_axis3d(platform,
                                     controller->view_yaw_neg,
                                     controller->view_yaw_pos,
                                     controller->view_pitch_pos,
                                     controller->view_pitch_neg,
                                     controller->view_roll_pos,
                                     controller->view_roll_neg,
                                     NU_FALSE);
    nu_vec3_t move = nu_input_axis3d(platform,
                                     controller->move_left,
                                     controller->move_right,
                                     controller->move_up,
                                     controller->move_down,
                                     controller->move_forward,
                                     controller->move_backward,
                                     NU_TRUE);

    // Switch mode
    if (nu_input_just_pressed(platform, controller->switch_mode))
    {
        controller->_free_mode = !controller->_free_mode;
    }

    // Translation
    nu_vec3_t direction = NU_VEC3_ZERO;

    direction = nu_vec3_add(
        direction,
        nu_vec3_muls(nu_quat_mulv3(controller->_rot, NU_VEC3_FORWARD), move.z));
    direction = nu_vec3_add(
        direction,
        nu_vec3_muls(nu_quat_mulv3(controller->_rot, NU_VEC3_RIGHT), move.x));
    if (controller->_free_mode)
    {
        direction = nu_vec3_add(
            direction,
            nu_vec3_muls(nu_quat_mulv3(controller->_rot, NU_VEC3_UP), move.y));
    }
    else
    {
        direction = nu_vec3_add(direction, nu_vec3_muls(NU_VEC3_UP, move.y));
    }

    direction = nu_vec3_normalize(direction);

    // Rotation
    if (controller->_free_mode)
    {
        if (look.x != 0)
        {
            controller->_rot = nu_quat_mul_axis(
                controller->_rot, NU_VEC3_UP, -nu_radian(look.x) * dt);
        }
        if (look.y != 0)
        {
            controller->_rot = nu_quat_mul_axis(
                controller->_rot, NU_VEC3_RIGHT, -nu_radian(look.y) * dt);
        }
        if (look.z != 0)
        {
            controller->_rot = nu_quat_mul_axis(
                controller->_rot, NU_VEC3_FORWARD, -nu_radian(look.z) * dt);
        }
    }
    else
    {
        if (look.x != 0)
        {
            controller->_yaw += look.x * dt;
        }
        if (look.y != 0)
        {
            controller->_pitch += look.y * dt;
        }
        controller->_pitch = NU_CLAMP(controller->_pitch, -90.0, 90.0);
        controller->_rot
            = nu_quat_axis(NU_VEC3_UP, -nu_radian(controller->_yaw));
        controller->_rot = nu_quat_mul(
            controller->_rot,
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

    nu_vec3_t forward = nu_quat_mulv3(controller->_rot, NU_VEC3_FORWARD);
    nu_vec3_t up
        = nu_vec3_normalize(nu_quat_mulv3(controller->_rot, NU_VEC3_UP));
    info->eye    = controller->_pos;
    info->up     = up;
    info->center = nu_vec3_add(controller->_pos, forward);
    info->fov    = controller->fov;
}

#endif

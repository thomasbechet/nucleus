#ifndef NU_CONTROLLER_IMPL_H
#define NU_CONTROLLER_IMPL_H

#include <nucleus/internal.h>

nu_camera_controller_t
nu_camera_controller_create (const nu_camera_controller_info_t *info)
{
    nu_camera_controller_t   handle;
    nu__camera_controller_t *ctrl
        = nu_pool_add(&_ctx.utils.controllers, &handle.id);

    ctrl->pos       = NU_VEC3_ZERO;
    ctrl->vel       = NU_VEC3_ZERO;
    ctrl->acc       = NU_VEC3_ZERO;
    ctrl->rot       = nu_quat_identity();
    ctrl->free_mode = NU_FALSE;
    ctrl->pitch     = 0;
    ctrl->yaw       = 0;

    ctrl->fov   = 90;
    ctrl->speed = 5;

    ctrl->info = *info;

    return handle;
}
void
nu_camera_controller_update (nu_camera_controller_t controller,
                             float                  dt,
                             nu_camera_info_t      *info)
{
    nu__camera_controller_t *ctrl = &_ctx.utils.controllers.data[controller.id];
    nu_vec3_t                look = nu_input_axis3d(ctrl->info.view_yaw_neg,
                                     ctrl->info.view_yaw_pos,
                                     ctrl->info.view_pitch_pos,
                                     ctrl->info.view_pitch_neg,
                                     ctrl->info.view_roll_pos,
                                     ctrl->info.view_roll_neg,
                                     NU_FALSE);
    nu_vec3_t                move = nu_input_axis3d(ctrl->info.move_left,
                                     ctrl->info.move_right,
                                     ctrl->info.move_up,
                                     ctrl->info.move_down,
                                     ctrl->info.move_forward,
                                     ctrl->info.move_backward,
                                     NU_TRUE);

    // Switch mode
    if (nu_input_just_pressed(ctrl->info.switch_mode))
    {
        ctrl->free_mode = !ctrl->free_mode;
    }

    // Translation
    nu_vec3_t direction = NU_VEC3_ZERO;

    direction = nu_vec3_add(
        direction,
        nu_vec3_muls(nu_quat_mulv3(ctrl->rot, NU_VEC3_FORWARD), move.z));
    direction = nu_vec3_add(
        direction,
        nu_vec3_muls(nu_quat_mulv3(ctrl->rot, NU_VEC3_RIGHT), move.x));
    if (ctrl->free_mode)
    {
        direction = nu_vec3_add(
            direction,
            nu_vec3_muls(nu_quat_mulv3(ctrl->rot, NU_VEC3_UP), move.y));
    }
    else
    {
        direction = nu_vec3_add(direction, nu_vec3_muls(NU_VEC3_UP, move.y));
    }

    direction = nu_vec3_normalize(direction);

    // Rotation
    if (ctrl->free_mode)
    {
        if (look.x != 0)
        {
            ctrl->rot = nu_quat_mul_axis(
                ctrl->rot, NU_VEC3_UP, -nu_radian(look.x) * dt);
        }
        if (look.y != 0)
        {
            ctrl->rot = nu_quat_mul_axis(
                ctrl->rot, NU_VEC3_RIGHT, -nu_radian(look.y) * dt);
        }
        if (look.z != 0)
        {
            ctrl->rot = nu_quat_mul_axis(
                ctrl->rot, NU_VEC3_FORWARD, -nu_radian(look.z) * dt);
        }
    }
    else
    {
        if (look.x != 0)
        {
            ctrl->yaw += look.x * dt;
        }
        if (look.y != 0)
        {
            ctrl->pitch += look.y * dt;
        }
        ctrl->pitch = NU_CLAMP(ctrl->pitch, -90.0, 90.0);
        ctrl->rot   = nu_quat_axis(NU_VEC3_UP, -nu_radian(ctrl->yaw));
        ctrl->rot   = nu_quat_mul(
            ctrl->rot, nu_quat_axis(NU_VEC3_RIGHT, -nu_radian(ctrl->pitch)));
    }

    dt = dt * 0.001;

    // Compute acceleration
    const float mass  = 10.0;
    nu_vec3_t   force = NU_VEC3_ZERO;
    if (nu_vec3_norm(direction) > 0.001)
    {
        force = nu_vec3_add(force, nu_vec3_muls(direction, 3));
    }
    force = nu_vec3_add(force, nu_vec3_muls(ctrl->vel, -0.5));

    // Integrate
    ctrl->pos
        = nu_vec3_add(ctrl->pos,
                      nu_vec3_add(nu_vec3_muls(ctrl->vel, dt),
                                  nu_vec3_muls(ctrl->acc, 0.5 * dt * dt)));
    nu_vec3_t new_acc = nu_vec3_muls(force, mass);
    nu_vec3_t newvel  = nu_vec3_add(
        ctrl->vel, nu_vec3_muls(nu_vec3_add(ctrl->acc, new_acc), 0.5 * dt));
    ctrl->acc = new_acc;
    ctrl->vel = newvel;

    nu_vec3_t forward = nu_quat_mulv3(ctrl->rot, NU_VEC3_FORWARD);
    nu_vec3_t up      = nu_vec3_normalize(nu_quat_mulv3(ctrl->rot, NU_VEC3_UP));
    info->eye         = ctrl->pos;
    info->up          = up;
    info->center      = nu_vec3_add(ctrl->pos, forward);
    info->fov         = ctrl->fov;
}

#endif

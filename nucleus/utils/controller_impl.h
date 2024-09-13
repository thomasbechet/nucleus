#ifndef NU_CONTROLLER_IMPL_H
#define NU_CONTROLLER_IMPL_H

#include <nucleus/internal.h>

nu_controller_t
nu_controller_create (const nu_controller_info_t *info)
{
    nu_size_t                index;
    nu__camera_controller_t *ctrl
        = nu_pool_add(&_ctx.utils.controllers, &index);

    ctrl->pos   = NU_VEC3_ZERO;
    ctrl->vel   = NU_VEC3_ZERO;
    ctrl->acc   = NU_VEC3_ZERO;
    ctrl->rot   = nu_quat_identity();
    ctrl->pitch = 0;
    ctrl->yaw   = 0;

    ctrl->mode = NU_CONTROLLER_FREEFLY_ALIGNED;

    ctrl->fov       = 90;
    ctrl->speed     = 10;
    ctrl->on_ground = NU_FALSE;

    ctrl->info = *info;

    return nu_handle_make(nu_controller_t, index);
}
void
nu_controller_update (nu_controller_t controller, float dt, nu_camera_t camera)
{
    nu__camera_controller_t *ctrl
        = &_ctx.utils.controllers.data[nu_handle_index(controller)];
    nu_vec3_t look = nu_input_axis3d(ctrl->info.view_yaw_neg,
                                     ctrl->info.view_yaw_pos,
                                     ctrl->info.view_pitch_pos,
                                     ctrl->info.view_pitch_neg,
                                     ctrl->info.view_roll_pos,
                                     ctrl->info.view_roll_neg,
                                     NU_FALSE);
    nu_vec3_t move = nu_input_axis3d(ctrl->info.move_left,
                                     ctrl->info.move_right,
                                     ctrl->info.move_up,
                                     ctrl->info.move_down,
                                     ctrl->info.move_forward,
                                     ctrl->info.move_backward,
                                     NU_TRUE);

    // Switch mode
    if (nu_input_just_pressed(ctrl->info.switch_mode))
    {
        switch (ctrl->mode)
        {
            case NU_CONTROLLER_FREEFLY_ALIGNED:
                ctrl->mode = NU_CONTROLLER_FREEFLY;
                break;
            case NU_CONTROLLER_FREEFLY:
                ctrl->mode = NU_CONTROLLER_CHARACTER;
                break;
            case NU_CONTROLLER_CHARACTER:
                ctrl->mode = NU_CONTROLLER_FREEFLY_ALIGNED;
                break;
        }
    }

    // Translation
    nu_vec3_t direction = NU_VEC3_ZERO;

    direction = nu_vec3_add(
        direction,
        nu_vec3_muls(nu_quat_mulv3(ctrl->rot, NU_VEC3_FORWARD), move.z));
    direction = nu_vec3_add(
        direction,
        nu_vec3_muls(nu_quat_mulv3(ctrl->rot, NU_VEC3_RIGHT), move.x));
    if (ctrl->mode == NU_CONTROLLER_FREEFLY)
    {
        direction = nu_vec3_add(
            direction,
            nu_vec3_muls(nu_quat_mulv3(ctrl->rot, NU_VEC3_UP), move.y));
    }
    else if (ctrl->mode == NU_CONTROLLER_FREEFLY_ALIGNED)
    {
        direction = nu_vec3_add(direction, nu_vec3_muls(NU_VEC3_UP, move.y));
    }
    else
    {
        // No vertical movement in character mode
    }

    direction = nu_vec3_normalize(direction);

    // Rotation
    if (ctrl->mode == NU_CONTROLLER_FREEFLY)
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
        ctrl->pitch = nu_clamp(ctrl->pitch, -90.0, 90.0);
        ctrl->rot   = nu_quat_axis(NU_VEC3_UP, -nu_radian(ctrl->yaw));
        ctrl->rot   = nu_quat_mul(
            ctrl->rot, nu_quat_axis(NU_VEC3_RIGHT, -nu_radian(ctrl->pitch)));
    }

    dt = dt * 0.001;

    // Compute sum of forces
    const float mass  = 10.0;
    nu_vec3_t   force = NU_VEC3_ZERO;

    // Apply movement
    if (nu_vec3_norm(direction) > 0.001)
    {
        force = nu_vec3_add(force, nu_vec3_muls(direction, 6));
    }

    // Apply gravity
    if (ctrl->mode == NU_CONTROLLER_CHARACTER)
    {
        force = nu_vec3_add(force, nu_vec3_muls(NU_VEC3_DOWN, 5));
    }

    // Apply jump
    if (ctrl->mode == NU_CONTROLLER_CHARACTER)
    {
        if (ctrl->on_ground && nu_input_just_pressed(ctrl->info.move_up))
        {
            ctrl->on_ground = NU_FALSE;
            ctrl->vel       = nu_vec3_muls(NU_VEC3_UP, 15);
        }
    }

    // Apply drag
    if (ctrl->mode == NU_CONTROLLER_CHARACTER)
    {
        // Allow infinite vertical fall
        force = nu_vec3_add(
            force, nu_vec3_muls(nu_vec3(ctrl->vel.x, 0, ctrl->vel.z), -0.5));
    }
    else
    {
        force = nu_vec3_add(force, nu_vec3_muls(ctrl->vel, -0.5));
    }

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

    // Collision detection and resolution
    if (ctrl->mode == NU_CONTROLLER_CHARACTER)
    {
        const float height = 1.75;
        if (ctrl->pos.y - height <= 0)
        {
            ctrl->pos.y     = height;
            ctrl->on_ground = NU_TRUE;
        }
    }

    nu_vec3_t forward = nu_quat_mulv3(ctrl->rot, NU_VEC3_FORWARD);
    nu_vec3_t up      = nu_vec3_normalize(nu_quat_mulv3(ctrl->rot, NU_VEC3_UP));

    nu_camera_perspective(camera, ctrl->fov, 0.01, 100);
    nu_camera_view(camera, up, nu_vec3_add(ctrl->pos, forward), ctrl->pos);
}

#endif

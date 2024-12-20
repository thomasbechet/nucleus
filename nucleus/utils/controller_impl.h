#ifndef NU_CONTROLLER_IMPL_H
#define NU_CONTROLLER_IMPL_H

#include <nucleus/internal.h>

nu_controller_t
nu_controller_new (nu_input_t view_pitch,
                   nu_input_t view_yaw,
                   nu_input_t view_roll,
                   nu_input_t move_x,
                   nu_input_t move_y,
                   nu_input_t move_z,
                   nu_input_t switch_mode)
{
    nu__camera_controller_t *ctrl
        = nu_object_new(_ctx.utils.obj_camera_controller);

    ctrl->view_pitch  = view_pitch;
    ctrl->view_yaw    = view_yaw;
    ctrl->view_roll   = view_roll;
    ctrl->move_x      = move_x;
    ctrl->move_y      = move_y;
    ctrl->move_z      = move_z;
    ctrl->switch_mode = switch_mode;

    ctrl->pos   = NU_V3_ZEROS;
    ctrl->vel   = NU_V3_ZEROS;
    ctrl->acc   = NU_V3_ZEROS;
    ctrl->rot   = nu_q4_identity();
    ctrl->pitch = 0;
    ctrl->yaw   = 0;

    ctrl->mode = NU_CONTROLLER_FREEFLY_ALIGNED;

    ctrl->fov       = 90;
    ctrl->speed     = 20;
    ctrl->on_ground = NU_FALSE;

    return (nu_controller_t)ctrl;
}
void
nu_controller_update (nu_controller_t controller,
                      nu_f32_t        dt,
                      nu_camera_t     camera)
{
    nu__camera_controller_t *ctrl = (nu__camera_controller_t *)controller;
    nu_v3_t                  look = nu_v3(nu_input_value(ctrl->view_yaw),
                         nu_input_value(ctrl->view_pitch),
                         nu_input_value(ctrl->view_roll));
    nu_v3_t move = nu_v3_normalize(nu_v3(nu_input_value(ctrl->move_x),
                                         nu_input_value(ctrl->move_y),
                                         nu_input_value(ctrl->move_z)));

    // Switch mode
    if (nu_input_just_pressed(ctrl->switch_mode))
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
    nu_v3_t direction = NU_V3_ZEROS;

    direction = nu_v3_add(
        direction, nu_v3_muls(nu_q4_mulv3(ctrl->rot, NU_V3_FORWARD), move.z));
    direction = nu_v3_add(
        direction, nu_v3_muls(nu_q4_mulv3(ctrl->rot, NU_V3_RIGHT), move.x));
    if (ctrl->mode == NU_CONTROLLER_FREEFLY)
    {
        direction = nu_v3_add(
            direction, nu_v3_muls(nu_q4_mulv3(ctrl->rot, NU_V3_UP), move.y));
    }
    else if (ctrl->mode == NU_CONTROLLER_FREEFLY_ALIGNED)
    {
        direction = nu_v3_add(direction, nu_v3_muls(NU_V3_UP, move.y));
    }
    else
    {
        // No vertical movement in character mode
    }

    direction = nu_v3_normalize(direction);

    // Rotation
    if (ctrl->mode == NU_CONTROLLER_FREEFLY)
    {
        if (look.x != 0)
        {
            ctrl->rot
                = nu_q4_mul_axis(ctrl->rot, NU_V3_UP, -nu_radian(look.x) * dt);
        }
        if (look.y != 0)
        {
            ctrl->rot = nu_q4_mul_axis(
                ctrl->rot, NU_V3_RIGHT, -nu_radian(look.y) * dt);
        }
        if (look.z != 0)
        {
            ctrl->rot = nu_q4_mul_axis(
                ctrl->rot, NU_V3_FORWARD, -nu_radian(look.z) * dt);
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
        ctrl->rot   = nu_q4_axis(NU_V3_UP, -nu_radian(ctrl->yaw));
        ctrl->rot   = nu_q4_mul(ctrl->rot,
                              nu_q4_axis(NU_V3_RIGHT, -nu_radian(ctrl->pitch)));
    }

    dt = dt * 0.001;

    // Compute sum of forces
    const nu_f32_t mass  = 10.0;
    nu_v3_t        force = NU_V3_ZEROS;

    // Apply movement
    if (nu_v3_norm(direction) > 0.001)
    {
        force = nu_v3_add(force, nu_v3_muls(direction, 8));
    }

    // Apply gravity
    if (ctrl->mode == NU_CONTROLLER_CHARACTER)
    {
        force = nu_v3_add(force, nu_v3_muls(NU_V3_DOWN, 5));
    }

    // Apply jump
    if (ctrl->mode == NU_CONTROLLER_CHARACTER)
    {
        if (ctrl->on_ground && nu_input_value(ctrl->move_y) > 0)
        {
            ctrl->on_ground = NU_FALSE;
            ctrl->vel       = nu_v3_muls(NU_V3_UP, 15);
        }
    }

    // Apply drag
    if (ctrl->mode == NU_CONTROLLER_CHARACTER)
    {
        // Allow infinite vertical fall
        force = nu_v3_add(force,
                          nu_v3_muls(nu_v3(ctrl->vel.x, 0, ctrl->vel.z), -0.5));
    }
    else
    {
        force = nu_v3_add(force, nu_v3_muls(ctrl->vel, -0.5));
    }

    // Integrate
    ctrl->pos       = nu_v3_add(ctrl->pos,
                          nu_v3_add(nu_v3_muls(ctrl->vel, dt),
                                    nu_v3_muls(ctrl->acc, 0.5 * dt * dt)));
    nu_v3_t new_acc = nu_v3_muls(force, mass);
    nu_v3_t newvel  = nu_v3_add(
        ctrl->vel, nu_v3_muls(nu_v3_add(ctrl->acc, new_acc), 0.5 * dt));
    ctrl->acc = new_acc;
    ctrl->vel = newvel;

    // Collision detection and resolution
    if (ctrl->mode == NU_CONTROLLER_CHARACTER)
    {
        const nu_f32_t height = 1.75;
        if (ctrl->pos.y - height <= 0)
        {
            ctrl->pos.y     = height;
            ctrl->on_ground = NU_TRUE;
        }
    }

    nu_v3_t pos     = ctrl->pos;
    nu_v3_t forward = nu_q4_mulv3(ctrl->rot, NU_V3_FORWARD);
    nu_v3_t up      = nu_v3_normalize(nu_q4_mulv3(ctrl->rot, NU_V3_UP));

    nu_camera_set_view(camera, nu_lookat(pos, nu_v3_add(pos, forward), up));
}
nu_m4_t
nu_controller_transform (nu_controller_t controller)
{
    nu__camera_controller_t *ctrl = (nu__camera_controller_t *)controller;
    return nu_m4_mul(nu_m4_translate(ctrl->pos), nu_q4_mat4(ctrl->rot));
}

#endif

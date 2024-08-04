#ifndef NU_RENDERER_H
#define NU_RENDERER_H

#include <nucleus/types.h>
#include <nucleus/math.h>

NU_API nu_error_t nu_create_camera(nu_context_t *ctx, nu_camera_t *camera);
NU_API nu_error_t nu_delete_camera(nu_context_t *ctx, nu_camera_t *camera);
NU_API nu_error_t nu_update_camera(nu_context_t *ctx, nu_camera_t *camera);
NU_API nu_error_t nu_create_mesh(nu_context_t         *ctx,
                                 const nu_mesh_info_t *info,
                                 nu_mesh_t            *mesh);
NU_API nu_error_t nu_delete_mesh(nu_context_t *ctx, nu_mesh_t *mesh);
NU_API nu_error_t nu_create_renderpass(nu_context_t               *ctx,
                                       const nu_renderpass_info_t *info,
                                       nu_renderpass_t            *pass);
NU_API nu_error_t nu_delete_renderpass(nu_context_t    *ctx,
                                       nu_renderpass_t *pass);

NU_API void nu_submit_renderpass(nu_context_t                 *ctx,
                                 nu_renderpass_t              *pass,
                                 const nu_renderpass_submit_t *info);
NU_API void nu_draw(nu_context_t    *ctx,
                    nu_renderpass_t *renderpass,
                    const nu_mesh_t *mesh,
                    const nu_mat4_t *transform);
NU_API void nu_draw_instanced(nu_renderpass_t *renderpass,
                              const float     *transforms,
                              nu_u32_t         count);

NU_API void nu_init_camera_controller(nu_camera_controller_t *controller);
NU_API void nu_update_camera_controller(nu_camera_controller_t *controller,
                                        float                   dt,
                                        nu_camera_t            *camera);

#ifdef NU_IMPLEMENTATION

#ifdef NU_BUILD_RENDERER_GL
#include <nucleus/backend/gl.h>
#endif

static nu_error_t
nu__renderer_null_init (void *ctx, nu_allocator_t allocator, nu_uvec2_t size)
{
    return NU_ERROR_NONE;
}
static nu_error_t
nu__renderer_null_render (void            *ctx,
                          const nu_rect_t *global_viewport,
                          const nu_rect_t *viewport)
{
    NU_UNUSED(ctx);
    NU_UNUSED(viewport);
    return NU_ERROR_NONE;
}
static nu_error_t
nu__renderer_null_create_camera (void *ctx, nu_camera_t *camera)
{
    return NU_ERROR_NONE;
}
static nu_error_t
nu__renderer_null_delete_camera (void *ctx, nu_camera_t *camera)
{
    return NU_ERROR_NONE;
}
static nu_error_t
nu__renderer_null_update_camera (void *ctx, nu_camera_t *camera)
{
    return NU_ERROR_NONE;
}
static nu_error_t
nu__renderer_null_create_mesh (void                 *ctx,
                               const nu_mesh_info_t *info,
                               nu_mesh_t            *mesh)
{
    return NU_ERROR_NONE;
}
static nu_error_t
nu__renderer_null_delete_mesh (void *ctx, nu_mesh_t *mesh)
{
    return NU_ERROR_NONE;
}
static nu_error_t
nu__renderer_null_create_renderpass (void                       *ctx,
                                     const nu_renderpass_info_t *info,
                                     nu_renderpass_t            *pass)
{
    return NU_ERROR_NONE;
}
static nu_error_t
nu__renderer_null_delete_renderpass (void *ctx, nu_renderpass_t *pass)
{
    return NU_ERROR_NONE;
}

static void
nu__renderer_null_draw (void            *ctx,
                        nu_renderpass_t *pass,
                        const nu_mesh_t *mesh,
                        const nu_mat4_t *transform)
{
}
static void
nu__renderer_null_submit_renderpass (void                         *ctx,
                                     nu_renderpass_t              *pass,
                                     const nu_renderpass_submit_t *info)
{
}

static nu_error_t
nu__init_renderer (nu_context_t *ctx)
{
    switch (ctx->_renderer_backend)
    {
        case NU_RENDERER_NULL:
            ctx->_renderer.api.init   = nu__renderer_null_init;
            ctx->_renderer.api.render = nu__renderer_null_render;

            ctx->_renderer.api.create_camera = nu__renderer_null_create_camera;
            ctx->_renderer.api.delete_camera = nu__renderer_null_delete_camera;
            ctx->_renderer.api.update_camera = nu__renderer_null_update_camera;
            ctx->_renderer.api.create_mesh   = nu__renderer_null_create_mesh;
            ctx->_renderer.api.delete_mesh   = nu__renderer_null_delete_mesh;
            ctx->_renderer.api.create_renderpass
                = nu__renderer_null_create_renderpass;
            ctx->_renderer.api.delete_renderpass
                = nu__renderer_null_delete_renderpass;

            ctx->_renderer.api.draw = nu__renderer_null_draw;
            ctx->_renderer.api.submit_renderpass
                = nu__renderer_null_submit_renderpass;

            ctx->_renderer.ctx = NU_NULL;
            break;
        case NU_RENDERER_GL:
            ctx->_renderer.api.init   = nugl__init;
            ctx->_renderer.api.render = nugl__render;

            ctx->_renderer.api.create_camera     = nugl__create_camera;
            ctx->_renderer.api.delete_camera     = nugl__delete_camera;
            ctx->_renderer.api.update_camera     = nugl__update_camera;
            ctx->_renderer.api.create_mesh       = nugl__create_mesh;
            ctx->_renderer.api.delete_mesh       = nugl__delete_mesh;
            ctx->_renderer.api.create_renderpass = nugl__create_renderpass;
            ctx->_renderer.api.delete_renderpass = nugl__delete_renderpass;

            ctx->_renderer.api.draw              = nugl__draw;
            ctx->_renderer.api.submit_renderpass = nugl__submit_renderpass;

            ctx->_renderer.ctx = &ctx->_renderer.backend.gl;
            break;
        case NU_RENDERER_DX11:
            break;
        case NU_RENDERER_SOFTRAST:
            break;
    }

    // Initialize backend
    nu_error_t error = ctx->_renderer.api.init(
        ctx->_renderer.ctx, ctx->_allocator, ctx->_surface_size);
    NU_ERROR_CHECK(error, return error);

    return NU_ERROR_NONE;
}
static nu_error_t
nu__terminate_renderer (nu_context_t *ctx)
{
    return NU_ERROR_NONE;
}

nu_error_t
nu_create_camera (nu_context_t *ctx, nu_camera_t *camera)
{
    camera->projection = NU_CAMERA_DEFAULT_PROJECTION;
    camera->fov        = NU_CAMERA_DEFAULT_FOV;
    camera->near       = NU_CAMERA_DEFAULT_NEAR;
    camera->far        = NU_CAMERA_DEFAULT_FAR;
    camera->eye        = NU_CAMERA_DEFAULT_EYE;
    camera->center     = NU_CAMERA_DEFAULT_CENTER;
    camera->up         = NU_CAMERA_DEFAULT_UP;
    return ctx->_renderer.api.create_camera(ctx->_renderer.ctx, camera);
}
nu_error_t
nu_delete_camera (nu_context_t *ctx, nu_camera_t *camera)
{
    return ctx->_renderer.api.delete_camera(ctx->_renderer.ctx, camera);
}
nu_error_t
nu_update_camera (nu_context_t *ctx, nu_camera_t *camera)
{
    return ctx->_renderer.api.update_camera(ctx->_renderer.ctx, camera);
}
nu_error_t
nu_create_mesh (nu_context_t *ctx, const nu_mesh_info_t *info, nu_mesh_t *mesh)
{
    return ctx->_renderer.api.create_mesh(ctx->_renderer.ctx, info, mesh);
}
nu_error_t
nu_delete_mesh (nu_context_t *ctx, nu_mesh_t *mesh)
{
    return ctx->_renderer.api.delete_mesh(ctx->_renderer.ctx, mesh);
}
nu_error_t
nu_create_renderpass (nu_context_t               *ctx,
                      const nu_renderpass_info_t *info,
                      nu_renderpass_t            *pass)
{
    return ctx->_renderer.api.create_renderpass(ctx->_renderer.ctx, info, pass);
}
nu_error_t
nu_delete_renderpass (nu_context_t *ctx, nu_renderpass_t *pass)
{
    return ctx->_renderer.api.delete_renderpass(ctx->_renderer.ctx, pass);
}

void
nu_submit_renderpass (nu_context_t                 *ctx,
                      nu_renderpass_t              *pass,
                      const nu_renderpass_submit_t *info)
{
    ctx->_renderer.api.submit_renderpass(ctx->_renderer.ctx, pass, info);
}
void
nu_draw (nu_context_t    *ctx,
         nu_renderpass_t *renderpass,
         const nu_mesh_t *mesh,
         const nu_mat4_t *transform)
{
    ctx->_renderer.api.draw(ctx->_renderer.ctx, renderpass, mesh, transform);
}

void
nu_init_camera_controller (nu_camera_controller_t *controller)
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
    controller->rotation       = nu_quat_identity();
}
void
nu_update_camera_controller (nu_camera_controller_t *controller,
                             float                   dt,
                             nu_camera_t            *camera)
{
    // Translation
    nu_vec3_t direction = NU_VEC3_ZERO;

    direction = nu_vec3_add(
        direction,
        nu_vec3_muls(nu_quat_mulv3(controller->rotation, NU_VEC3_FORWARD),
                     NU_CLAMP01(controller->move_forward)));
    direction = nu_vec3_add(
        direction,
        nu_vec3_muls(nu_quat_mulv3(controller->rotation, NU_VEC3_BACKWARD),
                     NU_CLAMP01(controller->move_backward)));
    direction = nu_vec3_add(
        direction,
        nu_vec3_muls(nu_quat_mulv3(controller->rotation, NU_VEC3_LEFT),
                     NU_CLAMP01(controller->move_left)));
    direction = nu_vec3_add(
        direction,
        nu_vec3_muls(nu_quat_mulv3(controller->rotation, NU_VEC3_RIGHT),
                     NU_CLAMP01(controller->move_right)));
    direction = nu_vec3_add(
        direction,
        nu_vec3_muls(nu_quat_mulv3(controller->rotation, NU_VEC3_UP),
                     NU_CLAMP01(controller->move_up)));
    direction = nu_vec3_add(
        direction,
        nu_vec3_muls(nu_quat_mulv3(controller->rotation, NU_VEC3_DOWN),
                     NU_CLAMP01(controller->move_down)));

    direction = nu_vec3_muls(nu_vec3_normalize(direction),
                             dt * 0.001f * controller->speed);

    // Rotation
    if (controller->view_pitch_neg > 0)
    {
        controller->rotation
            = nu_quat_mul_axis(controller->rotation,
                               NU_VEC3_RIGHT,
                               nu_radian(controller->view_pitch_neg) * dt);
    }
    if (controller->view_pitch_pos > 0)
    {
        controller->rotation
            = nu_quat_mul_axis(controller->rotation,
                               NU_VEC3_RIGHT,
                               -nu_radian(controller->view_pitch_pos) * dt);
    }
    if (controller->view_yaw_neg > 0)
    {
        controller->rotation
            = nu_quat_mul_axis(controller->rotation,
                               NU_VEC3_UP,
                               nu_radian(controller->view_yaw_neg) * dt);
    }
    if (controller->view_yaw_pos > 0)
    {
        controller->rotation
            = nu_quat_mul_axis(controller->rotation,
                               NU_VEC3_UP,
                               -nu_radian(controller->view_yaw_pos) * dt);
    }
    if (controller->view_roll_neg > 0)
    {
        controller->rotation
            = nu_quat_mul_axis(controller->rotation,
                               NU_VEC3_FORWARD,
                               nu_radian(controller->view_roll_neg) * dt);
    }
    if (controller->view_roll_pos > 0)
    {
        controller->rotation
            = nu_quat_mul_axis(controller->rotation,
                               NU_VEC3_FORWARD,
                               -nu_radian(controller->view_roll_pos) * dt);
    }

    nu_vec3_t position = nu_vec3_add(camera->eye, direction);
    nu_vec3_t forward  = nu_quat_mulv3(controller->rotation, NU_VEC3_FORWARD);
    nu_vec3_t up
        = nu_vec3_normalize(nu_quat_mulv3(controller->rotation, NU_VEC3_UP));
    camera->eye    = position;
    camera->up     = up;
    camera->center = nu_vec3_add(position, forward);
}

#endif

#endif

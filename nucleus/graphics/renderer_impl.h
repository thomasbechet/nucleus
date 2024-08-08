#ifndef NU_RENDERER_IMPL_H
#define NU_RENDERER_IMPL_H

#include <nucleus/graphics/renderer.h>

#ifdef NU_BUILD_RENDERER_GL
#include <nucleus/graphics/gl_impl.h>
#endif

static nu_error_t
nu__renderer_null_init (nu_renderer_t  *ctx,
                        nu_allocator_t *allocator,
                        nu_uvec2_t      size)
{
    return NU_ERROR_NONE;
}
static nu_error_t
nu__renderer_null_render (nu_renderer_t   *ctx,
                          const nu_rect_t *global_viewport,
                          const nu_rect_t *viewport)
{
    return NU_ERROR_NONE;
}
static nu_texture_t
nu__renderer_null_create_surface_color (nu_renderer_t *ctx)
{
    nu_texture_t tex = { 0 };
    return tex;
}
static nu_error_t
nu__renderer_null_create_camera (nu_renderer_t          *ctx,
                                 const nu_camera_info_t *info,
                                 nu_camera_t            *camera)
{
    return NU_ERROR_NONE;
}
static nu_error_t
nu__renderer_null_delete_camera (nu_renderer_t *ctx, nu_camera_t *camera)
{
    return NU_ERROR_NONE;
}
static nu_error_t
nu__renderer_null_update_camera (nu_renderer_t          *ctx,
                                 nu_camera_t            *camera,
                                 const nu_camera_info_t *info)
{
    return NU_ERROR_NONE;
}
static nu_error_t
nu__renderer_null_create_mesh (nu_renderer_t        *ctx,
                               const nu_mesh_info_t *info,
                               nu_mesh_t            *mesh)
{
    return NU_ERROR_NONE;
}
static nu_error_t
nu__renderer_null_delete_mesh (nu_renderer_t *ctx, nu_mesh_t *mesh)
{
    return NU_ERROR_NONE;
}
static nu_error_t
nu__renderer_null_create_texture (nu_renderer_t           *ctx,
                                  const nu_texture_info_t *info,
                                  nu_texture_t            *texture)
{
    return NU_ERROR_NONE;
}
static nu_error_t
nu__renderer_null_delete_texture (nu_renderer_t *ctx, nu_texture_t *texture)
{
    return NU_ERROR_NONE;
}
static nu_error_t
nu__renderer_null_write_texture (nu_renderer_t    *ctx,
                                 nu_texture_t     *texture,
                                 nu_rect_t         rect,
                                 const nu_color_t *data)
{
    return NU_ERROR_NONE;
}
static nu_error_t
nu__renderer_null_create_material (nu_renderer_t            *ctx,
                                   const nu_material_info_t *info,
                                   nu_material_t            *material)
{
    return NU_ERROR_NONE;
}
static nu_error_t
nu__renderer_null_delete_material (nu_renderer_t *ctx, nu_material_t *material)
{
    return NU_ERROR_NONE;
}
static nu_error_t
nu__renderer_null_update_material (nu_renderer_t            *ctx,
                                   nu_material_t            *material,
                                   const nu_material_info_t *info)
{
    return NU_ERROR_NONE;
}
static nu_error_t
nu__renderer_null_create_renderpass (nu_renderer_t              *ctx,
                                     const nu_renderpass_info_t *info,
                                     nu_renderpass_t            *pass)
{
    return NU_ERROR_NONE;
}
static nu_error_t
nu__renderer_null_delete_renderpass (nu_renderer_t *ctx, nu_renderpass_t *pass)
{
    return NU_ERROR_NONE;
}

static void
nu__renderer_null_draw (nu_renderer_t       *ctx,
                        nu_renderpass_t     *pass,
                        const nu_mesh_t     *mesh,
                        const nu_material_t *material,
                        const nu_mat4_t     *transform)
{
}
static void
nu__renderer_null_submit_renderpass (nu_renderer_t                *ctx,
                                     nu_renderpass_t              *pass,
                                     const nu_renderpass_submit_t *info)
{
}

nu_renderer_info_t
nu_renderer_info_default (void)
{
    nu_renderer_info_t info;
    info.logger = nu_logger_info_default();
    info.api    = NU_RENDERER_NULL;
    return info;
}

nu_error_t
nu_renderer_init (nu_platform_t            *platform,
                  const nu_renderer_info_t *info,
                  nu_renderer_t            *renderer)
{
    renderer->_allocator = info->allocator;

    switch (info->api)
    {
        case NU_RENDERER_NULL:
            renderer->_api.init   = nu__renderer_null_init;
            renderer->_api.render = nu__renderer_null_render;
            renderer->_api.create_surface_color
                = nu__renderer_null_create_surface_color;

            renderer->_api.create_camera   = nu__renderer_null_create_camera;
            renderer->_api.delete_camera   = nu__renderer_null_delete_camera;
            renderer->_api.update_camera   = nu__renderer_null_update_camera;
            renderer->_api.create_mesh     = nu__renderer_null_create_mesh;
            renderer->_api.delete_mesh     = nu__renderer_null_delete_mesh;
            renderer->_api.create_texture  = nu__renderer_null_create_texture;
            renderer->_api.delete_texture  = nu__renderer_null_delete_texture;
            renderer->_api.write_texture   = nu__renderer_null_write_texture;
            renderer->_api.create_material = nu__renderer_null_create_material;
            renderer->_api.delete_material = nu__renderer_null_delete_material;
            renderer->_api.update_material = nu__renderer_null_update_material;
            renderer->_api.create_renderpass
                = nu__renderer_null_create_renderpass;
            renderer->_api.delete_renderpass
                = nu__renderer_null_delete_renderpass;

            renderer->_api.draw = nu__renderer_null_draw;
            renderer->_api.submit_renderpass
                = nu__renderer_null_submit_renderpass;
            break;
        case NU_RENDERER_GL:
            renderer->_api.init                 = nugl__init;
            renderer->_api.render               = nugl__render;
            renderer->_api.create_surface_color = nugl__create_surface_color;

            renderer->_api.create_camera     = nugl__create_camera;
            renderer->_api.delete_camera     = nugl__delete_camera;
            renderer->_api.update_camera     = nugl__update_camera;
            renderer->_api.create_mesh       = nugl__create_mesh;
            renderer->_api.delete_mesh       = nugl__delete_mesh;
            renderer->_api.create_texture    = nugl__create_texture;
            renderer->_api.delete_texture    = nugl__delete_texture;
            renderer->_api.write_texture     = nugl__write_texture;
            renderer->_api.create_material   = nugl__create_material;
            renderer->_api.delete_material   = nugl__delete_material;
            renderer->_api.update_material   = nugl__update_material;
            renderer->_api.create_renderpass = nugl__create_renderpass;
            renderer->_api.delete_renderpass = nugl__delete_renderpass;

            renderer->_api.draw              = nugl__draw;
            renderer->_api.submit_renderpass = nugl__submit_renderpass;
            break;
        case NU_RENDERER_DX11:
            break;
        case NU_RENDERER_SOFTRAST:
            break;
    }

    // Initialize logger
    nu_logger_init(&info->logger, &renderer->_logger);

    NU_INFO(&renderer->_logger, "initialize renderer context");

    // Initialize backend
    nu_error_t error = renderer->_api.init(
        renderer, &renderer->_allocator, platform->_surface.size);
    NU_ERROR_CHECK(error, return error);

    // Create surface texture
    renderer->_surface_color = renderer->_api.create_surface_color(renderer);

    return NU_ERROR_NONE;
}
nu_error_t
nu_renderer_terminate (nu_renderer_t *renderer)
{
    NU_INFO(&renderer->_logger, "terminate renderer context");

    return NU_ERROR_NONE;
}
nu_error_t
nu_render (nu_platform_t *platform, nu_renderer_t *renderer)
{
    renderer->_api.render(renderer,
                          &platform->_surface.glfw.viewport.extent,
                          &platform->_surface.glfw.viewport.viewport);
    return NU_ERROR_NONE;
}
const nu_texture_t *
nu_surface_color_target (const nu_platform_t *platform,
                         const nu_renderer_t *renderer)
{
    return &renderer->_surface_color;
}

nu_camera_info_t
nu_camera_info_default (void)
{
    nu_camera_info_t info;
    info.projection = NU_CAMERA_DEFAULT_PROJECTION;
    info.fov        = NU_CAMERA_DEFAULT_FOV;
    info.near       = NU_CAMERA_DEFAULT_NEAR;
    info.far        = NU_CAMERA_DEFAULT_FAR;
    info.eye        = NU_CAMERA_DEFAULT_EYE;
    info.center     = NU_CAMERA_DEFAULT_CENTER;
    info.up         = NU_CAMERA_DEFAULT_UP;
    return info;
}
nu_error_t
nu_camera_create (nu_renderer_t          *ctx,
                  const nu_camera_info_t *info,
                  nu_camera_t            *camera)
{
    return ctx->_api.create_camera(ctx, info, camera);
}
nu_error_t
nu_camera_delete (nu_renderer_t *ctx, nu_camera_t *camera)
{
    return ctx->_api.delete_camera(ctx, camera);
}
nu_error_t
nu_camera_update (nu_renderer_t          *ctx,
                  nu_camera_t            *camera,
                  const nu_camera_info_t *info)
{
    return ctx->_api.update_camera(ctx, camera, info);
}

nu_error_t
nu_mesh_create (nu_renderer_t *ctx, const nu_mesh_info_t *info, nu_mesh_t *mesh)
{
    return ctx->_api.create_mesh(ctx, info, mesh);
}
nu_error_t
nu_mesh_delete (nu_renderer_t *ctx, nu_mesh_t *mesh)
{
    return ctx->_api.delete_mesh(ctx, mesh);
}

nu_error_t
nu_texture_create (nu_renderer_t           *ctx,
                   const nu_texture_info_t *info,
                   nu_texture_t            *texture)
{
    return ctx->_api.create_texture(ctx, info, texture);
}
nu_error_t
nu_texture_delete (nu_renderer_t *ctx, nu_texture_t *texture)
{
    return ctx->_api.delete_texture(ctx, texture);
}
nu_error_t
nu_texture_write (nu_renderer_t    *ctx,
                  nu_texture_t     *texture,
                  nu_rect_t         rect,
                  const nu_color_t *data)
{
    return ctx->_api.write_texture(ctx, texture, rect, data);
}

nu_material_info_t
nu_material_info_default (void)
{
    nu_material_info_t info;
    info.texture0     = NU_NULL;
    info.texture1     = NU_NULL;
    info.uv_transform = nu_mat3_identity();
    return info;
}
nu_error_t
nu_material_create (nu_renderer_t            *ctx,
                    const nu_material_info_t *info,
                    nu_material_t            *material)
{
    return ctx->_api.create_material(ctx, info, material);
}
nu_error_t
nu_material_delete (nu_renderer_t *ctx, nu_material_t *material)
{
    return ctx->_api.delete_material(ctx, material);
}
nu_error_t
nu_material_update (nu_renderer_t            *ctx,
                    nu_material_t            *material,
                    const nu_material_info_t *info)
{
    return ctx->_api.update_material(ctx, material, info);
}

nu_error_t
nu_renderpass_create (nu_renderer_t              *ctx,
                      const nu_renderpass_info_t *info,
                      nu_renderpass_t            *pass)
{
    return ctx->_api.create_renderpass(ctx, info, pass);
}
nu_error_t
nu_renderpass_delete (nu_renderer_t *ctx, nu_renderpass_t *pass)
{
    return ctx->_api.delete_renderpass(ctx, pass);
}

void
nu_renderpass_submit (nu_renderer_t                *ctx,
                      nu_renderpass_t              *pass,
                      const nu_renderpass_submit_t *info)
{
    ctx->_api.submit_renderpass(ctx, pass, info);
}
void
nu_draw (nu_renderer_t       *ctx,
         nu_renderpass_t     *renderpass,
         const nu_mesh_t     *mesh,
         const nu_material_t *material,
         const nu_mat4_t     *transform)
{
    ctx->_api.draw(ctx, renderpass, mesh, material, transform);
}

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
    controller->_position      = NU_VEC3_ZERO;
    controller->_rotation      = nu_quat_identity();
}
void
nu_camera_controller_update (nu_camera_controller_t *controller,
                             float                   dt,
                             nu_camera_info_t       *info)
{
    // Translation
    nu_vec3_t direction = NU_VEC3_ZERO;

    direction = nu_vec3_add(
        direction,
        nu_vec3_muls(nu_quat_mulv3(controller->_rotation, NU_VEC3_FORWARD),
                     NU_CLAMP01(controller->move_forward)));
    direction = nu_vec3_add(
        direction,
        nu_vec3_muls(nu_quat_mulv3(controller->_rotation, NU_VEC3_BACKWARD),
                     NU_CLAMP01(controller->move_backward)));
    direction = nu_vec3_add(
        direction,
        nu_vec3_muls(nu_quat_mulv3(controller->_rotation, NU_VEC3_LEFT),
                     NU_CLAMP01(controller->move_left)));
    direction = nu_vec3_add(
        direction,
        nu_vec3_muls(nu_quat_mulv3(controller->_rotation, NU_VEC3_RIGHT),
                     NU_CLAMP01(controller->move_right)));
    direction = nu_vec3_add(
        direction,
        nu_vec3_muls(nu_quat_mulv3(controller->_rotation, NU_VEC3_UP),
                     NU_CLAMP01(controller->move_up)));
    direction = nu_vec3_add(
        direction,
        nu_vec3_muls(nu_quat_mulv3(controller->_rotation, NU_VEC3_DOWN),
                     NU_CLAMP01(controller->move_down)));

    direction = nu_vec3_muls(nu_vec3_normalize(direction),
                             dt * 0.001f * controller->speed);

    // Rotation
    if (controller->view_pitch_neg > 0)
    {
        controller->_rotation
            = nu_quat_mul_axis(controller->_rotation,
                               NU_VEC3_RIGHT,
                               nu_radian(controller->view_pitch_neg) * dt);
    }
    if (controller->view_pitch_pos > 0)
    {
        controller->_rotation
            = nu_quat_mul_axis(controller->_rotation,
                               NU_VEC3_RIGHT,
                               -nu_radian(controller->view_pitch_pos) * dt);
    }
    if (controller->view_yaw_neg > 0)
    {
        controller->_rotation
            = nu_quat_mul_axis(controller->_rotation,
                               NU_VEC3_UP,
                               nu_radian(controller->view_yaw_neg) * dt);
    }
    if (controller->view_yaw_pos > 0)
    {
        controller->_rotation
            = nu_quat_mul_axis(controller->_rotation,
                               NU_VEC3_UP,
                               -nu_radian(controller->view_yaw_pos) * dt);
    }
    if (controller->view_roll_neg > 0)
    {
        controller->_rotation
            = nu_quat_mul_axis(controller->_rotation,
                               NU_VEC3_FORWARD,
                               nu_radian(controller->view_roll_neg) * dt);
    }
    if (controller->view_roll_pos > 0)
    {
        controller->_rotation
            = nu_quat_mul_axis(controller->_rotation,
                               NU_VEC3_FORWARD,
                               -nu_radian(controller->view_roll_pos) * dt);
    }

    controller->_position = nu_vec3_add(controller->_position, direction);
    nu_vec3_t forward = nu_quat_mulv3(controller->_rotation, NU_VEC3_FORWARD);
    nu_vec3_t up
        = nu_vec3_normalize(nu_quat_mulv3(controller->_rotation, NU_VEC3_UP));

    info->eye    = controller->_position;
    info->up     = up;
    info->center = nu_vec3_add(controller->_position, forward);
    info->fov    = controller->fov;
}

#endif

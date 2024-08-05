#ifndef NU_RENDERER_H
#define NU_RENDERER_H

#include <nucleus/types.h>

NU_API nu_camera_info_t nu_camera_info_default(void);
NU_API nu_error_t       nu_camera_create(nu_context_t           *ctx,
                                         const nu_camera_info_t *info,
                                         nu_camera_t            *camera);
NU_API nu_error_t nu_camera_delete(nu_context_t *ctx, nu_camera_t *camera);
NU_API nu_error_t nu_camera_update(nu_context_t           *ctx,
                                   nu_camera_t            *camera,
                                   const nu_camera_info_t *info);

NU_API nu_error_t nu_mesh_create(nu_context_t         *ctx,
                                 const nu_mesh_info_t *info,
                                 nu_mesh_t            *mesh);
NU_API nu_error_t nu_mesh_delete(nu_context_t *ctx, nu_mesh_t *mesh);

NU_API nu_error_t nu_texture_create(nu_context_t            *ctx,
                                    const nu_texture_info_t *info,
                                    nu_texture_t            *texture);
NU_API nu_error_t nu_texture_delete(nu_context_t *ctx, nu_texture_t *texture);
NU_API nu_error_t nu_texture_write(nu_context_t     *ctx,
                                   nu_texture_t     *texture,
                                   nu_rect_t         rect,
                                   const nu_color_t *data);

NU_API nu_material_info_t nu_material_info_default(void);
NU_API nu_error_t         nu_material_create(nu_context_t             *ctx,
                                             const nu_material_info_t *info,
                                             nu_material_t            *material);
NU_API nu_error_t         nu_material_delete(nu_context_t  *ctx,
                                             nu_material_t *material);
NU_API nu_error_t         nu_material_update(nu_context_t             *ctx,
                                             nu_material_t            *material,
                                             const nu_material_info_t *info);

NU_API nu_error_t nu_renderpass_create(nu_context_t               *ctx,
                                       const nu_renderpass_info_t *info,
                                       nu_renderpass_t            *pass);
NU_API nu_error_t nu_renderpass_delete(nu_context_t    *ctx,
                                       nu_renderpass_t *pass);

NU_API void nu_renderpass_submit(nu_context_t                 *ctx,
                                 nu_renderpass_t              *pass,
                                 const nu_renderpass_submit_t *info);
NU_API void nu_draw(nu_context_t        *ctx,
                    nu_renderpass_t     *renderpass,
                    const nu_mesh_t     *mesh,
                    const nu_material_t *material,
                    const nu_mat4_t     *transform);

NU_API nu_color_t nu_color(nu_u8_t r, nu_u8_t g, nu_u8_t b, nu_u8_t a);

NU_API void nu_camera_controller_init(nu_camera_controller_t *controller);
NU_API void nu_camera_controller_update(nu_camera_controller_t *controller,
                                        float                   dt,
                                        nu_camera_info_t       *info);

#ifdef NU_IMPLEMENTATION

#include <nucleus/math.h>

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
nu__renderer_null_create_camera (void                   *ctx,
                                 const nu_camera_info_t *info,
                                 nu_camera_t            *camera)
{
    return NU_ERROR_NONE;
}
static nu_error_t
nu__renderer_null_delete_camera (void *ctx, nu_camera_t *camera)
{
    return NU_ERROR_NONE;
}
static nu_error_t
nu__renderer_null_update_camera (void                   *ctx,
                                 nu_camera_t            *camera,
                                 const nu_camera_info_t *info)
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
nu__renderer_null_create_texture (void                    *ctx,
                                  const nu_texture_info_t *info,
                                  nu_texture_t            *texture)
{
    return NU_ERROR_NONE;
}
static nu_error_t
nu__renderer_null_delete_texture (void *ctx, nu_texture_t *texture)
{
    return NU_ERROR_NONE;
}
static nu_error_t
nu__renderer_null_write_texture (void             *ctx,
                                 nu_texture_t     *texture,
                                 nu_rect_t         rect,
                                 const nu_color_t *data)
{
    return NU_ERROR_NONE;
}
static nu_error_t
nu__renderer_null_create_material (void                     *ctx,
                                   const nu_material_info_t *info,
                                   nu_material_t            *material)
{
    return NU_ERROR_NONE;
}
static nu_error_t
nu__renderer_null_delete_material (void *ctx, nu_material_t *material)
{
    return NU_ERROR_NONE;
}
static nu_error_t
nu__renderer_null_update_material (void                     *ctx,
                                   nu_material_t            *material,
                                   const nu_material_info_t *info)
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
nu__renderer_null_draw (void                *ctx,
                        nu_renderpass_t     *pass,
                        const nu_mesh_t     *mesh,
                        const nu_material_t *material,
                        const nu_mat4_t     *transform)
{
}
static void
nu__renderer_null_submit_renderpass (void                         *ctx,
                                     nu_renderpass_t              *pass,
                                     const nu_renderpass_submit_t *info)
{
}

static nu_error_t
nu__init_renderer (nu_context_t *ctx, nu_renderer_backend_t backend)
{
    switch (backend)
    {
        case NU_RENDERER_NULL:
            ctx->_renderer.api.init   = nu__renderer_null_init;
            ctx->_renderer.api.render = nu__renderer_null_render;

            ctx->_renderer.api.create_camera = nu__renderer_null_create_camera;
            ctx->_renderer.api.delete_camera = nu__renderer_null_delete_camera;
            ctx->_renderer.api.update_camera = nu__renderer_null_update_camera;
            ctx->_renderer.api.create_mesh   = nu__renderer_null_create_mesh;
            ctx->_renderer.api.delete_mesh   = nu__renderer_null_delete_mesh;
            ctx->_renderer.api.create_texture
                = nu__renderer_null_create_texture;
            ctx->_renderer.api.delete_texture
                = nu__renderer_null_delete_texture;
            ctx->_renderer.api.write_texture = nu__renderer_null_write_texture;
            ctx->_renderer.api.create_material
                = nu__renderer_null_create_material;
            ctx->_renderer.api.delete_material
                = nu__renderer_null_delete_material;
            ctx->_renderer.api.update_material
                = nu__renderer_null_update_material;
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
            ctx->_renderer.api.create_texture    = nugl__create_texture;
            ctx->_renderer.api.delete_texture    = nugl__delete_texture;
            ctx->_renderer.api.write_texture     = nugl__write_texture;
            ctx->_renderer.api.create_material   = nugl__create_material;
            ctx->_renderer.api.delete_material   = nugl__delete_material;
            ctx->_renderer.api.update_material   = nugl__update_material;
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
        ctx->_renderer.ctx, ctx->_allocator, ctx->_surface.size);
    NU_ERROR_CHECK(error, return error);

    return NU_ERROR_NONE;
}
static nu_error_t
nu__terminate_renderer (nu_context_t *ctx)
{
    return NU_ERROR_NONE;
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
nu_camera_create (nu_context_t           *ctx,
                  const nu_camera_info_t *info,
                  nu_camera_t            *camera)
{
    return ctx->_renderer.api.create_camera(ctx->_renderer.ctx, info, camera);
}
nu_error_t
nu_camera_delete (nu_context_t *ctx, nu_camera_t *camera)
{
    return ctx->_renderer.api.delete_camera(ctx->_renderer.ctx, camera);
}
nu_error_t
nu_camera_update (nu_context_t           *ctx,
                  nu_camera_t            *camera,
                  const nu_camera_info_t *info)
{
    return ctx->_renderer.api.update_camera(ctx->_renderer.ctx, camera, info);
}

nu_error_t
nu_mesh_create (nu_context_t *ctx, const nu_mesh_info_t *info, nu_mesh_t *mesh)
{
    return ctx->_renderer.api.create_mesh(ctx->_renderer.ctx, info, mesh);
}
nu_error_t
nu_mesh_delete (nu_context_t *ctx, nu_mesh_t *mesh)
{
    return ctx->_renderer.api.delete_mesh(ctx->_renderer.ctx, mesh);
}

nu_error_t
nu_texture_create (nu_context_t            *ctx,
                   const nu_texture_info_t *info,
                   nu_texture_t            *texture)
{
    return ctx->_renderer.api.create_texture(ctx->_renderer.ctx, info, texture);
}
nu_error_t
nu_texture_delete (nu_context_t *ctx, nu_texture_t *texture)
{
    return ctx->_renderer.api.delete_texture(ctx->_renderer.ctx, texture);
}
nu_error_t
nu_texture_write (nu_context_t     *ctx,
                  nu_texture_t     *texture,
                  nu_rect_t         rect,
                  const nu_color_t *data)
{
    return ctx->_renderer.api.write_texture(
        ctx->_renderer.ctx, texture, rect, data);
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
nu_material_create (nu_context_t             *ctx,
                    const nu_material_info_t *info,
                    nu_material_t            *material)
{
    return ctx->_renderer.api.create_material(
        ctx->_renderer.ctx, info, material);
}
nu_error_t
nu_material_delete (nu_context_t *ctx, nu_material_t *material)
{
    return ctx->_renderer.api.delete_material(ctx->_renderer.ctx, material);
}
nu_error_t
nu_material_update (nu_context_t             *ctx,
                    nu_material_t            *material,
                    const nu_material_info_t *info)
{
    return ctx->_renderer.api.update_material(
        ctx->_renderer.ctx, material, info);
}

nu_error_t
nu_renderpass_create (nu_context_t               *ctx,
                      const nu_renderpass_info_t *info,
                      nu_renderpass_t            *pass)
{
    return ctx->_renderer.api.create_renderpass(ctx->_renderer.ctx, info, pass);
}
nu_error_t
nu_renderpass_delete (nu_context_t *ctx, nu_renderpass_t *pass)
{
    return ctx->_renderer.api.delete_renderpass(ctx->_renderer.ctx, pass);
}

void
nu_renderpass_submit (nu_context_t                 *ctx,
                      nu_renderpass_t              *pass,
                      const nu_renderpass_submit_t *info)
{
    ctx->_renderer.api.submit_renderpass(ctx->_renderer.ctx, pass, info);
}
void
nu_draw (nu_context_t        *ctx,
         nu_renderpass_t     *renderpass,
         const nu_mesh_t     *mesh,
         const nu_material_t *material,
         const nu_mat4_t     *transform)
{
    ctx->_renderer.api.draw(
        ctx->_renderer.ctx, renderpass, mesh, material, transform);
}

nu_color_t
nu_color (nu_u8_t r, nu_u8_t g, nu_u8_t b, nu_u8_t a)
{
    nu_color_t c;
    c.r = r;
    c.g = g;
    c.b = b;
    c.a = a;
    return c;
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

#endif

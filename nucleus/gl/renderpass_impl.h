#ifndef NUGL_RENDERPASS_IMPL_H
#define NUGL_RENDERPASS_IMPL_H

#include <nucleus/internal.h>

#include <nucleus/gl/flat_impl.h>
#include <nucleus/gl/skybox_impl.h>
#include <nucleus/gl/canvas_impl.h>
#include <nucleus/gl/wireframe_impl.h>

static nu_renderpass_t
nugl__renderpass_create (nu_renderpass_type_t type,
                         nu_bool_t            reset_after_submit)
{
    nugl__renderpass_t *pass  = NU_VEC_PUSH(&_ctx.gl.passes);
    nu_size_t           index = _ctx.gl.passes.size - 1;
    pass->type                = type;
    pass->reset_after_submit  = reset_after_submit;
    pass->has_clear_color     = NU_FALSE;
    pass->clear_color         = NU_COLOR_BLACK;
    pass->color_target        = NU_NULL;
    pass->depth_target        = NU_NULL;

    // Allocate command buffer
    switch (pass->type)
    {
        case NU_RENDERPASS_FLAT:
            nugl__flat_create(&pass->flat);
            break;
        case NU_RENDERPASS_SKYBOX:
            nugl__skybox_create(&pass->skybox);
            break;
        case NU_RENDERPASS_CANVAS:
            nugl__canvas_create(&pass->canvas);
            break;
        case NU_RENDERPASS_WIREFRAME:
            nugl__wireframe_create(&pass->wireframe);
            break;
    }

    return NU_HANDLE_MAKE(nu_renderpass_t, index);
}

static GLuint
nugl__find_or_create_framebuffer (GLuint color, GLuint depth)
{
    nu__gl_t *gl = &_ctx.gl;
    for (nu_size_t i = 0; i < gl->targets.size; ++i)
    {
        const nugl__rendertarget_t *target = gl->targets.data + i;
        if (target->color == color && target->depth == depth)
        {
            return target->fbo;
        }
    }

    NU_INFO("new framebuffer created for color: %d depth: %d", color, depth);

    nugl__rendertarget_t *target = NU_VEC_PUSH(&gl->targets);
    target->color                = color;
    target->depth                = depth;

    glGenFramebuffers(1, &target->fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, target->fbo);
    if (target->color)
    {
        glFramebufferTexture2D(GL_FRAMEBUFFER,
                               GL_COLOR_ATTACHMENT0,
                               GL_TEXTURE_2D,
                               target->color,
                               0);
    }
    if (target->depth)
    {
        glFramebufferTexture2D(GL_FRAMEBUFFER,
                               GL_DEPTH_STENCIL_ATTACHMENT,
                               GL_TEXTURE_2D,
                               target->depth,
                               0);
    }
    NU_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER)
              == GL_FRAMEBUFFER_COMPLETE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return target->fbo;
}
static void
nugl__prepare_color_depth (nugl__renderpass_t *pass,
                           nu_texture_t        color_target,
                           nu_texture_t        depth_target)
{
    nugl__texture_t *color
        = color_target ? _ctx.gl.textures.data + NU_HANDLE_INDEX(color_target)
                       : NU_NULL;
    nugl__texture_t *depth
        = depth_target ? _ctx.gl.textures.data + NU_HANDLE_INDEX(depth_target)
                       : NU_NULL;
    if (color_target || depth_target)
    {
        pass->fbo = nugl__find_or_create_framebuffer(
            color ? color->texture : 0, depth ? depth->texture : 0);
        pass->fbo_size = color ? color->size : depth->size;
    }
    else
    {
        pass->fbo      = 0;
        pass->fbo_size = NU_UVEC2_ZERO;
    }
}
static void
nugl__renderpass_clear_color (nu_renderpass_t pass, nu_color_t *color)
{
    nugl__renderpass_t *ppass = _ctx.gl.passes.data + NU_HANDLE_INDEX(pass);
    if (color)
    {
        ppass->clear_color     = *color;
        ppass->has_clear_color = NU_TRUE;
    }
    else
    {
        ppass->has_clear_color = NU_FALSE;
    }
}
static void
nugl__renderpass_camera (nu_renderpass_t pass, nu_camera_t camera)
{
    NU_ASSERT(camera);
    nugl__renderpass_t *ppass = _ctx.gl.passes.data + NU_HANDLE_INDEX(pass);
    nu_u32_t            index = NU_HANDLE_INDEX(camera);
    switch (ppass->type)
    {
        case NU_RENDERPASS_FLAT:
            ppass->flat.camera = index;
            break;
        case NU_RENDERPASS_WIREFRAME:
            ppass->wireframe.camera = index;
            break;
        default:
            NU_ERROR("invalid");
            break;
    }
}
static void
nugl__renderpass_skybox_cubemap (nu_renderpass_t pass, nu_cubemap_t cubemap)
{
    nugl__renderpass_t *ppass = _ctx.gl.passes.data + NU_HANDLE_INDEX(pass);
    NU_ASSERT(ppass->type == NU_RENDERPASS_SKYBOX);
    ppass->skybox.cubemap = cubemap;
}
static void
nugl__renderpass_skybox_rotation (nu_renderpass_t pass, nu_quat_t rot)
{
    nugl__renderpass_t *ppass = _ctx.gl.passes.data + NU_HANDLE_INDEX(pass);
    NU_ASSERT(ppass->type == NU_RENDERPASS_SKYBOX);
    ppass->skybox.rotation = nu_quat_mat3(rot);
}
static void
nugl__renderpass_target_color (nu_renderpass_t pass, nu_texture_t color)
{
    nugl__renderpass_t *ppass = _ctx.gl.passes.data + NU_HANDLE_INDEX(pass);
    ppass->color_target       = color;
}
static void
nugl__renderpass_target_depth (nu_renderpass_t pass, nu_texture_t depth)
{
    nugl__renderpass_t *ppass = _ctx.gl.passes.data + NU_HANDLE_INDEX(pass);
    ppass->depth_target       = depth;
}
static void
nugl__renderpass_polygon_mode (nu_renderpass_t pass, nu_polygon_mode_t mode)
{
    nugl__renderpass_t *ppass = _ctx.gl.passes.data + NU_HANDLE_INDEX(pass);
    NU_ASSERT(ppass->type == NU_RENDERPASS_WIREFRAME);
    ppass->wireframe.polygon_mode = mode;
}
static void
nugl__reset_renderpass (nugl__renderpass_t *pass)
{
    switch (pass->type)
    {
        case NU_RENDERPASS_FLAT:
            nugl__flat_reset(pass);
            break;
        case NU_RENDERPASS_SKYBOX:
            nugl__skybox_reset(pass);
            break;
        case NU_RENDERPASS_CANVAS:
            nugl__canvas_reset(pass);
            break;
        case NU_RENDERPASS_WIREFRAME:
            nugl__wireframe_reset(pass);
            break;
    }
}
static void
nugl__renderpass_reset (nu_renderpass_t pass)
{
    nugl__renderpass_t *ppass = _ctx.gl.passes.data + NU_HANDLE_INDEX(pass);
    nugl__reset_renderpass(ppass);
}
static void
nugl__renderpass_submit (nu_renderpass_t pass)
{
    nu__gl_t *gl = &_ctx.gl;

    *NU_VEC_PUSH(&gl->passes_order) = NU_HANDLE_INDEX(pass);

    nugl__renderpass_t *ppass = gl->passes.data + NU_HANDLE_INDEX(pass);
    switch (ppass->type)
    {
        case NU_RENDERPASS_FLAT: {
            NU_ASSERT(ppass->color_target && ppass->depth_target);
            nugl__prepare_color_depth(
                ppass, ppass->color_target, ppass->depth_target);
        }
        break;
        case NU_RENDERPASS_SKYBOX: {
            NU_ASSERT(ppass->color_target && ppass->depth_target);
            nugl__prepare_color_depth(
                ppass, ppass->color_target, ppass->depth_target);
        }
        break;
        case NU_RENDERPASS_CANVAS: {
            NU_ASSERT(ppass->color_target);
            nugl__prepare_color_depth(ppass, ppass->color_target, NU_NULL);
        }
        break;
        case NU_RENDERPASS_WIREFRAME: {
            NU_ASSERT(ppass->color_target && ppass->depth_target);
            nugl__prepare_color_depth(
                ppass, ppass->color_target, ppass->depth_target);
        }
        break;
    }
}

static void
nugl__draw_mesh (nu_renderpass_t pass,
                 nu_material_t   material,
                 nu_mesh_t       mesh,
                 nu_mat4_t       transform)
{
    nu__gl_t *gl = &_ctx.gl;

    NU_ASSERT(NU_HANDLE_INDEX(material) < gl->materials.size);
    nugl__renderpass_t *ppass = gl->passes.data + NU_HANDLE_INDEX(pass);
    nugl__mesh_t       *pmesh = gl->meshes.data + NU_HANDLE_INDEX(mesh);
    nugl__material_t   *pmat  = gl->materials.data + NU_HANDLE_INDEX(material);
    NU_ASSERT(pmat->type == NU_MATERIAL_MESH);
    // TODO: check command validity
    switch (ppass->type)
    {
        case NU_RENDERPASS_FLAT:
            nugl__flat_draw_mesh(ppass, pmat, pmesh, transform);
            break;
        case NU_RENDERPASS_WIREFRAME:
            nugl__wireframe_draw_mesh(ppass, pmat, pmesh, transform);
            break;
        default:
            return;
    }
}
static void
nugl__draw_blit (nu_renderpass_t pass,
                 nu_material_t   material,
                 nu_rect_t       extent,
                 nu_rect_t       tex_extent)
{
    nu__gl_t *gl = &_ctx.gl;

    nugl__renderpass_t *ppass = gl->passes.data + NU_HANDLE_INDEX(pass);
    nugl__material_t   *pmat  = gl->materials.data + NU_HANDLE_INDEX(material);
    NU_ASSERT(pmat->type == NU_MATERIAL_CANVAS);

    switch (ppass->type)
    {
        case NU_RENDERPASS_CANVAS: {
            nugl__canvas_blit_rect(
                &ppass->canvas,
                (_ctx.gl.textures.data + NU_HANDLE_INDEX(pmat->canvas.texture0))
                    ->texture,
                extent,
                tex_extent,
                pmat->canvas.wrap_mode);
        }
        break;
        default:
            break;
    }
}

static void
nugl__execute_renderpass (nugl__renderpass_t *pass)
{
    // Bind surface
    glBindFramebuffer(GL_FRAMEBUFFER, pass->fbo);
    glViewport(0, 0, pass->fbo_size.x, pass->fbo_size.y);

    // Clear color
    if (pass->has_clear_color)
    {
        nu_vec4_t clear_color = nu_color_to_vec4(pass->clear_color);
        glClearColor(
            clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    // Render
    switch (pass->type)
    {
        case NU_RENDERPASS_FLAT:
            nugl__flat_render(pass);
            break;
        case NU_RENDERPASS_SKYBOX:
            nugl__skybox_render(pass);
            break;
        case NU_RENDERPASS_CANVAS:
            nugl__canvas_render(pass);
            break;
        case NU_RENDERPASS_WIREFRAME:
            nugl__wireframe_render(pass);
            break;
    }

    // Reset pass
    if (pass->reset_after_submit)
    {
        nugl__reset_renderpass(pass);
    }
}

#endif
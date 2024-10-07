#ifndef NUGL_RENDERPASS_IMPL_H
#define NUGL_RENDERPASS_IMPL_H

#include <nucleus/internal.h>

#include <nucleus/gl/unlit_impl.h>
#include <nucleus/gl/lit_impl.h>
#include <nucleus/gl/skybox_impl.h>
#include <nucleus/gl/canvas_impl.h>
#include <nucleus/gl/wireframe_impl.h>
#include <nucleus/gl/shadow_impl.h>

static void
nugl__submesh_draw_instanced (nugl__mesh_command_vec_t *cmds,
                              const nugl__mesh_t       *pmesh,
                              nu_material_t             mat,
                              nu_size_t                 first,
                              nu_size_t                 count,
                              nu_size_t                 instance_count,
                              const nu_mat4_t          *transforms)
{
    for (nu_size_t i = 0; i < instance_count; ++i)
    {
        nugl__mesh_command_t *cmd = NU_VEC_PUSH(cmds);
        cmd->type                 = NUGL__DRAW;
        cmd->transform            = transforms[i];
        cmd->vao                  = pmesh->vao;
        switch (pmesh->primitive)
        {
            case NU_PRIMITIVE_POINTS:
                cmd->primitive = GL_POINTS;
                break;
            case NU_PRIMITIVE_LINES:
                cmd->primitive = GL_LINES;
                break;
            case NU_PRIMITIVE_LINES_STRIP:
                cmd->primitive = GL_LINE_STRIP;
                break;
            case NU_PRIMITIVE_TRIANGLES:
                cmd->primitive = GL_TRIANGLES;
                break;
        }
        cmd->vfirst   = nu__primitive_vertex_first(pmesh->primitive, first);
        cmd->vcount   = nu__primitive_vertex_count(pmesh->primitive, count);
        cmd->material = mat;
    }
}

static nu_renderpass_t
nugl__renderpass_create (nu_renderpass_type_t type)
{
    nugl__renderpass_t *pass  = NU_VEC_PUSH(&_ctx.gl.passes);
    nu_size_t           index = _ctx.gl.passes.size - 1;
    pass->type                = type;
    pass->reset_after_submit  = NU_TRUE;
    pass->has_clear_color     = NU_FALSE;
    pass->clear_color         = NU_COLOR_BLACK;
    pass->color_target        = NU_NULL;
    pass->depth_target        = NU_NULL;

    // Allocate command buffer
    switch (pass->type)
    {
        case NU_RENDERPASS_UNLIT:
            nugl__unlit_create(&pass->unlit);
            break;
        case NU_RENDERPASS_LIT:
            nugl__lit_create(&pass->lit);
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
        case NU_RENDERPASS_SHADOW:
            nugl__shadow_create(&pass->shadow);
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
        pass->fbo_size = NU_VEC2U_ZEROS;
    }
}

static void
nugl__renderpass_set_reset_after_submit (nu_renderpass_t pass, nu_bool_t bool)
{
    nugl__renderpass_t *ppass = _ctx.gl.passes.data + NU_HANDLE_INDEX(pass);
    ppass->reset_after_submit = bool;
}
static void
nugl__renderpass_set_clear_color (nu_renderpass_t pass, nu_color_t *color)
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
nugl__renderpass_set_camera (nu_renderpass_t pass, nu_camera_t camera)
{
    NU_ASSERT(camera);
    nugl__renderpass_t *ppass = _ctx.gl.passes.data + NU_HANDLE_INDEX(pass);
    switch (ppass->type)
    {
        case NU_RENDERPASS_UNLIT:
            ppass->unlit.camera = camera;
            break;
        case NU_RENDERPASS_LIT:
            ppass->lit.camera = camera;
            break;
        case NU_RENDERPASS_WIREFRAME:
            ppass->wireframe.camera = camera;
            break;
        case NU_RENDERPASS_SKYBOX:
            ppass->skybox.camera = camera;
            break;
        default:
            NU_ERROR("invalid");
            break;
    }
}
static void
nugl__renderpass_set_color_target (nu_renderpass_t pass, nu_texture_t texture)
{
    nugl__renderpass_t *ppass = _ctx.gl.passes.data + NU_HANDLE_INDEX(pass);
    ppass->color_target       = texture;
}
static void
nugl__renderpass_set_depth_target (nu_renderpass_t pass, nu_texture_t texture)
{
    nugl__renderpass_t *ppass = _ctx.gl.passes.data + NU_HANDLE_INDEX(pass);
    ppass->depth_target       = texture;
}
static void
nugl__renderpass_set_shadow_target (nu_renderpass_t pass, nu_texture_t texture)
{
    nugl__renderpass_t *ppass = _ctx.gl.passes.data + NU_HANDLE_INDEX(pass);
    NU_ASSERT(ppass->type == NU_RENDERPASS_SHADOW);
    nugl__shadow_set_depth_map(&ppass->shadow, texture);
}
static void
nugl__renderpass_set_skybox (nu_renderpass_t pass,
                             nu_cubemap_t    cubemap,
                             nu_quat_t       rotation)
{
    nugl__renderpass_t *ppass = _ctx.gl.passes.data + NU_HANDLE_INDEX(pass);
    NU_ASSERT(ppass->type == NU_RENDERPASS_SKYBOX);
    ppass->skybox.cubemap = cubemap;
}

static void
nugl__reset_renderpass (nugl__renderpass_t *pass)
{
    switch (pass->type)
    {
        case NU_RENDERPASS_UNLIT:
            nugl__unlit_reset(&pass->unlit);
            break;
        case NU_RENDERPASS_LIT:
            nugl__lit_reset(&pass->lit);
            break;
        case NU_RENDERPASS_SKYBOX:
            nugl__skybox_reset(&pass->skybox);
            break;
        case NU_RENDERPASS_CANVAS:
            nugl__canvas_reset(&pass->canvas);
            break;
        case NU_RENDERPASS_WIREFRAME:
            nugl__wireframe_reset(&pass->wireframe);
            break;
        case NU_RENDERPASS_SHADOW:
            nugl__shadow_reset(&pass->shadow);
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
        case NU_RENDERPASS_UNLIT: {
            NU_ASSERT(ppass->color_target && ppass->depth_target);
            nugl__prepare_color_depth(
                ppass, ppass->color_target, ppass->depth_target);
        }
        break;
        case NU_RENDERPASS_LIT: {
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
            NU_ASSERT(ppass->color_target);
            nugl__prepare_color_depth(
                ppass, ppass->color_target, ppass->depth_target);
        }
        break;
        case NU_RENDERPASS_SHADOW: {
        }
        break;
    }
}

static void
nugl__bind_material (nu_renderpass_t pass, nu_material_t material)
{
    nu__gl_t *gl = &_ctx.gl;

    nugl__renderpass_t *ppass = gl->passes.data + NU_HANDLE_INDEX(pass);

    switch (ppass->type)
    {
        case NU_RENDERPASS_UNLIT:
            nugl__unlit_bind_material(&ppass->unlit, material);
            break;
        case NU_RENDERPASS_LIT:
            nugl__lit_bind_material(&ppass->lit, material);
            break;
        case NU_RENDERPASS_SKYBOX:
            break;
        case NU_RENDERPASS_CANVAS:
            nugl__canvas_bind_material(&ppass->canvas, material);
            break;
        case NU_RENDERPASS_WIREFRAME:
            nugl__wireframe_bind_material(&ppass->wireframe, material);
            break;
        case NU_RENDERPASS_SHADOW:
            break;
    }
}
static void
nugl__draw_submesh_instanced (nu_renderpass_t  pass,
                              nu_mesh_t        mesh,
                              nu_size_t        first,
                              nu_size_t        count,
                              nu_size_t        instance_count,
                              const nu_mat4_t *transforms)
{
    nugl__renderpass_t *ppass = _ctx.gl.passes.data + NU_HANDLE_INDEX(pass);
    const nugl__mesh_t *pmesh = _ctx.gl.meshes.data + NU_HANDLE_INDEX(mesh);
    // TODO: check command validity ?
    switch (ppass->type)
    {
        case NU_RENDERPASS_UNLIT:
            nugl__submesh_draw_instanced(&ppass->unlit.cmds,
                                         pmesh,
                                         ppass->unlit.material,
                                         first,
                                         count,
                                         instance_count,
                                         transforms);
            break;
        case NU_RENDERPASS_LIT: {
            NU_ASSERT(pmesh->primitive == NU_PRIMITIVE_TRIANGLES);
            nugl__submesh_draw_instanced(&ppass->lit.cmds,
                                         pmesh,
                                         ppass->lit.material,
                                         first,
                                         count,
                                         instance_count,
                                         transforms);
        }
        break;
        case NU_RENDERPASS_WIREFRAME:
            nugl__submesh_draw_instanced(&ppass->wireframe.cmds,
                                         pmesh,
                                         ppass->wireframe.material,
                                         first,
                                         count,
                                         instance_count,
                                         transforms);
            break;
        case NU_RENDERPASS_SHADOW:
            nugl__submesh_draw_instanced(&ppass->shadow.cmds,
                                         pmesh,
                                         NU_NULL,
                                         first,
                                         count,
                                         instance_count,
                                         transforms);
            break;
        default:
            NU_ASSERT(NU_FALSE);
            return;
    }
}
static void
nugl__draw_blit (nu_renderpass_t pass, nu_box2i_t extent, nu_box2i_t tex_extent)
{
    nugl__renderpass_t *ppass = _ctx.gl.passes.data + NU_HANDLE_INDEX(pass);
    switch (ppass->type)
    {
        case NU_RENDERPASS_CANVAS:
            nugl__canvas_draw_blit(ppass, extent, tex_extent);
            break;
        default:
            NU_ASSERT(NU_FALSE);
            break;
    }
}

static void
nugl__execute_renderpass (nugl__renderpass_t *pass)
{

    if (pass->type == NU_RENDERPASS_SHADOW)
    {
        nugl__shadow_render(pass);
    }
    else
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
            case NU_RENDERPASS_UNLIT:
                nugl__unlit_render(pass);
                break;
            case NU_RENDERPASS_LIT:
                nugl__lit_render(pass);
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
            default:
                NU_ASSERT(NU_FALSE);
        }
    }

    // Reset pass
    if (pass->reset_after_submit)
    {
        nugl__reset_renderpass(pass);
    }
}

#endif

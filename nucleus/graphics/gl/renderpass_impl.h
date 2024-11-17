#ifndef NUGL_RENDERPASS_IMPL_H
#define NUGL_RENDERPASS_IMPL_H

#include <nucleus/internal.h>

#include <nucleus/graphics/gl/forward_impl.h>
#include <nucleus/graphics/gl/canvas_impl.h>
#include <nucleus/graphics/gl/shadow_impl.h>

static void
nugl__submesh_draw_instanced (nugl__mesh_command_vec_t *cmds,
                              nu_mesh_t                 mesh,
                              nu_size_t                 first,
                              nu_size_t                 count,
                              nu_material_t             mat,
                              const nu_m4_t            *transforms,
                              nu_size_t                 instance_count)
{
    nu__mesh_t *pmesh = (nu__mesh_t *)mesh;
    for (nu_size_t i = 0; i < instance_count; ++i)
    {
        nugl__mesh_command_t *cmd = NU_VEC_PUSH(cmds);
        cmd->transform            = transforms[i];
        cmd->vao                  = pmesh->gl.vao;
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

static void
nugl__renderpass_init (nu__renderpass_t *pass)
{
    switch (pass->type)
    {
        case NU_RENDERPASS_FORWARD:
            nugl__forward_init(&pass->gl.forward);
            break;
        case NU_RENDERPASS_CANVAS:
            nugl__canvas_init(&pass->gl.canvas);
            break;
        case NU_RENDERPASS_SHADOW:
            nugl__shadow_init(&pass->gl.shadow);
            break;
    }
}
static void
nugl__renderpass_free (nu__renderpass_t *pass)
{
}

static GLuint
nugl__find_or_create_framebuffer (GLuint color, GLuint depth)
{
    nu__gl_t *gl = &_ctx.graphics.gl;
    for (nu_size_t i = 0; i < gl->targets.size; ++i)
    {
        const nugl__rendertarget_t *target = gl->targets.data + i;
        if (target->color == color && target->depth == depth)
        {
            return target->fbo;
        }
    }

    NU_DEBUG("new framebuffer created for color: %d depth: %d", color, depth);

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
nugl__prepare_color_depth (nu__renderpass_t *pass,
                           nu_texture_t      color_target,
                           nu_texture_t      depth_target)
{
    nu__texture_t *color = (nu__texture_t *)color_target;
    nu__texture_t *depth = (nu__texture_t *)depth_target;
    if (color_target || depth_target)
    {
        pass->gl.fbo = nugl__find_or_create_framebuffer(
            color ? color->gl.texture : 0, depth ? depth->gl.texture : 0);
        pass->gl.fbo_size
            = color ? nu_v3u_xy(color->size) : nu_v3u_xy(depth->size);
    }
    else
    {
        pass->gl.fbo      = 0;
        pass->gl.fbo_size = NU_V2U_ZEROS;
    }
}

static void
nugl__renderpass_set_shade (nu__renderpass_t *pass)
{
    switch (pass->forward.mode)
    {
        case NU_SHADE_UNLIT:
            pass->gl.forward.program = _ctx.graphics.gl.unlit_program;
            break;
        case NU_SHADE_LIT:
            pass->gl.forward.program = _ctx.graphics.gl.lit_program;
            break;
        case NU_SHADE_WIREFRAME:
            pass->gl.forward.program = _ctx.graphics.gl.wireframe_program;
            break;
    }
}

static void
nugl__renderpass_reset (nu__renderpass_t *pass)
{
    switch (pass->type)
    {
        case NU_RENDERPASS_FORWARD:
            nugl__forward_reset(&pass->gl.forward);
            break;
        case NU_RENDERPASS_CANVAS:
            nugl__canvas_reset(&pass->gl.canvas);
            break;
        case NU_RENDERPASS_SHADOW:
            nugl__shadow_reset(&pass->gl.shadow);
            break;
    }
}
static void
nugl__renderpass_submit (nu_renderpass_t pass)
{
    nu__gl_t *gl = &_ctx.graphics.gl;

    nu_renderpass_t *pushed = NU_FIXEDVEC_PUSH(&gl->passes_order);
    if (!pushed)
    {
        NU_ERROR("max renderpass submitted count reached");
        return;
    }
    *pushed             = pass;
    nu__renderpass_t *p = (nu__renderpass_t *)pass;

    switch (p->type)
    {
        case NU_RENDERPASS_FORWARD: {
            NU_ASSERT(p->color_target);
            nugl__prepare_color_depth(p, p->color_target, p->depth_target);
        }
        break;
        case NU_RENDERPASS_CANVAS: {
            NU_ASSERT(p->color_target);
            nugl__prepare_color_depth(p, p->color_target, NU_NULL);
        }
        break;
        case NU_RENDERPASS_SHADOW: {
        }
        break;
    }
}

static void
nugl__draw_submesh_instanced (nu__renderpass_t *pass,
                              nu_mesh_t         mesh,
                              nu_size_t         first,
                              nu_size_t         count,
                              nu_material_t     material,
                              const nu_m4_t    *transforms,
                              nu_size_t         instance_count)
{
    // TODO: check command validity ?
    switch (pass->type)
    {
        case NU_RENDERPASS_FORWARD: {
            nugl__submesh_draw_instanced(&pass->gl.forward.cmds,
                                         mesh,
                                         first,
                                         count,
                                         material,
                                         transforms,
                                         instance_count);
        }
        break;
        case NU_RENDERPASS_SHADOW:
            nugl__submesh_draw_instanced(&pass->gl.shadow.cmds,
                                         mesh,
                                         first,
                                         count,
                                         material,
                                         transforms,
                                         instance_count);
            break;
        default:
            NU_ASSERT(NU_FALSE);
            return;
    }
}
static void
nugl__draw_blit (nu__renderpass_t *pass,
                 nu_b2i_t          extent,
                 nu_b2i_t          tex_extent,
                 nu_material_t     material)
{
    switch (pass->type)
    {
        case NU_RENDERPASS_CANVAS:
            nugl__canvas_draw_blit(pass, material, extent, tex_extent);
            break;
        default:
            NU_ASSERT(NU_FALSE);
            break;
    }
}

static void
nugl__execute_renderpass (nu__renderpass_t *pass)
{

    if (pass->type == NU_RENDERPASS_SHADOW)
    {
        nugl__shadow_render(pass);
    }
    else
    {
        // Bind surface
        glBindFramebuffer(GL_FRAMEBUFFER, pass->gl.fbo);
        glViewport(0, 0, pass->gl.fbo_size.x, pass->gl.fbo_size.y);

        // Clear color
        if (pass->has_clear_color)
        {
            nu_v4_t clear_color = nu_color_to_vec4(pass->clear_color);
            glClearColor(
                clear_color.x, clear_color.y, clear_color.z, clear_color.w);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        }

        // Render
        switch (pass->type)
        {
            case NU_RENDERPASS_FORWARD:
                nugl__forward_render(pass);
                break;
            case NU_RENDERPASS_CANVAS:
                nugl__canvas_render(pass);
                break;
            default:
                NU_ASSERT(NU_FALSE);
        }
    }

    // Reset pass
    if (pass->reset_after_submit)
    {
        nugl__renderpass_reset(pass);
    }
}

#endif

#ifndef NUGL_SHADOW_IMPL_H
#define NUGL_SHADOW_IMPL_H

#include <nucleus/internal.h>

static void
nugl__shadow_reset (nugl__renderpass_shadow_t *pass)
{
    NU_VEC_CLEAR(&pass->cmds);
}
static void
nugl__shadow_init (nugl__renderpass_shadow_t *pass)
{
    NU_VEC_ALLOC(&pass->cmds, 128);
    nugl__shadow_reset(pass);
}
static void
nugl__shadow_set_depth_map (nu__renderpass_t *pass, nu_texture_t texture)
{
    nu__gl_t *gl = &_ctx.graphics.gl;
    NU_ASSERT(texture);
    nugfx__texture_t *ptex = (nugfx__texture_t *)texture;
    NU_ASSERT(ptex->type == NU_TEXTURE_SHADOWMAP_TARGET);

    if (pass->gl.fbo)
    {
        glDeleteFramebuffers(1, &pass->gl.fbo);
    }

    glGenFramebuffers(1, &pass->gl.fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, pass->gl.fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER,
                           GL_DEPTH_ATTACHMENT,
                           GL_TEXTURE_2D,
                           ptex->gl.texture,
                           0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
static void
nugl__shadow_render (nu__renderpass_t *pass)
{
    nu__gl_t *gl = &_ctx.graphics.gl;
    NU_ASSERT(pass->shadow.camera && pass->depth_target);
    nu__camera_t  *pcam = (nu__camera_t *)pass->shadow.camera;
    nugfx__texture_t *ptex = (nugfx__texture_t *)pass->depth_target;

    if (!pass->gl.fbo)
    {
        return;
    }

    glViewport(0, 0, ptex->size.x, ptex->size.y);
    glBindFramebuffer(GL_FRAMEBUFFER, pass->gl.fbo);
    glClear(GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    glUseProgram(gl->shadow_program);

    glUniform2uiv(glGetUniformLocation(gl->shadow_program, "viewport_size"),
                  1,
                  ptex->size.data);

    // Compute light view projection matrix
    const nugl__mesh_command_buffer_t *cmds = &pass->gl.shadow.cmds;
    for (nu_size_t i = 0; i < cmds->size; ++i)
    {
        const nugl__mesh_command_t *cmd = &cmds->data[i];

        glUniformMatrix4fv(glGetUniformLocation(gl->shadow_program, "model"),
                           1,
                           GL_FALSE,
                           cmd->transform.data);

        glUniformMatrix4fv(
            glGetUniformLocation(gl->shadow_program, "view_projection"),
            1,
            GL_FALSE,
            pcam->vp.data);

        glBindVertexArray(cmd->vao);
        glDrawArrays(cmd->primitive, cmd->vfirst, cmd->vcount);
        glBindVertexArray(0);
    }

    glUseProgram(0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

#endif

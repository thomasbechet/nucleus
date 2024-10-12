#ifndef NUGL_SHADOW_IMPL_H
#define NUGL_SHADOW_IMPL_H

#include <nucleus/internal.h>

static void
nugl__shadow_reset (nugl__renderpass_shadow_t *pass)
{
    NU_VEC_CLEAR(&pass->cmds);
}
static void
nugl__shadow_create (nugl__renderpass_shadow_t *pass)
{
    NU_VEC_INIT(128, &pass->cmds);
    pass->fbo      = 0;
    pass->depthmap = NU_NULL;
    pass->camera   = NU_NULL;
    nugl__shadow_reset(pass);
}
static void
nugl__shadow_set_depth_map (nugl__renderpass_shadow_t *pass,
                            nu_texture_t               texture)
{
    nu__gl_t        *gl   = &_ctx.gl;
    nugl__texture_t *ptex = gl->textures.data + NU_HANDLE_INDEX(texture);

    if (pass->depthmap != texture)
    {
        pass->depthmap = texture;
        if (pass->fbo)
        {
            glDeleteFramebuffers(1, &pass->fbo);
        }

        glGenFramebuffers(1, &pass->fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, pass->fbo);
        glFramebufferTexture2D(GL_FRAMEBUFFER,
                               GL_DEPTH_ATTACHMENT,
                               GL_TEXTURE_2D,
                               ptex->texture,
                               0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
}
static void
nugl__shadow_render (nugl__renderpass_t *pass)
{
    nu__gl_t *gl = &_ctx.gl;
    NU_ASSERT(pass->shadow.camera && pass->shadow.depthmap);
    nugl__camera_t *pcam
        = gl->cameras.data + NU_HANDLE_INDEX(pass->shadow.camera);
    nugl__texture_t *ptex
        = gl->textures.data + NU_HANDLE_INDEX(pass->shadow.depthmap);

    if (!pass->shadow.fbo)
    {
        return;
    }

    glViewport(0, 0, ptex->size.x, ptex->size.y);
    glBindFramebuffer(GL_FRAMEBUFFER, pass->shadow.fbo);
    glClear(GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    glUseProgram(gl->shadow_program);

    glUniform2uiv(glGetUniformLocation(gl->shadow_program, "viewport_size"),
                  1,
                  ptex->size.data);

    // Compute light view projection matrix
    const nugl__mesh_command_vec_t *cmds = &pass->shadow.cmds;
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

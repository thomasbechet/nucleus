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
    pass->fbo       = 0;
    pass->depth_map = NU_NULL;
    nugl__shadow_reset(pass);
}
static void
nugl__shadow_set_depth_map (nugl__renderpass_shadow_t *pass,
                            nu_texture_t               texture)
{
    nu__gl_t        *gl   = &_ctx.gl;
    nugl__texture_t *ptex = gl->textures.data + NU_HANDLE_INDEX(texture);

    if (pass->depth_map != texture)
    {
        pass->depth_map = texture;
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
    nu__gl_t        *gl = &_ctx.gl;
    nugl__texture_t *ptex
        = gl->textures.data + NU_HANDLE_INDEX(pass->shadow.depth_map);

    if (!pass->fbo)
    {
        return;
    }

    glViewport(0, 0, ptex->size.x, ptex->size.y);
    glBindFramebuffer(GL_FRAMEBUFFER, pass->shadow.fbo);
    glClear(GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    glUseProgram(gl->shadow_program);

    // Compute light view projection matrix
    nu_mat4_t projection = nu_ortho(-50, 50, -50, 50, 1, 500);
    nu_mat4_t view = nu_lookat(nu_vec3s(100.0), NU_VEC3_ZEROS, NU_VEC3_UP);
    nu_mat4_t view_projection = nu_mat4_mul(projection, view);

    const nugl__mesh_command_vec_t *cmds = &pass->shadow.cmds;
    for (nu_size_t i = 0; i < cmds->size; ++i)
    {
        const nugl__mesh_command_t *cmd = &cmds->data[i];
        switch (cmds->data[i].type)
        {
            case NUGL__DRAW: {
                glUniformMatrix4fv(
                    glGetUniformLocation(gl->shadow_program, "model"),
                    1,
                    GL_FALSE,
                    cmd->transform.data);

                glUniformMatrix4fv(
                    glGetUniformLocation(gl->shadow_program, "view_projection"),
                    1,
                    GL_FALSE,
                    view_projection.data);

                glBindVertexArray(cmd->vao);
                glDrawArrays(cmd->primitive, cmd->vfirst, cmd->vcount);
                glBindVertexArray(0);
            }
            break;
            case NUGL__DRAW_INSTANCED: {
            }
            break;
        }
    }

    glUseProgram(0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

#endif

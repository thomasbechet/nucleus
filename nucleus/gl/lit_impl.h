#ifndef NUGL_LIT_H
#define NUGL_LIT_H

#include <nucleus/internal.h>

static void
nugl__lit_reset (nugl__renderpass_lit_t *pass)
{
    NU_VEC_CLEAR(&pass->cmds);
}
static void
nugl__lit_create (nugl__renderpass_lit_t *pass)
{
    NU_VEC_INIT(128, &pass->cmds);
    pass->camera = NU_NULL;
    nugl__lit_reset(pass);
}
static void
nugl__lit_render (nugl__renderpass_t *pass)
{
    nu__gl_t *gl = &_ctx.gl;
    NU_ASSERT(pass->lit.camera);
    nugl__camera_t *cam = gl->cameras.data + NU_HANDLE_INDEX(pass->lit.camera);

    // Prepare pass
    glUseProgram(gl->lit_program);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LEQUAL);
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // Execute commands
    glUniform2uiv(glGetUniformLocation(gl->lit_program, "viewport_size"),
                  1,
                  pass->fbo_size.data);
    const nugl__mesh_command_vec_t *cmds = &pass->lit.cmds;
    for (nu_size_t i = 0; i < cmds->size; ++i)
    {
        const nugl__mesh_command_t *cmd = &cmds->data[i];
        nu_mat3_t                   uv_transform
            = nugl__material_surface_uv_transform(cmd->material);
        GLuint texture0 = nugl__material_surface_texture0(cmd->material);
        switch (cmds->data[i].type)
        {
            case NUGL__DRAW: {

                glUniformMatrix4fv(
                    glGetUniformLocation(gl->lit_program, "model"),
                    1,
                    GL_FALSE,
                    cmd->transform.data);

                glUniformMatrix4fv(
                    glGetUniformLocation(gl->lit_program, "view_projection"),
                    1,
                    GL_FALSE,
                    cam->vp.data);

                glUniformMatrix3fv(
                    glGetUniformLocation(gl->lit_program, "uv_transform"),
                    1,
                    GL_FALSE,
                    uv_transform.data);

                glBindTexture(GL_TEXTURE_2D, texture0);

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
}

#endif

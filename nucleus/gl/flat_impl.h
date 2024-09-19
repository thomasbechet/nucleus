#ifndef NUGL_FLAT_H
#define NUGL_FLAT_H

#include <nucleus/internal.h>

static void
nugl__flat_create (nugl__renderpass_flat_t *pass)
{
    NU_VEC_INIT(128, &pass->cmds);
}
static void
nugl__flat_draw_mesh (nugl__renderpass_t *pass,
                      nugl__material_t   *pmat,
                      nugl__mesh_t       *pmesh,
                      nu_mat4_t           transform)
{
    nugl__mesh_command_t *cmd = NU_VEC_PUSH(&pass->flat.cmds);
    cmd->type                 = NUGL__DRAW;
    cmd->transform            = transform;
    cmd->vao                  = pmesh->vao;
    cmd->vbo                  = pmesh->vbo;
    cmd->vcount               = pmesh->vertex_count;
    cmd->texture0
        = pmat->mesh.texture0
              ? (_ctx.gl.textures.data + NU_HANDLE_INDEX(pmat->mesh.texture0))
                    ->texture
              : 0;
    cmd->texture1
        = pmat->mesh.texture1
              ? (_ctx.gl.textures.data + NU_HANDLE_INDEX(pmat->mesh.texture1))
                    ->texture
              : 0;
    cmd->uv_transform = pmat->mesh.uv_transform;
}
static void
nugl__flat_render (nugl__renderpass_t *pass)
{
    nu__gl_t       *gl  = &_ctx.gl;
    nugl__camera_t *cam = gl->cameras.data + pass->flat.camera;

    // Prepare pass
    glUseProgram(gl->flat_program);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // Execute commands
    glUniform2uiv(glGetUniformLocation(gl->flat_program, "viewport_size"),
                  1,
                  pass->fbo_size.data);
    const nugl__mesh_command_vec_t *cmds = &pass->flat.cmds;
    for (nu_size_t i = 0; i < cmds->size; ++i)
    {
        const nugl__mesh_command_t *cmd = &cmds->data[i];
        switch (cmds->data[i].type)
        {
            case NUGL__DRAW: {

                glUniformMatrix4fv(
                    glGetUniformLocation(gl->flat_program, "model"),
                    1,
                    GL_FALSE,
                    cmd->transform.data);

                glUniformMatrix4fv(
                    glGetUniformLocation(gl->flat_program, "view_projection"),
                    1,
                    GL_FALSE,
                    cam->vp.data);

                glUniformMatrix3fv(
                    glGetUniformLocation(gl->flat_program, "uv_transform"),
                    1,
                    GL_FALSE,
                    cmd->uv_transform.data);

                // glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, cmd->texture0);

                glBindVertexArray(cmd->vao);
                glDrawArrays(GL_TRIANGLES, 0, cmd->vcount);
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
static void
nugl__flat_reset (nugl__renderpass_t *pass)
{
    NU_VEC_CLEAR(&pass->flat.cmds);
}

#endif

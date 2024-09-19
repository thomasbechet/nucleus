#ifndef NUGL_WIREFRAME_IMPL_H
#define NUGL_WIREFRAME_IMPL_H

#include <nucleus/internal.h>

static void
nugl__wireframe_create (nugl__renderpass_wireframe_t *pass)
{
    NU_VEC_INIT(128, &pass->cmds);
    pass->color        = NU_COLOR_WHITE;
    pass->polygon_mode = NU_POLYGON_TRIANGLE;
}
static void
nugl__wireframe_draw_mesh (nugl__renderpass_t *pass,
                           nugl__material_t   *pmat,
                           nugl__mesh_t       *pmesh,
                           nu_mat4_t           transform)
{
    nugl__mesh_command_t *cmd = NU_VEC_PUSH(&pass->wireframe.cmds);
    cmd->type                 = NUGL__DRAW;
    cmd->transform            = transform;
    cmd->vao                  = pmesh->vao;
    cmd->vbo                  = pmesh->vbo;
    cmd->vcount               = pmesh->vertex_count;
}
static void
nugl__wireframe_render (nugl__renderpass_t *pass)
{
    nu__gl_t       *gl  = &_ctx.gl;
    nugl__camera_t *cam = gl->cameras.data + pass->wireframe.camera;

    // Prepare pass
    glUseProgram(gl->wireframe_program);
    glEnable(GL_DEPTH_TEST);
    glFrontFace(GL_CCW);
    glDisable(GL_CULL_FACE);
    switch (pass->wireframe.polygon_mode)
    {
        case NU_POLYGON_POINT:
            glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
            break;
        case NU_POLYGON_LINE:
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            break;
        case NU_POLYGON_LINE_STRIP:
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            break;
        case NU_POLYGON_TRIANGLE:
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            break;
    }

    // Execute commands
    glUniform2uiv(glGetUniformLocation(gl->wireframe_program, "viewport_size"),
                  1,
                  pass->fbo_size.data);
    nu_vec4_t color = nu_color_to_vec4(pass->wireframe.color);
    glUniform3fv(
        glGetUniformLocation(gl->wireframe_program, "color"), 1, color.data);

    const nugl__mesh_command_vec_t *cmds = &pass->wireframe.cmds;
    for (nu_size_t i = 0; i < cmds->size; ++i)
    {
        const nugl__mesh_command_t *cmd = &cmds->data[i];
        switch (cmds->data[i].type)
        {
            case NUGL__DRAW: {
                glUniformMatrix4fv(
                    glGetUniformLocation(gl->wireframe_program, "model"),
                    1,
                    GL_FALSE,
                    cmd->transform.data);

                glUniformMatrix4fv(glGetUniformLocation(gl->wireframe_program,
                                                        "view_projection"),
                                   1,
                                   GL_FALSE,
                                   cam->vp.data);

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
    glEnable(GL_CULL_FACE);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glUseProgram(0);
}
static void
nugl__wireframe_reset (nugl__renderpass_t *pass)
{
    NU_VEC_CLEAR(&pass->wireframe.cmds);
}

#endif

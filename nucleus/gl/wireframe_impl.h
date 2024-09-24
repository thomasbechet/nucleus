#ifndef NUGL_WIREFRAME_IMPL_H
#define NUGL_WIREFRAME_IMPL_H

#include <nucleus/internal.h>

static void
nugl__wireframe_reset (nugl__renderpass_wireframe_t *pass)
{
    NU_VEC_CLEAR(&pass->cmds);
    pass->material = NU_NULL;
}
static void
nugl__wireframe_create (nugl__renderpass_wireframe_t *pass)
{
    NU_VEC_INIT(128, &pass->cmds);
    pass->camera = NU_NULL;
    pass->color  = NU_COLOR_WHITE;
    nugl__wireframe_reset(pass);
}
static void
nugl__wireframe_bind_material (nugl__renderpass_wireframe_t *pass,
                               nu_material_t                 material)
{
    pass->material = material;
}
static void
nugl__wireframe_draw_mesh_instanced (nugl__renderpass_t *pass,
                                     nugl__mesh_t       *pmesh,
                                     nu_size_t           first,
                                     nu_size_t           count,
                                     nu_size_t           instance_count,
                                     const nu_mat4_t    *transforms)
{
    if (!pass->wireframe.material)
    {
        NU_ERROR("no material bound");
        return;
    }
    const nugl__material_t *pmat
        = _ctx.gl.materials.data + NU_HANDLE_INDEX(pass->wireframe.material);
    NU_ASSERT(pmat->type == NU_MATERIAL_TYPE_SURFACE);

    // TODO: instanced rendering
    for (nu_size_t i = 0; i < instance_count; ++i)
    {
        nugl__mesh_command_t *cmd = NU_VEC_PUSH(&pass->wireframe.cmds);
        cmd->type                 = NUGL__DRAW;
        cmd->transform            = transforms[i];
        cmd->vao                  = pmesh->vao;
        cmd->vfirst               = first * 3;
        cmd->vcount               = count * 3;
    }
}
static void
nugl__wireframe_render (nugl__renderpass_t *pass)
{
    nu__gl_t *gl = &_ctx.gl;
    NU_ASSERT(pass->wireframe.camera);
    nugl__camera_t *cam
        = gl->cameras.data + NU_HANDLE_INDEX(pass->wireframe.camera);

    // Prepare pass
    glUseProgram(gl->wireframe_program);
    glEnable(GL_DEPTH_TEST);
    glFrontFace(GL_CCW);
    glDisable(GL_CULL_FACE);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

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
                glDrawArrays(GL_TRIANGLES, cmd->vfirst, cmd->vcount);
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

#endif

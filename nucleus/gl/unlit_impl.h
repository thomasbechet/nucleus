#ifndef NUGL_UNLIT_H
#define NUGL_UNLIT_H

#include <nucleus/internal.h>

static void
nugl__unlit_reset (nugl__renderpass_unlit_t *pass)
{
    NU_VEC_CLEAR(&pass->cmds);
    pass->material = NU_NULL;
}
static void
nugl__unlit_create (nugl__renderpass_unlit_t *pass)
{
    NU_VEC_INIT(128, &pass->cmds);
    pass->camera = NU_NULL;
    nugl__unlit_reset(pass);
}
static void
nugl__unlit_bind_material (nugl__renderpass_unlit_t *pass,
                           nu_material_t             material)
{
    pass->material = material;
}
static void
nugl__unlit_draw_mesh_instanced (nugl__renderpass_t *pass,
                                 nugl__mesh_t       *pmesh,
                                 nu_size_t           first,
                                 nu_size_t           count,
                                 nu_size_t           instance_count,
                                 const nu_mat4_t    *transforms)
{
    if (!pass->unlit.material)
    {
        NU_ERROR("no material bound");
        return;
    }
    const nugl__material_t *pmat
        = _ctx.gl.materials.data + NU_HANDLE_INDEX(pass->unlit.material);
    NU_ASSERT(pmat->type == NU_MATERIAL_TYPE_SURFACE);

    // TODO: instanced rendering
    for (nu_size_t i = 0; i < instance_count; ++i)
    {
        nugl__mesh_command_t *cmd = NU_VEC_PUSH(&pass->unlit.cmds);
        cmd->type                 = NUGL__DRAW;
        cmd->transform            = transforms[i];
        cmd->vao                  = pmesh->vao;
        cmd->vfirst               = first * 3;
        cmd->vcount               = count * 3;
        cmd->texture0             = pmat->mesh.texture0
                                        ? (_ctx.gl.textures.data
                               + NU_HANDLE_INDEX(pmat->mesh.texture0))
                                  ->texture
                                        : 0;
        cmd->texture1             = pmat->mesh.texture1
                                        ? (_ctx.gl.textures.data
                               + NU_HANDLE_INDEX(pmat->mesh.texture1))
                                  ->texture
                                        : 0;
        cmd->uv_transform         = pmat->mesh.uv_transform;
    }
}
static void
nugl__unlit_render (nugl__renderpass_t *pass)
{
    nu__gl_t *gl = &_ctx.gl;
    NU_ASSERT(pass->unlit.camera);
    nugl__camera_t *cam
        = gl->cameras.data + NU_HANDLE_INDEX(pass->unlit.camera);

    // Prepare pass
    glUseProgram(gl->unlit_program);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // Execute commands
    glUniform2uiv(glGetUniformLocation(gl->unlit_program, "viewport_size"),
                  1,
                  pass->fbo_size.data);
    const nugl__mesh_command_vec_t *cmds = &pass->unlit.cmds;
    for (nu_size_t i = 0; i < cmds->size; ++i)
    {
        const nugl__mesh_command_t *cmd = &cmds->data[i];
        switch (cmds->data[i].type)
        {
            case NUGL__DRAW: {

                glUniformMatrix4fv(
                    glGetUniformLocation(gl->unlit_program, "model"),
                    1,
                    GL_FALSE,
                    cmd->transform.data);

                glUniformMatrix4fv(
                    glGetUniformLocation(gl->unlit_program, "view_projection"),
                    1,
                    GL_FALSE,
                    cam->vp.data);

                glUniformMatrix3fv(
                    glGetUniformLocation(gl->unlit_program, "uv_transform"),
                    1,
                    GL_FALSE,
                    cmd->uv_transform.data);

                // glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, cmd->texture0);

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

    glUseProgram(0);
}

#endif

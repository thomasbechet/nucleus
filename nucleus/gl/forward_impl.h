#ifndef NUGL_FORWARD_H
#define NUGL_FORWARD_H

#include <nucleus/internal.h>

static void
nugl__forward_reset (nugl__renderpass_forward_t *pass)
{
    NU_VEC_CLEAR(&pass->cmds);
}
static void
nugl__forward_create (nugl__renderpass_forward_t *pass)
{
    NU_VEC_INIT(128, &pass->cmds);
    pass->camera          = NU_NULL;
    pass->mode            = NU_SHADE_LIT;
    pass->skybox          = NU_NULL;
    pass->skybox_rotation = nu_mat3_identity();
    nugl__forward_reset(pass);
}
static void
nugl__forward_render (nugl__renderpass_t *pass)
{
    nu__gl_t *gl = &_ctx.gl;
    NU_ASSERT(pass->forward.camera);
    nugl__camera_t *cam
        = gl->cameras.data + NU_HANDLE_INDEX(pass->forward.camera);

    glUseProgram(pass->forward.program);

    // Prepare pass
    switch (pass->forward.mode)
    {
        case NU_SHADE_UNLIT: {
            glEnable(GL_DEPTH_TEST);
            glEnable(GL_CULL_FACE);
            glDepthMask(GL_TRUE);
            glDepthFunc(GL_LESS);
            glFrontFace(GL_CCW);
            glCullFace(GL_BACK);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
        break;
        case NU_SHADE_LIT: {
            glEnable(GL_DEPTH_TEST);
            glEnable(GL_CULL_FACE);
            glDepthMask(GL_TRUE);
            glDepthFunc(GL_LEQUAL);
            glFrontFace(GL_CCW);
            glCullFace(GL_BACK);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
        break;
        case NU_SHADE_WIREFRAME: {
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_LESS);
            glFrontFace(GL_CCW);
            glDisable(GL_CULL_FACE);
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }
        break;
    }

    // Execute commands
    glUniform2uiv(glGetUniformLocation(pass->forward.program, "viewport_size"),
                  1,
                  pass->fbo_size.data);
    const nugl__mesh_command_vec_t *cmds = &pass->forward.cmds;
    for (nu_size_t i = 0; i < cmds->size; ++i)
    {
        const nugl__mesh_command_t *cmd = &cmds->data[i];

        // points and lines are skipped for non wireframe shading
        if ((pass->forward.mode != NU_SHADE_WIREFRAME)
            && (cmd->primitive != GL_TRIANGLES))
        {
            continue;
        }

        if (pass->forward.mode == NU_SHADE_WIREFRAME)
        {
            nu_vec4_t color = nu_color_to_vec4(
                nugl__material_surface_color(cmd->material, NU_COLOR_WHITE));
            glUniform3fv(glGetUniformLocation(gl->wireframe_program, "color"),
                         1,
                         color.data);
        }

        nu_mat3_t uv_transform
            = nugl__material_surface_uv_transform(cmd->material);
        GLuint texture0 = nugl__material_surface_texture0(cmd->material);

        glUniformMatrix4fv(glGetUniformLocation(pass->forward.program, "model"),
                           1,
                           GL_FALSE,
                           cmd->transform.data);

        glUniformMatrix4fv(
            glGetUniformLocation(pass->forward.program, "view_projection"),
            1,
            GL_FALSE,
            cam->vp.data);

        glUniformMatrix3fv(
            glGetUniformLocation(pass->forward.program, "uv_transform"),
            1,
            GL_FALSE,
            uv_transform.data);

        glBindTexture(GL_TEXTURE_2D, texture0);

        glBindVertexArray(cmd->vao);
        glDrawArrays(cmd->primitive, cmd->vfirst, cmd->vcount);
        glBindVertexArray(0);
    }

    // Render skybox
    if (pass->forward.skybox)
    {
        nugl__texture_t *cubemap
            = gl->textures.data + NU_HANDLE_INDEX(pass->forward.skybox);

        // Bind program
        glUseProgram(gl->skybox_program);

        glEnable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        glDepthMask(GL_FALSE);
        glDepthFunc(GL_LEQUAL);

        glUniformMatrix4fv(
            glGetUniformLocation(gl->skybox_program, "projection"),
            1,
            GL_FALSE,
            cam->projection.data);
        glUniformMatrix4fv(glGetUniformLocation(gl->skybox_program, "view"),
                           1,
                           GL_FALSE,
                           cam->view.data);
        glUniformMatrix3fv(glGetUniformLocation(gl->skybox_program, "rotation"),
                           1,
                           GL_FALSE,
                           pass->forward.skybox_rotation.data);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap->texture);

        glDrawArrays(GL_TRIANGLES, 0, 3);

        glEnable(GL_CULL_FACE);
        glDepthMask(GL_TRUE);
        glDepthFunc(GL_LESS);
        glUseProgram(0);
    }

    glEnable(GL_CULL_FACE);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glUseProgram(0);
}

#endif

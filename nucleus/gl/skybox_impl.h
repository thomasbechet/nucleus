#ifndef NUGL_SKYBOX_IMPL_H
#define NUGL_SKYBOX_IMPL_H

#include <nucleus/internal.h>

static void
nugl__skybox_render (nugl__renderpass_t *pass)
{
    nu__gl_t        *gl  = &_ctx.gl;
    nugl__camera_t  *cam = gl->cameras.data + pass->skybox.camera;
    nugl__cubemap_t *cubemap
        = gl->cubemaps.data + NU_HANDLE_INDEX(pass->skybox.cubemap);

    // Bind program
    glUseProgram(gl->skybox_program);

    // Bind surface
    glBindFramebuffer(GL_FRAMEBUFFER, pass->fbo);
    glViewport(0, 0, pass->fbo_size.x, pass->fbo_size.y);

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glDepthMask(GL_FALSE);
    glDepthFunc(GL_LEQUAL);

    glUniformMatrix4fv(glGetUniformLocation(gl->skybox_program, "projection"),
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
                       pass->skybox.rotation.data);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap->texture);

    glDrawArrays(GL_TRIANGLES, 0, 3);

    glUseProgram(0);
}
static void
nugl__skybox_reset (nugl__renderpass_t *pass)
{
    NU_VEC_CLEAR(&pass->skybox.cmds);
}

#endif

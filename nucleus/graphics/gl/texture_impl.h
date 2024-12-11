#ifndef NUGL_TEXTURE_IMPL_H
#define NUGL_TEXTURE_IMPL_H

#include <nucleus/internal.h>

static void
nugl__texture_init (nugfx__texture_t *tex)
{
    nu__gl_t *gl = &_ctx.graphics.gl;

    glGenTextures(1, &tex->gl.texture);
    glBindTexture(GL_TEXTURE_2D, tex->gl.texture);
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RGBA,
                 tex->size.x,
                 tex->size.y,
                 0,
                 GL_RGBA,
                 GL_UNSIGNED_BYTE,
                 NU_NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glBindTexture(GL_TEXTURE_2D, 0);
}
static void
nugl__texture_free (nugfx__texture_t *tex)
{
    glDeleteTextures(1, &tex->gl.texture);
}
static void
nugl__texture_upload (nugfx__texture_t *tex)
{
    glBindTexture(GL_TEXTURE_2D, tex->gl.texture);
    glTexSubImage2D(GL_TEXTURE_2D,
                    0,
                    0,
                    0,
                    tex->size.x,
                    tex->size.y,
                    GL_RGBA,
                    GL_UNSIGNED_BYTE,
                    tex->data);
    glBindTexture(GL_TEXTURE_2D, 0);
}

#endif

#ifndef NUGL_TEXTURE_IMPL_H
#define NUGL_TEXTURE_IMPL_H

#include <nucleus/internal.h>

static void
nugl__texture_create (nu__texture_t *tex)
{
    nu__gl_t *gl = &_ctx.gl;

    glGenTextures(1, &tex->gl.texture);

    if (tex->type == NU_TEXTURE_CUBEMAP_COLOR)
    {
        NU_ASSERT(tex->size.x == tex->size.y);
        glBindTexture(GL_TEXTURE_CUBE_MAP, tex->gl.texture);
        for (nu_size_t i = 0; i < 6; ++i)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                         0,
                         GL_RGBA,
                         tex->size.x,
                         tex->size.y,
                         0,
                         GL_RGBA,
                         GL_UNSIGNED_BYTE,
                         NU_NULL);
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(
            GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(
            GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(
            GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    }
    else
    {
        glBindTexture(GL_TEXTURE_2D, tex->gl.texture);
        switch (tex->type)
        {
            case NU_TEXTURE_COLOR:
            case NU_TEXTURE_COLOR_TARGET:
                glTexImage2D(GL_TEXTURE_2D,
                             0,
                             GL_RGBA,
                             tex->size.x,
                             tex->size.y,
                             0,
                             GL_RGBA,
                             GL_UNSIGNED_BYTE,
                             NU_NULL);
                break;
            case NU_TEXTURE_DEPTH_TARGET:
                glTexImage2D(GL_TEXTURE_2D,
                             0,
                             GL_DEPTH24_STENCIL8,
                             tex->size.x,
                             tex->size.y,
                             0,
                             GL_DEPTH_STENCIL,
                             GL_UNSIGNED_INT_24_8,
                             NU_NULL);
                break;
            case NU_TEXTURE_SHADOW_TARGET: {
                glTexImage2D(GL_TEXTURE_2D,
                             0,
                             GL_DEPTH_COMPONENT,
                             tex->size.x,
                             tex->size.y,
                             0,
                             GL_DEPTH_COMPONENT,
                             GL_FLOAT,
                             NU_NULL);
                glTexParameteri(
                    GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
                glTexParameteri(
                    GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
                float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
                glTexParameterfv(
                    GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
            }
            break;
            case NU_TEXTURE_CUBEMAP_COLOR:
                NU_ASSERT(NU_FALSE);
                break;
        }

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glBindTexture(GL_TEXTURE_2D, 0);
    }
}
static void
nugl__texture_free (nu__texture_t *tex)
{
    glDeleteTextures(1, &tex->gl.texture);
}
static void
nugl__texture_write_colors (nu__texture_t *tex, const nu_color_t *colors)
{
    nu__gl_t *gl = &_ctx.gl;

    switch (tex->type)
    {
        case NU_TEXTURE_COLOR: {
            glBindTexture(GL_TEXTURE_2D, tex->gl.texture);
            glTexSubImage2D(GL_TEXTURE_2D,
                            0,
                            0,
                            0,
                            tex->size.x,
                            tex->size.y,
                            GL_RGBA,
                            GL_UNSIGNED_BYTE,
                            colors);
        }
        break;
        default:
            NU_ERROR("invalid texture usage for colors update");
            break;
    }
}
static void
nugl__texture_write_cubemap_colors (nu__texture_t    *tex,
                                    nu_cubemap_face_t face,
                                    const nu_color_t *colors)
{
    nu__gl_t *gl = &_ctx.gl;

    NU_ASSERT(tex->type == NU_TEXTURE_CUBEMAP_COLOR);

    glBindTexture(GL_TEXTURE_CUBE_MAP, tex->gl.texture);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face,
                 0,
                 GL_RGBA,
                 tex->size.x,
                 tex->size.y,
                 0,
                 GL_RGBA,
                 GL_UNSIGNED_BYTE,
                 colors);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

#endif

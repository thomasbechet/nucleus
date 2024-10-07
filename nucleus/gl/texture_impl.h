#ifndef NUGL_TEXTURE_IMPL_H
#define NUGL_TEXTURE_IMPL_H

#include <nucleus/internal.h>

static nu_texture_t
nugl__texture_create (nu_vec2u_t size, nu_texture_type_t usage)
{
    nu__gl_t *gl = &_ctx.gl;

    nugl__texture_t *ptex = NU_VEC_PUSH(&gl->textures);
    nu_texture_t handle   = NU_HANDLE_MAKE(nu_texture_t, gl->textures.size - 1);

    ptex->size  = size;
    ptex->usage = usage;

    glGenTextures(1, &ptex->texture);
    glBindTexture(GL_TEXTURE_2D, ptex->texture);

    switch (usage)
    {
        case NU_TEXTURE_COLOR:
        case NU_TEXTURE_COLOR_TARGET:
            glTexImage2D(GL_TEXTURE_2D,
                         0,
                         GL_RGBA,
                         size.x,
                         size.y,
                         0,
                         GL_RGBA,
                         GL_UNSIGNED_BYTE,
                         NU_NULL);
            break;
        case NU_TEXTURE_DEPTH_TARGET:
            glTexImage2D(GL_TEXTURE_2D,
                         0,
                         GL_DEPTH24_STENCIL8,
                         size.x,
                         size.y,
                         0,
                         GL_DEPTH_STENCIL,
                         GL_UNSIGNED_INT_24_8,
                         NU_NULL);
            break;
        case NU_TEXTURE_SHADOW_TARGET:
            glTexImage2D(GL_TEXTURE_2D,
                         0,
                         GL_DEPTH_COMPONENT,
                         size.x,
                         size.y,
                         0,
                         GL_DEPTH_COMPONENT,
                         GL_FLOAT,
                         NU_NULL);
            break;
    }

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    return handle;
}
static void
nugl__texture_delete (nu_texture_t texture)
{
    nu__gl_t        *gl   = &_ctx.gl;
    nugl__texture_t *ptex = gl->textures.data + NU_HANDLE_INDEX(texture);
    glDeleteTextures(1, &ptex->texture);
}
static void
nugl__texture_colors (nu_texture_t texture, const nu_color_t *colors)
{
    nu__gl_t        *gl   = &_ctx.gl;
    nugl__texture_t *ptex = gl->textures.data + NU_HANDLE_INDEX(texture);

    switch (ptex->usage)
    {
        case NU_TEXTURE_COLOR: {
            glBindTexture(GL_TEXTURE_2D, ptex->texture);
            glTexSubImage2D(GL_TEXTURE_2D,
                            0,
                            0,
                            0,
                            ptex->size.x,
                            ptex->size.y,
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
static nu_cubemap_t
nugl__cubemap_create (nu_u32_t size, const nu_color_t **colors)
{
    nu__gl_t *gl = &_ctx.gl;

    nugl__cubemap_t *pcube = NU_VEC_PUSH(&gl->cubemaps);
    nu_cubemap_t handle = NU_HANDLE_MAKE(nu_cubemap_t, gl->cubemaps.size - 1);

    glGenTextures(1, &pcube->texture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, pcube->texture);
    for (nu_size_t i = 0; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                     0,
                     GL_RGBA,
                     size,
                     size,
                     0,
                     GL_RGBA,
                     GL_UNSIGNED_BYTE,
                     colors[i]);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    return handle;
}

#endif

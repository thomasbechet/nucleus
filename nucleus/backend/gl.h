#ifndef NUGL_BACKEND_H
#define NUGL_BACKEND_H

#include <GL/gl.h>
#include <nucleus/types.h>

#if defined(NU_IMPLEMENTATION) && defined(NU_BUILD_RENDERER_GL)

static const char *nugl__quad_vertex
    = " \
    #version 330 core \n\
    layout(location = 0) in vec2 vertex; \n\
    layout(location = 1) in vec2 uv; \n\
    out vec2 tex_coord; \n\
    void main() { \n\
        tex_coord = uv; \n\
        gl_Position = vec4(vertex, 0, 1); \n\
    }  \
";
static const char *nugl__quad_fragment
    = " \
    #version 330 core \n\
    in vec2 tex_coord; \n\
    uniform sampler2D image; \n\
    out vec4 color; \n\
    void main() { \n\
        color = texture(image, tex_coord); \n\
    }  \
";
static const float nugl__vertices[]
    = { -1, -1, 1, -1, 1, 1, 1, 1, -1, 1, -1, -1 };
static const float nugl__uvs[] = { 0, 0, 1, 0, 1, 1, 1, 1, 0, 1, 0, 0 };

static void
nugl__create_quad_shader (nugl__context_t *ctx)
{
}

static nu_error_t
nugl__clear (void)
{
    return NU_ERROR_NONE;
}

static nu_error_t
nugl__render (void *ctx, const nu_int_t *global_viewport, const float *viewport)
{
    // Clear whole screen
    glViewport(global_viewport[0],
               global_viewport[1],
               global_viewport[2],
               global_viewport[3]);
    glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Clear viewport
    glEnable(GL_SCISSOR_TEST);
    glScissor(viewport[0], viewport[1], viewport[2], viewport[3]);
    glClearColor(1.0f, 1.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_SCISSOR_TEST);

    return NU_ERROR_NONE;
}

#endif

#endif

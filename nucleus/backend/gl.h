#ifndef NUGL_BACKEND_H
#define NUGL_BACKEND_H

#include <nucleus/types.h>

#if defined(NU_IMPLEMENTATION) && defined(NU_BUILD_RENDERER_GL)

// #define GLBIND_IMPLEMENTATION
// #include <nucleus/external/glbind/glbind.h>

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
    GLuint vertex_shader, fragment_shader;
    GLint  success;

    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &nugl__quad_vertex, NULL);
    glCompileShader(vertex_shader);
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE)
    {
        /* nu_warning(NUGLFW_LOGGER_NAME
                   "Failed to compile quad vertex shader.\n"); */
        glDeleteShader(vertex_shader);
    }

    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &nugl__quad_fragment, 0);
    glCompileShader(fragment_shader);
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE)
    {
        /* nu_warning(NUGLFW_LOGGER_NAME
                   "Failed to compile quad fragment shader.\n"); */
        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);
    }

    /* _module.quad_shader = glCreateProgram();
    glAttachShader(_module.quad_shader, vertex_shader);
    glAttachShader(_module.quad_shader, fragment_shader);

    glLinkProgram(_module.quad_shader);
    glGetProgramiv(_module.quad_shader, GL_LINK_STATUS, &success);
    if (success == GL_FALSE)
    {
        nu_warning(NUGLFW_LOGGER_NAME "Failed to link quad shader.\n");
        glDeleteProgram(_module.quad_shader);
        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);
    }

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader); */
}

static nu_error_t
nugl__clear (void)
{
    return NU_ERROR_NONE;
}

void GLAPIENTRY
MessageCallback (GLenum        source,
                 GLenum        type,
                 GLuint        id,
                 GLenum        severity,
                 GLsizei       length,
                 const GLchar *message,
                 const void   *userParam)
{
    fprintf(stderr,
            "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
            (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
            type,
            severity,
            message);
}

static nu_error_t
nugl__init (void *ctx)
{
    // GLenum result = glbInit(NULL, NULL);
    // NU_ASSERT(result == GL_NO_ERROR);

    // During init, enable debug output
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(MessageCallback, 0);
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

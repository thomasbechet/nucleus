#ifndef NUGL_BACKEND_H
#define NUGL_BACKEND_H

#include <nucleus/types.h>

#if defined(NU_IMPLEMENTATION) && defined(NU_BUILD_RENDERER_GL)

#include <nucleus/backend/gl_shader.h>

static nu_error_t
nugl__compile_shader (nugl__context_t *ctx,
                      const nu_char_t *vert,
                      const nu_char_t *frag,
                      GLuint          *program)
{
    GLuint vertex_shader, fragment_shader;
    GLint  success;

    NU_ASSERT(vert && frag);

    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vert, NULL);
    glCompileShader(vertex_shader);
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE)
    {
        glDeleteShader(vertex_shader);
        return NU_ERROR_SHADER_COMPILATION;
    }

    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &frag, 0);
    glCompileShader(fragment_shader);
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE)
    {
        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);
        return NU_ERROR_SHADER_COMPILATION;
    }

    *program = glCreateProgram();
    glAttachShader(*program, vertex_shader);
    glAttachShader(*program, fragment_shader);

    glLinkProgram(*program);
    glGetProgramiv(*program, GL_LINK_STATUS, &success);
    if (success == GL_FALSE)
    {
        glDeleteProgram(*program);
        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);
        return NU_ERROR_SHADER_COMPILATION;
    }

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    return NU_ERROR_NONE;
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
    nu_error_t       error;
    nugl__context_t *context = ctx;

    // During init, enable debug output
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(MessageCallback, 0);

    // Compile programs
    error = nugl__compile_shader(context,
                                 nugl__shader_blit_vert,
                                 nugl__shader_blit_frag,
                                 &context->blit_program);
    NU_ERROR_ASSERT(error);

    return NU_ERROR_NONE;
}

static nu_error_t
nugl__render (void          *ctx,
              const nu_int_t global_viewport[NU_VEC4],
              const float    viewport[NU_VEC4])
{
    nugl__context_t *gl = ctx;

    // Clear whole screen
    glViewport(global_viewport[0],
               global_viewport[1],
               global_viewport[2],
               global_viewport[3]);
    glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Clear viewport
    glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
    glUseProgram(gl->blit_program);
    glBindVertexArray(0);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    return NU_ERROR_NONE;
}

#define NUGL_VERTEX_SIZE (3 + 2)

static nu_error_t
nugl__create_mesh (void *ctx, const nu_mesh_info_t *info, nu_mesh_t *mesh)
{
    NU_ASSERT(info->positions);

    // Create VAO
    glGenVertexArrays(1, &mesh->gl.vao);
    glBindVertexArray(mesh->gl.vao);

    // Create VBO
    glGenBuffers(1, &mesh->gl.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, mesh->gl.vbo);

    // Format vertices
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(float) * info->vertex_count * NUGL_VERTEX_SIZE,
                 NU_NULL,
                 GL_STATIC_DRAW);
    float *ptr = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    NU_ASSERT(ptr);
    for (nu_size_t i = 0; i < info->vertex_count; ++i)
    {
        nu_memcpy(ptr + i * NUGL_VERTEX_SIZE,
                  info->positions + i * NU_VEC3,
                  sizeof(float) * NU_VEC3);
        if (info->uvs)
        {
            nu_memcpy(ptr + i * NUGL_VERTEX_SIZE + NU_VEC3,
                      info->uvs + i * NU_VEC2,
                      sizeof(float) * NU_VEC2);
        }
    }
    glUnmapBuffer(GL_ARRAY_BUFFER);

    // Configure VAO
    glVertexAttribPointer(
        0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * NUGL_VERTEX_SIZE, (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1,
                          2,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(float) * NUGL_VERTEX_SIZE,
                          (void *)(sizeof(float) * NU_VEC3));
    glEnableVertexAttribArray(1);

    // Unbind buffers
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return NU_ERROR_NONE;
}
static nu_error_t
nugl__delete_mesh (void *ctx, nu_mesh_t *mesh)
{
    glDeleteVertexArrays(1, &mesh->gl.vao);
    glDeleteBuffers(1, &mesh->gl.vbo);
    return NU_ERROR_NONE;
}

#endif

#endif

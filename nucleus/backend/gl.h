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
        GLint max_length = 0;
        glGetShaderiv(vertex_shader, GL_INFO_LOG_LENGTH, &max_length);
        char *log = malloc(sizeof(char) * max_length);
        glGetShaderInfoLog(vertex_shader, max_length, &max_length, log);
        fprintf(stderr, "%s\n", log);

        glDeleteShader(vertex_shader);
        return NU_ERROR_SHADER_COMPILATION;
    }

    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &frag, 0);
    glCompileShader(fragment_shader);
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE)
    {
        GLint max_length = 0;
        glGetShaderiv(fragment_shader, GL_INFO_LOG_LENGTH, &max_length);
        char *log = malloc(sizeof(char) * max_length);
        glGetShaderInfoLog(fragment_shader, max_length, &max_length, log);
        fprintf(stderr, "%s\n", log);

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
nugl__init (void *ctx, const nu_int_t size[NU_V2])
{
    nu_error_t       error;
    nugl__context_t *gl = ctx;

    // During init, enable debug output
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(MessageCallback, 0);

    // Compile programs
    error = nugl__compile_shader(
        gl, nugl__shader_blit_vert, nugl__shader_blit_frag, &gl->blit_program);
    NU_ERROR_ASSERT(error);
    error = nugl__compile_shader(
        gl, nugl__shader_flat_vert, nugl__shader_flat_frag, &gl->flat_program);
    NU_ERROR_ASSERT(error);

    // Create nearest sampler
    glGenSamplers(1, &gl->nearest_sampler);
    glSamplerParameteri(gl->nearest_sampler, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glSamplerParameteri(gl->nearest_sampler, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glSamplerParameteri(gl->nearest_sampler, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glSamplerParameteri(gl->nearest_sampler, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    // Create surface texture and framebuffer
    nu_iv2_copy(size, gl->surface_size);
    glGenTextures(1, &gl->surface_texture);
    glBindTexture(GL_TEXTURE_2D, gl->surface_texture);
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RGB,
                 size[0],
                 size[1],
                 0,
                 GL_RGB,
                 GL_UNSIGNED_BYTE,
                 NU_NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glGenFramebuffers(1, &gl->surface_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, gl->surface_fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER,
                           GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D,
                           gl->surface_texture,
                           0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return NU_ERROR_NONE;
}

static nu_error_t
nugl__render (void          *ctx,
              const nu_int_t global_viewport[NU_V4],
              const float    viewport[NU_V4])
{
    nugl__context_t *gl = ctx;

    // Prepare matrix
    float model[NU_M4];
    nu_mat4_identity(model);
    float view[NU_M4];
    float eye[NU_V3]    = { 1.0f, 0.0f, 1.0f };
    float center[NU_V3] = { 0.0f, 0.0f, 0.0f };
    nu_lookat(eye, center, NU_V3_UP, view);
    float projection[NU_M4];
    float aspect = viewport[2] / viewport[3];
    nu_perspective(nu_radian(70.0f), aspect, 0.01f, 100.0f, projection);

    // Bind surface
    glBindFramebuffer(GL_FRAMEBUFFER, gl->surface_fbo);
    glViewport(0, 0, gl->surface_size[0], gl->surface_size[1]);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Render
    glUseProgram(gl->flat_program);
    GLuint modelId = glGetUniformLocation(gl->flat_program, "model");
    glUniformMatrix4fv(modelId, 1, GL_FALSE, model);
    GLuint viewId = glGetUniformLocation(gl->flat_program, "view");
    glUniformMatrix4fv(viewId, 1, GL_FALSE, view);
    GLuint projectionId = glGetUniformLocation(gl->flat_program, "projection");
    glUniformMatrix4fv(projectionId, 1, GL_FALSE, projection);

    glBindVertexArray(gl->mesh->vao);
    glDrawArrays(GL_TRIANGLES, 0, gl->mesh->vertex_count);
    glBindVertexArray(0);

    // Blit surface
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
    glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(gl->blit_program);
    glBindTexture(GL_TEXTURE_2D, gl->surface_texture);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    return NU_ERROR_NONE;
}

static nu_error_t
nugl__create_camera (void *ctx, nu_camera_t *camera)
{
    return NU_ERROR_NONE;
}
static nu_error_t
nugl__delete_camera (void *ctx, nu_camera_t *camera)
{
    return NU_ERROR_NONE;
}
static nu_error_t
nugl__create_mesh (void *ctx, const nu_mesh_info_t *info, nu_mesh_t *mesh)
{
    NU_ASSERT(info->positions);

    mesh->gl.vertex_count = info->vertex_count;

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
                  info->positions + i * NU_V3,
                  sizeof(float) * NU_V3);
        if (info->uvs)
        {
            nu_memcpy(ptr + i * NUGL_VERTEX_SIZE + NU_V3,
                      info->uvs + i * NU_V2,
                      sizeof(float) * NU_V2);
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
                          (void *)(sizeof(float) * NU_V3));
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

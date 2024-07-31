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
nugl__init (void *ctx, nu_uvec2_t size)
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
    gl->surface_size = size;
    glGenTextures(1, &gl->surface_texture);
    glBindTexture(GL_TEXTURE_2D, gl->surface_texture);
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RGB,
                 size.x,
                 size.y,
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
nugl__render (void             *ctx,
              const nu_uvec4_t *global_viewport,
              const nu_vec4_t  *viewport)
{
    nugl__context_t *gl = ctx;

    // Prepare matrix
    nu_mat4_t model = nu_mat4_identity();

    // Bind surface
    glBindFramebuffer(GL_FRAMEBUFFER, gl->surface_fbo);
    glViewport(0, 0, gl->surface_size.x, gl->surface_size.y);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Render
    glUseProgram(gl->flat_program);
    GLuint model_id = glGetUniformLocation(gl->flat_program, "model");
    glUniformMatrix4fv(model_id, 1, GL_FALSE, model.data);
    GLuint vp_id = glGetUniformLocation(gl->flat_program, "view_projection");
    glUniformMatrix4fv(vp_id, 1, GL_FALSE, gl->cam->vp.data);

    glBindVertexArray(gl->mesh->vao);
    glDrawArrays(GL_TRIANGLES, 0, gl->mesh->vertex_count);
    glBindVertexArray(0);

    // Blit surface
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(viewport->x, viewport->y, viewport->z, viewport->w);
    glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(gl->blit_program);
    glBindTexture(GL_TEXTURE_2D, gl->surface_texture);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    // TODO: resolve renderpass tree
    // TODO: resolve renderpass targets
    // TODO: batch render calls
    // TODO: bind renderpass states
    // TODO: execute draw calls
    // TODO: ?? sort draw calls

    return NU_ERROR_NONE;
}

static nu_error_t
nugl__update_camera (void *ctx, nu_camera_t *camera)
{
    nugl__context_t *gl = ctx;

    nu_mat4_t view   = nu_lookat(camera->eye, camera->center, camera->up);
    float     aspect = (float)gl->surface_size.x / (float)gl->surface_size.y;
    nu_mat4_t projection = nu_perspective(
        nu_radian(camera->fov), aspect, camera->near, camera->far);

    camera->_data.gl.vp = nu_mat4_mul(projection, view);

    return NU_ERROR_NONE;
}
static nu_error_t
nugl__create_camera (void *ctx, nu_camera_t *camera)
{
    nu_error_t error;
    error = nugl__update_camera(ctx, camera);
    NU_ERROR_CHECK(error, return error);
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

    mesh->gl.vertex_count = info->count;

    // Create VAO
    glGenVertexArrays(1, &mesh->gl.vao);
    glBindVertexArray(mesh->gl.vao);

    // Create VBO
    glGenBuffers(1, &mesh->gl.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, mesh->gl.vbo);

    // Format vertices
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(float) * info->count * NUGL__VERTEX_SIZE,
                 NU_NULL,
                 GL_STATIC_DRAW);
    float *ptr = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    NU_ASSERT(ptr);
    for (nu_size_t i = 0; i < info->count; ++i)
    {
        ptr[i * NUGL__VERTEX_SIZE + 0] = info->positions[i].x;
        ptr[i * NUGL__VERTEX_SIZE + 1] = info->positions[i].y;
        ptr[i * NUGL__VERTEX_SIZE + 2] = info->positions[i].z;
        if (info->uvs)
        {
            ptr[i * NUGL__VERTEX_SIZE + 3] = info->uvs[i].x;
            ptr[i * NUGL__VERTEX_SIZE + 4] = info->uvs[i].y;
        }
    }
    glUnmapBuffer(GL_ARRAY_BUFFER);

    // Configure VAO
    glVertexAttribPointer(
        0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * NUGL__VERTEX_SIZE, (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1,
                          2,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(float) * NUGL__VERTEX_SIZE,
                          (void *)(sizeof(float) * NU_VEC3_SIZE));
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
static nu_error_t
nugl__create_renderpass (void                       *ctx,
                         const nu_renderpass_info_t *info,
                         nu_renderpass_t            *pass)
{
    switch (info->type)
    {
        case NU_RENDERPASS_FLAT:
            break;
        case NU_RENDERPASS_TRANSPARENT:
            break;
    }
    pass->gl.type       = info->type;
    pass->gl.cmds.count = 0;
    return NU_ERROR_NONE;
}
static nu_error_t
nugl__delete_renderpass (void *ctx, nu_renderpass_t *pass)
{
    return NU_ERROR_NONE;
}
static void
nugl__submit_renderpass (void                         *ctx,
                         nu_renderpass_t              *pass,
                         const nu_renderpass_submit_t *info)
{
    // Prepare pass
    const nugl__renderpass_t *renderpass = &pass->gl;
    switch (renderpass->type)
    {
        case NU_RENDERPASS_FLAT: {
        }
        break;
        case NU_RENDERPASS_TRANSPARENT:
            break;
    }

    // Execute pass
    const nugl__command_buffer_t *cmds = &pass->gl.cmds;
    for (nu_size_t i = 0; i < cmds->count; ++i)
    {
        switch (cmds->commands[i].type)
        {
            case NUGL__DRAW:
                break;
            case NUGL__DRAW_INSTANCED:
                break;
        }
    }

    // Terminate pass

    if (info->reset)
    {
        pass->gl.cmds.count = 0;
    }
}

#endif

#endif

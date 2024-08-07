#ifndef NUGL_IMPL_H
#define NUGL_IMPL_H

#include <nucleus/graphics/renderer.h>
#include <nucleus/graphics/gl_shaders.h>

#define GLAD_GL_IMPLEMENTATION
#include <nucleus/external/glad/gl.h>

static nu_error_t
nugl__compile_shader (nu_renderer_t   *ctx,
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
        NU_ERROR(&ctx->_logger, "%s", log);

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
        NU_ERROR(&ctx->_logger, "%s", log);

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
    nu_renderer_t *ctx = (nu_renderer_t *)userParam;
    switch (severity)
    {
        case GL_DEBUG_SEVERITY_HIGH:
            NU_ERROR(&ctx->_logger,
                     "GL: %s, message = %s",
                     (type == GL_DEBUG_TYPE_ERROR ? "ERROR " : ""),
                     message);
            break;
        case GL_DEBUG_SEVERITY_MEDIUM:
            NU_ERROR(&ctx->_logger,
                     "GL: %s, message = %s",
                     (type == GL_DEBUG_TYPE_ERROR ? "ERROR " : ""),
                     message);
            break;
        case GL_DEBUG_SEVERITY_LOW:
            NU_WARNING(&ctx->_logger,
                       "GL: %s, message = %s",
                       (type == GL_DEBUG_TYPE_ERROR ? "ERROR " : ""),
                       message);
            break;
        case GL_DEBUG_SEVERITY_NOTIFICATION:
            NU_INFO(&ctx->_logger,
                    "GL: %s, message = %s",
                    (type == GL_DEBUG_TYPE_ERROR ? "ERROR " : ""),
                    message);
            break;
    }
}

static nugl__context_t *
nugl__ctx (nu_renderer_t *ctx)
{
    return &ctx->_backend.gl;
}
static nu_error_t
nugl__init (nu_renderer_t *ctx, nu_allocator_t *allocator, nu_uvec2_t size)
{
    nu_error_t       error;
    nugl__context_t *gl = nugl__ctx(ctx);

    gl->allocator     = *allocator; // TODO: who is handles the allocator ?
    gl->surface_size  = size;
    gl->surface_color = 0;
    gl->target_count  = 0;

    // Initialize GL functions
    if (!gladLoadGL(glfwGetProcAddress))
    {
        NU_ERROR(&ctx->_logger, "failed to load GL functions");
        return NU_ERROR_BACKEND;
    }

    // During init, enable debug output
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(MessageCallback, ctx);

    // Compile programs
    error = nugl__compile_shader(
        ctx, nugl__shader_blit_vert, nugl__shader_blit_frag, &gl->blit_program);
    NU_ERROR_CHECK(error, return error);
    error = nugl__compile_shader(
        ctx, nugl__shader_flat_vert, nugl__shader_flat_frag, &gl->flat_program);
    NU_ERROR_CHECK(error, return error);

    // Create nearest sampler
    glGenSamplers(1, &gl->nearest_sampler);
    // glSamplerParameteri(gl->nearest_sampler, GL_TEXTURE_WRAP_S, GL_REPEAT);
    // glSamplerParameteri(gl->nearest_sampler, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glSamplerParameteri(gl->nearest_sampler, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glSamplerParameteri(gl->nearest_sampler, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    // Initialize passes
    gl->pass_count       = 0;
    gl->pass_order_count = 0;

    return NU_ERROR_NONE;
}
static nu_error_t
nugl__render (nu_renderer_t   *ctx,
              const nu_rect_t *global_viewport,
              const nu_rect_t *viewport)
{
    nugl__context_t *gl = nugl__ctx(ctx);

    for (nu_u32_t i = 0; i < gl->pass_order_count; ++i)
    {
        nu_u32_t                 passid = gl->pass_order[i];
        nugl__renderpass_data_t *pass   = &gl->passes[passid];
        switch (pass->type)
        {
            case NU_RENDERPASS_UNLIT:
            case NU_RENDERPASS_FLAT: {
                // Prepare pass
                {
                    // Bind surface
                    glBindFramebuffer(GL_FRAMEBUFFER, pass->fbo);
                    glViewport(0, 0, pass->fbo_size.x, pass->fbo_size.y);
                    nu_vec4_t clear_color = nu_color_to_vec4(pass->clear_color);

                    glEnable(GL_DEPTH_TEST);
                    glEnable(GL_CULL_FACE);
                    // glDisable(GL_DEPTH_TEST);
                    // glDisable(GL_CULL_FACE);
                    glFrontFace(GL_CCW);
                    glCullFace(GL_BACK);

                    glClearColor(clear_color.x,
                                 clear_color.y,
                                 clear_color.z,
                                 clear_color.w);
                    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                    // Bind program
                    glUseProgram(gl->flat_program);
                }

                // Execute commands
                {
                    const nugl__command_buffer_t *cmds = &pass->cmds;
                    for (nu_size_t i = 0; i < cmds->count; ++i)
                    {
                        const nugl__command_t *cmd = &cmds->commands[i];
                        switch (cmds->commands[i].type)
                        {
                            case NUGL__DRAW: {
                                GLuint id = glGetUniformLocation(
                                    gl->flat_program, "viewport_size");
                                glUniform2uiv(id, 1, pass->fbo_size.xy);

                                id = glGetUniformLocation(gl->flat_program,
                                                          "model");
                                glUniformMatrix4fv(
                                    id, 1, GL_FALSE, cmd->transform.data);

                                id = glGetUniformLocation(gl->flat_program,
                                                          "view_projection");
                                glUniformMatrix4fv(
                                    id, 1, GL_FALSE, pass->vp.data);

                                id = glGetUniformLocation(gl->flat_program,
                                                          "uv_transform");
                                glUniformMatrix3fv(
                                    id, 1, GL_FALSE, cmd->uv_transform.data);

                                glActiveTexture(GL_TEXTURE0);
                                glBindTexture(GL_TEXTURE_2D, cmd->texture0);

                                glBindVertexArray(cmd->vao);
                                glDrawArrays(GL_TRIANGLES, 0, cmd->vcount);
                                glBindVertexArray(0);
                            }
                            break;
                            case NUGL__DRAW_INSTANCED: {
                            }
                            break;
                        }
                    }
                }

                // Terminate pass
                {
                    glUseProgram(0);
                }
            }
            break;
            case NU_RENDERPASS_TRANSPARENT:
                break;
        }

        // Reset pass commands
        if (pass->reset)
        {
            pass->cmds.count = 0;
        }
    }

    // Blit surface
    glEnable(GL_FRAMEBUFFER_SRGB);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(viewport->x, viewport->y, viewport->w, viewport->h);
    nu_color_t clear_color = nu_color(25, 27, 43, 255);
    clear_color            = nu_color_to_linear(clear_color);
    nu_vec4_t clear_vec    = nu_color_to_vec4(clear_color);
    glClearColor(clear_vec.x, clear_vec.y, clear_vec.z, clear_vec.w);
    glDisable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(gl->blit_program);
    glBindTexture(GL_TEXTURE_2D, gl->surface_color);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    // Reset for next frame
    gl->pass_order_count = 0;

    return NU_ERROR_NONE;
}
static nu_texture_t
nugl__create_surface_color (nu_renderer_t *ctx)
{
    nugl__context_t *gl = nugl__ctx(ctx);

    nu_texture_t texture;
    glGenTextures(1, &texture._gl.texture);
    glBindTexture(GL_TEXTURE_2D, texture._gl.texture);
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_SRGB,
                 gl->surface_size.x,
                 gl->surface_size.y,
                 0,
                 GL_RGB,
                 GL_UNSIGNED_BYTE,
                 NU_NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    texture._size = gl->surface_size;

    gl->surface_color = texture._gl.texture;

    return texture;
}

static nu_error_t
nugl__update_camera (nu_renderer_t          *ctx,
                     nu_camera_t            *camera,
                     const nu_camera_info_t *info)
{
    nugl__context_t *gl = nugl__ctx(ctx);

    nu_mat4_t view   = nu_lookat(info->eye, info->center, info->up);
    float     aspect = (float)gl->surface_size.x / (float)gl->surface_size.y;
    nu_mat4_t projection
        = nu_perspective(nu_radian(info->fov), aspect, info->near, info->far);

    camera->gl.vp = nu_mat4_mul(projection, view);

    return NU_ERROR_NONE;
}
static nu_error_t
nugl__create_camera (nu_renderer_t          *ctx,
                     const nu_camera_info_t *info,
                     nu_camera_t            *camera)
{
    nu_error_t error;
    error = nugl__update_camera(ctx, camera, info);
    NU_ERROR_CHECK(error, return error);
    return NU_ERROR_NONE;
}
static nu_error_t
nugl__delete_camera (nu_renderer_t *ctx, nu_camera_t *camera)
{
    return NU_ERROR_NONE;
}
static nu_error_t
nugl__create_mesh (nu_renderer_t        *ctx,
                   const nu_mesh_info_t *info,
                   nu_mesh_t            *mesh)
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
        if (info->normals)
        {
            ptr[i * NUGL__VERTEX_SIZE + 5] = info->normals[i].x;
            ptr[i * NUGL__VERTEX_SIZE + 6] = info->normals[i].y;
            ptr[i * NUGL__VERTEX_SIZE + 7] = info->normals[i].z;
        }
    }
    glUnmapBuffer(GL_ARRAY_BUFFER);

    // Configure VAO
    // Position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * NUGL__VERTEX_SIZE, (void *)0);
    // UV
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1,
                          2,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(float) * NUGL__VERTEX_SIZE,
                          (void *)(sizeof(float) * NU_VEC3_SIZE));
    // Normal
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(
        2,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(float) * NUGL__VERTEX_SIZE,
        (void *)(sizeof(float) * (NU_VEC3_SIZE + NU_VEC2_SIZE)));

    // Unbind buffers
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return NU_ERROR_NONE;
}
static nu_error_t
nugl__delete_mesh (nu_renderer_t *ctx, nu_mesh_t *mesh)
{
    glDeleteVertexArrays(1, &mesh->gl.vao);
    glDeleteBuffers(1, &mesh->gl.vbo);
    return NU_ERROR_NONE;
}
static nu_error_t
nugl__create_texture (nu_renderer_t           *ctx,
                      const nu_texture_info_t *info,
                      nu_texture_t            *texture)
{

    glGenTextures(1, &texture->_gl.texture);
    glBindTexture(GL_TEXTURE_2D, texture->_gl.texture);

    switch (info->format)
    {
        case NU_TEXTURE_FORMAT_COLOR:
            glTexImage2D(GL_TEXTURE_2D,
                         0,
                         GL_RGB,
                         info->size.x,
                         info->size.y,
                         0,
                         GL_RGB,
                         GL_UNSIGNED_BYTE,
                         NU_NULL);
            break;
        case NU_TEXTURE_FORMAT_DEPTH:
            glTexImage2D(GL_TEXTURE_2D,
                         0,
                         GL_DEPTH24_STENCIL8,
                         info->size.x,
                         info->size.y,
                         0,
                         GL_DEPTH_STENCIL,
                         GL_UNSIGNED_INT_24_8,
                         NU_NULL);
            break;
    }

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    return NU_ERROR_NONE;
}
static nu_error_t
nugl__delete_texture (nu_renderer_t *ctx, nu_texture_t *texture)
{
    glDeleteTextures(1, &texture->_gl.texture);
    return NU_ERROR_NONE;
}
static nu_error_t
nugl__write_texture (nu_renderer_t    *ctx,
                     nu_texture_t     *texture,
                     nu_rect_t         rect,
                     const nu_color_t *data)
{
    // TODO: handle sub texture rect
    NU_ASSERT(rect.x >= 0 && rect.y >= 0);
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RGBA,
                 rect.w,
                 rect.h,
                 0,
                 GL_RGBA,
                 GL_UNSIGNED_BYTE,
                 data);

    return NU_ERROR_NONE;
}
static nu_error_t
nugl__update_material (nu_renderer_t            *ctx,
                       nu_material_t            *material,
                       const nu_material_info_t *info)
{
    if (info->texture0)
    {
        material->gl.texture0 = info->texture0->_gl.texture;
    }
    if (info->texture1)
    {
        material->gl.texture1 = info->texture1->_gl.texture;
    }
    material->gl.uv_transform = info->uv_transform;
    return NU_ERROR_NONE;
}
static nu_error_t
nugl__create_material (nu_renderer_t            *ctx,
                       const nu_material_info_t *info,
                       nu_material_t            *material)
{
    nugl__update_material(ctx, material, info);
    return NU_ERROR_NONE;
}
static nu_error_t
nugl__delete_material (nu_renderer_t *ctx, nu_material_t *material)
{
    return NU_ERROR_NONE;
}
static nu_error_t
nugl__create_renderpass (nu_renderer_t              *ctx,
                         const nu_renderpass_info_t *info,
                         nu_renderpass_t            *pass)
{
    nugl__context_t *gl = nugl__ctx(ctx);

    nu_u32_t                 passid = gl->pass_count++;
    nugl__renderpass_data_t *data   = &gl->passes[passid];
    data->type                      = info->type;

    // Allocate command buffer
    data->cmds.commands = nu_alloc(
        &gl->allocator, sizeof(nugl__command_t) * NUGL__MAX_COMMAND_COUNT);
    data->cmds.count = 0;

    pass->gl.id = passid;

    return NU_ERROR_NONE;
}
static nu_error_t
nugl__delete_renderpass (nu_renderer_t *ctx, nu_renderpass_t *pass)
{
    return NU_ERROR_NONE;
}
static void
nugl__draw (nu_renderer_t       *ctx,
            nu_renderpass_t     *pass,
            const nu_mesh_t     *mesh,
            const nu_material_t *material,
            const nu_mat4_t     *transform)
{
    nugl__context_t *gl = nugl__ctx(ctx);

    nugl__renderpass_data_t *data = &gl->passes[pass->gl.id];
    // TODO: check command validity
    nugl__command_t *cmd = &data->cmds.commands[data->cmds.count++];
    cmd->type            = NUGL__DRAW;
    cmd->transform       = *transform;
    cmd->vao             = mesh->gl.vao;
    cmd->vbo             = mesh->gl.vbo;
    cmd->vcount          = mesh->gl.vertex_count;
    cmd->texture0        = material->gl.texture0;
    cmd->texture1        = material->gl.texture1;
    cmd->uv_transform    = material->gl.uv_transform;
}
static GLuint
nugl__find_or_create_framebuffer (nu_renderer_t *ctx,
                                  GLuint         color,
                                  GLuint         depth)
{
    nugl__context_t *gl = nugl__ctx(ctx);
    for (nu_size_t i = 0; i < gl->target_count; ++i)
    {
        const nugl__rendertarget_t *target = gl->targets + i;
        if (target->color == color && target->depth == depth)
        {
            return target->fbo;
        }
    }

    NU_INFO(&ctx->_logger,
            "new framebuffer created for color: %d depth: %d",
            color,
            depth);

    nugl__rendertarget_t *target = &gl->targets[gl->target_count++];
    target->color                = color;
    target->depth                = depth;

    glGenFramebuffers(1, &target->fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, target->fbo);
    if (target->color)
    {
        glFramebufferTexture2D(GL_FRAMEBUFFER,
                               GL_COLOR_ATTACHMENT0,
                               GL_TEXTURE_2D,
                               target->color,
                               0);
    }
    if (target->depth)
    {
        glFramebufferTexture2D(GL_FRAMEBUFFER,
                               GL_DEPTH_STENCIL_ATTACHMENT,
                               GL_TEXTURE_2D,
                               target->depth,
                               0);
    }
    NU_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER)
              == GL_FRAMEBUFFER_COMPLETE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return target->fbo;
}
static void
nugl__submit_renderpass (nu_renderer_t                *ctx,
                         nu_renderpass_t              *pass,
                         const nu_renderpass_submit_t *info)
{
    nugl__context_t *gl = nugl__ctx(ctx);

    gl->pass_order[gl->pass_order_count++] = pass->gl.id;
    gl->passes[pass->gl.id].reset          = info->reset;

    nugl__renderpass_data_t *data = &gl->passes[pass->gl.id];
    switch (data->type)
    {
        case NU_RENDERPASS_UNLIT: {
            data->vp  = info->unlit.camera->gl.vp;
            data->ivp = info->unlit.camera->gl.ivp;
        }
        break;
        case NU_RENDERPASS_FLAT: {
            if (info->flat.camera)
            {
                data->vp  = info->flat.camera->gl.vp;
                data->ivp = info->flat.camera->gl.ivp;
            }
            else
            {
                data->vp  = nu_mat4_identity();
                data->ivp = nu_mat4_identity();
            }
            data->color_target = info->flat.color_target
                                     ? info->flat.color_target->_gl.texture
                                     : 0;
            data->depth_target = info->flat.depth_target
                                     ? info->flat.depth_target->_gl.texture
                                     : 0;
            data->clear_color  = info->flat.clear_color;
            if (data->color_target || data->depth_target)
            {
                data->fbo = nugl__find_or_create_framebuffer(
                    ctx, data->color_target, data->depth_target);
                data->fbo_size = data->color_target
                                     ? info->flat.color_target->_size
                                     : info->flat.depth_target->_size;
            }
            else
            {
                data->fbo      = 0;
                data->fbo_size = NU_UVEC2_ZERO;
            }
        }
        break;
        case NU_RENDERPASS_TRANSPARENT:
            break;
    }
}

#endif

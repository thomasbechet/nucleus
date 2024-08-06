#ifndef NUGL_BACKEND_H
#define NUGL_BACKEND_H

#include <nucleus/types.h>

#if defined(NU_IMPLEMENTATION) && defined(NU_BUILD_RENDERER_GL)

#include <nucleus/math.h>
#include <nucleus/memory.h>
#include <nucleus/image.h>
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
nugl__init (void *ctx, nu_allocator_t allocator, nu_uvec2_t size)
{
    nu_error_t       error;
    nugl__context_t *gl = ctx;

    gl->allocator     = allocator;
    gl->surface_size  = size;
    gl->surface_color = 0;
    gl->target_count  = 0;

    // During init, enable debug output
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(MessageCallback, 0);

    // Compile programs
    error = nugl__compile_shader(
        gl, nugl__shader_blit_vert, nugl__shader_blit_frag, &gl->blit_program);
    NU_ERROR_CHECK(error, return error);
    error = nugl__compile_shader(
        gl, nugl__shader_flat_vert, nugl__shader_flat_frag, &gl->flat_program);
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
nugl__render (void            *ctx,
              const nu_rect_t *global_viewport,
              const nu_rect_t *viewport)
{
    nugl__context_t *gl = ctx;

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
                                GLuint model_id = glGetUniformLocation(
                                    gl->flat_program, "model");
                                glUniformMatrix4fv(
                                    model_id, 1, GL_FALSE, cmd->transform.data);
                                GLuint vp_id = glGetUniformLocation(
                                    gl->flat_program, "view_projection");
                                glUniformMatrix4fv(
                                    vp_id, 1, GL_FALSE, pass->vp.data);
                                GLuint uv_transform_id = glGetUniformLocation(
                                    gl->flat_program, "uv_transform");
                                glUniformMatrix3fv(uv_transform_id,
                                                   1,
                                                   GL_FALSE,
                                                   cmd->uv_transform.data);

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
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(viewport->x, viewport->y, viewport->w, viewport->h);
    glClearColor(25.0 / 255.0, 27.0 / 255.0, 43.0 / 255.0, 1.0);
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
nugl__create_surface_color (void *ctx)
{
    nugl__context_t *gl = ctx;

    nu_texture_t texture;
    glGenTextures(1, &texture.gl.texture);
    glBindTexture(GL_TEXTURE_2D, texture.gl.texture);
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RGB,
                 gl->surface_size.x,
                 gl->surface_size.y,
                 0,
                 GL_RGB,
                 GL_UNSIGNED_BYTE,
                 NU_NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    texture.gl.size = gl->surface_size;

    gl->surface_color = texture.gl.texture;

    return texture;
}

static nu_error_t
nugl__update_camera (void                   *ctx,
                     nu_camera_t            *camera,
                     const nu_camera_info_t *info)
{
    nugl__context_t *gl = ctx;

    nu_mat4_t view   = nu_lookat(info->eye, info->center, info->up);
    float     aspect = (float)gl->surface_size.x / (float)gl->surface_size.y;
    nu_mat4_t projection
        = nu_perspective(nu_radian(info->fov), aspect, info->near, info->far);

    camera->gl.vp = nu_mat4_mul(projection, view);

    return NU_ERROR_NONE;
}
static nu_error_t
nugl__create_camera (void                   *ctx,
                     const nu_camera_info_t *info,
                     nu_camera_t            *camera)
{
    nu_error_t error;
    error = nugl__update_camera(ctx, camera, info);
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
nugl__create_texture (void                    *ctx,
                      const nu_texture_info_t *info,
                      nu_texture_t            *texture)
{

    glGenTextures(1, &texture->gl.texture);
    glBindTexture(GL_TEXTURE_2D, texture->gl.texture);
    texture->gl.size = info->size;

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
nugl__delete_texture (void *ctx, nu_texture_t *texture)
{
    glDeleteTextures(1, &texture->gl.texture);
    return NU_ERROR_NONE;
}
static nu_error_t
nugl__write_texture (void             *ctx,
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
nugl__update_material (void                     *ctx,
                       nu_material_t            *material,
                       const nu_material_info_t *info)
{
    if (info->texture0)
    {
        material->gl.texture0 = info->texture0->gl.texture;
    }
    if (info->texture1)
    {
        material->gl.texture1 = info->texture1->gl.texture;
    }
    material->gl.uv_transform = info->uv_transform;
    return NU_ERROR_NONE;
}
static nu_error_t
nugl__create_material (void                     *ctx,
                       const nu_material_info_t *info,
                       nu_material_t            *material)
{
    nugl__update_material(ctx, material, info);
    return NU_ERROR_NONE;
}
static nu_error_t
nugl__delete_material (void *ctx, nu_material_t *material)
{
    return NU_ERROR_NONE;
}
static nu_error_t
nugl__create_renderpass (void                       *ctx,
                         const nu_renderpass_info_t *info,
                         nu_renderpass_t            *pass)
{
    nugl__context_t *gl = ctx;

    nu_u32_t                 passid = gl->pass_count++;
    nugl__renderpass_data_t *data   = &gl->passes[passid];
    data->type                      = info->type;

    // Allocate command buffer
    data->cmds.commands = nu_alloc(
        gl->allocator, sizeof(nugl__command_t) * NUGL__MAX_COMMAND_COUNT);
    data->cmds.count = 0;

    pass->gl.id = passid;

    return NU_ERROR_NONE;
}
static nu_error_t
nugl__delete_renderpass (void *ctx, nu_renderpass_t *pass)
{
    return NU_ERROR_NONE;
}
static void
nugl__draw (void                *ctx,
            nu_renderpass_t     *pass,
            const nu_mesh_t     *mesh,
            const nu_material_t *material,
            const nu_mat4_t     *transform)
{
    nugl__context_t *gl = ctx;

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
nugl__find_or_create_framebuffer (nugl__context_t *ctx,
                                  GLuint           color,
                                  GLuint           depth)
{
    for (nu_size_t i = 0; i < ctx->target_count; ++i)
    {
        const nugl__rendertarget_t *target = ctx->targets + i;
        if (target->color == color && target->depth == depth)
        {
            return target->fbo;
        }
    }

    nugl__rendertarget_t *target = &ctx->targets[ctx->target_count++];
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
nugl__submit_renderpass (void                         *ctx,
                         nu_renderpass_t              *pass,
                         const nu_renderpass_submit_t *info)
{
    nugl__context_t *gl = ctx;

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
            data->vp           = info->flat.camera->gl.vp;
            data->ivp          = info->flat.camera->gl.ivp;
            data->color_target = info->flat.color_target
                                     ? info->flat.color_target->gl.texture
                                     : 0;
            data->depth_target = info->flat.depth_target
                                     ? info->flat.depth_target->gl.texture
                                     : 0;
            data->clear_color  = info->flat.clear_color;
            if (data->color_target || data->depth_target)
            {
                data->fbo = nugl__find_or_create_framebuffer(
                    gl, data->color_target, data->depth_target);
                data->fbo_size = data->color_target
                                     ? info->flat.color_target->gl.size
                                     : info->flat.depth_target->gl.size;
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

#endif

#ifndef NUGL_RENDERER_IMPL_H
#define NUGL_RENDERER_IMPL_H

#include <nucleus/graphics/renderer.h>
#include <nucleus/gl/renderer.h>
#include <nucleus/gl/shader_data.h>

#define GLAD_GL_IMPLEMENTATION
#include <nucleus/external/glad/gl.h>

static void
nugl__reset_renderpass_typed (nugl__context_t *gl, nugl__renderpass_t *pass)
{
    switch (pass->type)
    {
        case NU_RENDERPASS_UNLIT:
        case NU_RENDERPASS_FLAT: {
            nu_vec_clear(&pass->flat.cmds);
        }
        break;
        case NU_RENDERPASS_TRANSPARENT:
        case NU_RENDERPASS_WIREFRAME:
        case NU_RENDERPASS_SKYBOX: {
            nu_vec_clear(&pass->skybox.cmds);
        }
        break;
        case NU_RENDERPASS_CANVAS: {
            nu_vec_clear(&pass->canvas.cmds);
            nu_vec_clear(&pass->canvas.blit_transfer);
            pass->canvas.depth = 0;
        }
        break;
        default:
            break;
    }
}
static void
nugl__write_canvas_buffers (nugl__context_t           *gl,
                            nugl__renderpass_canvas_t *pass)
{
    nu_size_t required_size
        = sizeof(nugl__gpu_blit_t) * pass->blit_transfer.size;
    glBindBuffer(GL_ARRAY_BUFFER, pass->blit_vbo);
    if (pass->blit_vbo_size < required_size)
    {
        pass->blit_vbo_size = required_size * 2;
        glBufferData(
            GL_ARRAY_BUFFER, pass->blit_vbo_size, NU_NULL, GL_DYNAMIC_DRAW);
    }
    glBufferSubData(GL_ARRAY_BUFFER,
                    0,
                    sizeof(nugl__gpu_blit_t) * pass->blit_transfer.size,
                    pass->blit_transfer.data);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

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

static const nu_char_t *
nugl__message_type_string (GLenum type)
{
    switch (type)
    {
        case GL_DEBUG_TYPE_ERROR:
            return "ERROR";
        case GL_DEBUG_TYPE_MARKER:
            return "MARKER";
        case GL_DEBUG_TYPE_OTHER:
            return "OTHER";
        case GL_DEBUG_TYPE_PERFORMANCE:
            return "PERFORMANCE";
        case GL_DEBUG_TYPE_PORTABILITY:
            return "PORTABILITY";
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
            return "UNDEFINED_BEHAVIOR";
    }
    return "";
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
    if (type == GL_DEBUG_TYPE_OTHER) // Skip other messages
    {
        return;
    }
    switch (severity)
    {
        case GL_DEBUG_SEVERITY_HIGH:
            NU_ERROR(&ctx->_logger,
                     "GL: %s, message = %s",
                     nugl__message_type_string(type),
                     message);
            break;
        case GL_DEBUG_SEVERITY_MEDIUM:
            NU_ERROR(&ctx->_logger,
                     "GL: %s, message = %s",
                     nugl__message_type_string(type),
                     message);
            break;
        case GL_DEBUG_SEVERITY_LOW:
            NU_WARNING(&ctx->_logger,
                       "GL: %s, message = %s",
                       nugl__message_type_string(type),
                       message);
            break;
        case GL_DEBUG_SEVERITY_NOTIFICATION:
            NU_INFO(&ctx->_logger,
                    "GL: %s, message = %s",
                    nugl__message_type_string(type),
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

    gl->allocator = *allocator; // TODO: who is handles the allocator ?

    // Initialize containers
    nu_vec_init(&gl->cameras, allocator, 16);
    nu_vec_init(&gl->meshes, allocator, 16);
    nu_vec_init(&gl->textures, allocator, 16);
    nu_vec_init(&gl->cubemaps, allocator, 16);
    nu_vec_init(&gl->materials, allocator, 16);
    nu_vec_init(&gl->targets, allocator, 16);
    nu_vec_init(&gl->passes, allocator, 16);
    nu_vec_init(&gl->passes_order, allocator, 16);

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

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    error = nugl__compile_shader(
        ctx, nugl__shader_flat_vert, nugl__shader_flat_frag, &gl->flat_program);
    NU_ERROR_CHECK(error, return error);

    error = nugl__compile_shader(ctx,
                                 nugl__shader_skybox_vert,
                                 nugl__shader_skybox_frag,
                                 &gl->skybox_program);
    NU_ERROR_CHECK(error, return error);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);
    error = nugl__compile_shader(ctx,
                                 nugl__shader_canvas_blit_vert,
                                 nugl__shader_canvas_blit_frag,
                                 &gl->canvas_blit_program);
    NU_ERROR_CHECK(error, return error);

    // Create nearest sampler
    glGenSamplers(1, &gl->nearest_sampler);
    // glSamplerParameteri(gl->nearest_sampler, GL_TEXTURE_WRAP_S, GL_REPEAT);
    // glSamplerParameteri(gl->nearest_sampler, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glSamplerParameteri(gl->nearest_sampler, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glSamplerParameteri(gl->nearest_sampler, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    return NU_ERROR_NONE;
}
static nu_error_t
nugl__render (nu_renderer_t   *ctx,
              const nu_rect_t *global_viewport,
              const nu_rect_t *viewport)
{
    nugl__context_t *gl = nugl__ctx(ctx);

    for (nu_u32_t i = 0; i < gl->passes_order.size; ++i)
    {
        nu_u32_t            pass_index = gl->passes_order.data[i];
        nugl__renderpass_t *pass       = &gl->passes.data[pass_index];
        switch (pass->type)
        {
            case NU_RENDERPASS_FLAT: {
                nugl__camera_t *cam = gl->cameras.data + pass->flat.camera;

                // Prepare pass
                {
                    // Bind program
                    glUseProgram(gl->flat_program);

                    // Bind surface
                    glBindFramebuffer(GL_FRAMEBUFFER, pass->fbo);
                    glViewport(0, 0, pass->fbo_size.x, pass->fbo_size.y);

                    glEnable(GL_DEPTH_TEST);
                    glEnable(GL_CULL_FACE);
                    glDepthMask(GL_TRUE);
                    glDepthFunc(GL_LESS);
                    glFrontFace(GL_CCW);
                    glCullFace(GL_BACK);
                }

                // Clear color
                if (pass->flat.has_clear_color)
                {
                    nu_vec4_t clear_color
                        = nu_color_to_vec4(pass->flat.clear_color);
                    glClearColor(clear_color.x,
                                 clear_color.y,
                                 clear_color.z,
                                 clear_color.w);
                    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                }

                // Execute commands
                {
                    const nugl__mesh_command_vec_t *cmds = &pass->flat.cmds;
                    for (nu_size_t i = 0; i < cmds->size; ++i)
                    {
                        const nugl__mesh_command_t *cmd = &cmds->data[i];
                        switch (cmds->data[i].type)
                        {
                            case NUGL__DRAW: {
                                glUniform2uiv(
                                    glGetUniformLocation(gl->flat_program,
                                                         "viewport_size"),
                                    1,
                                    pass->fbo_size.xy);

                                glUniformMatrix4fv(
                                    glGetUniformLocation(gl->flat_program,
                                                         "model"),
                                    1,
                                    GL_FALSE,
                                    cmd->transform.data);

                                glUniformMatrix4fv(
                                    glGetUniformLocation(gl->flat_program,
                                                         "view_projection"),
                                    1,
                                    GL_FALSE,
                                    cam->vp.data);

                                glUniformMatrix3fv(
                                    glGetUniformLocation(gl->flat_program,
                                                         "uv_transform"),
                                    1,
                                    GL_FALSE,
                                    cmd->uv_transform.data);

                                // glActiveTexture(GL_TEXTURE0);
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
            case NU_RENDERPASS_SKYBOX: {
                nugl__camera_t *cam = gl->cameras.data + pass->skybox.camera;

                // Bind program
                glUseProgram(gl->skybox_program);

                // Bind surface
                glBindFramebuffer(GL_FRAMEBUFFER, pass->fbo);
                glViewport(0, 0, pass->fbo_size.x, pass->fbo_size.y);

                glEnable(GL_DEPTH_TEST);
                glDisable(GL_CULL_FACE);
                glDepthMask(GL_FALSE);
                glDepthFunc(GL_LEQUAL);

                glUniformMatrix4fv(
                    glGetUniformLocation(gl->skybox_program, "projection"),
                    1,
                    GL_FALSE,
                    cam->projection.data);
                glUniformMatrix4fv(
                    glGetUniformLocation(gl->skybox_program, "view"),
                    1,
                    GL_FALSE,
                    cam->view.data);
                glUniformMatrix3fv(
                    glGetUniformLocation(gl->skybox_program, "rotation"),
                    1,
                    GL_FALSE,
                    pass->skybox.rotation.data);

                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_CUBE_MAP, pass->skybox.cubemap);

                glDrawArrays(GL_TRIANGLES, 0, 3);

                glUseProgram(0);
            }
            break;
            case NU_RENDERPASS_CANVAS: {
                // Update buffers
                nugl__write_canvas_buffers(gl, &pass->canvas);
                glUseProgram(gl->canvas_blit_program);

                glDisable(GL_DEPTH_TEST);
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

                glUniform2uiv(glGetUniformLocation(gl->canvas_blit_program,
                                                   "viewport_size"),
                              1,
                              pass->fbo_size.xy);
                glBindVertexArray(pass->canvas.blit_vao);
                for (nu_size_t c = 0; c < pass->canvas.cmds.size; ++c)
                {
                    nugl__canvas_command_t *cmd = pass->canvas.cmds.data + c;
                    switch (cmd->type)
                    {
                        case NUGL__CANVAS_BLIT: {
                            glActiveTexture(GL_TEXTURE0);
                            glBindTexture(GL_TEXTURE_2D, cmd->blit.texture);
                            glDrawArraysInstancedBaseInstance(
                                GL_TRIANGLES,
                                0,
                                6,
                                cmd->blit.instance_count,
                                cmd->blit.instance_start);
                        }
                        break;
                    }
                }
                glBindVertexArray(0);
                glDisable(GL_BLEND);
                glUseProgram(0);
            }
            break;
            default:
                break;
        }

        // Reset pass
        if (pass->reset_after_submit)
        {
            nugl__reset_renderpass_typed(gl, pass);
        }
    }

    // Blit surface
    nu_vec4_t clear
        = nu_color_to_vec4(nu_color_to_linear(nu_color(25, 27, 43, 255)));
    glUseProgram(gl->blit_program);
    glEnable(GL_FRAMEBUFFER_SRGB);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(viewport->p.x, viewport->p.y, viewport->s.x, viewport->s.y);
    glClearColor(clear.x, clear.y, clear.z, clear.w);
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    glBindTexture(GL_TEXTURE_2D,
                  gl->textures.data[gl->surface_color_index].texture);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glUseProgram(0);

    // Reset for next frame
    nu_vec_clear(&gl->passes_order);

    return NU_ERROR_NONE;
}
static nu_texture_handle_t
nugl__create_surface_color (nu_renderer_t *ctx, nu_uvec2_t size)
{
    nugl__context_t *gl = nugl__ctx(ctx);

    nu_vec_push(&gl->textures, &gl->allocator);
    gl->surface_color_index = gl->textures.size - 1;
    nugl__texture_t *ptex   = nu_vec_last(&gl->textures);
    ptex->size              = size;

    glGenTextures(1, &ptex->texture);
    glBindTexture(GL_TEXTURE_2D, ptex->texture);
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_SRGB,
                 size.x,
                 size.y,
                 0,
                 GL_RGB,
                 GL_UNSIGNED_BYTE,
                 NU_NULL);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    nu_texture_handle_t handle;
    handle._gl.index = gl->surface_color_index;
    return handle;
}

static nu_error_t
nugl__update_camera (nu_renderer_t          *ctx,
                     nu_camera_handle_t      camera,
                     const nu_camera_info_t *info)
{
    nugl__context_t *gl = nugl__ctx(ctx);

    nugl__texture_t *surface_color
        = gl->textures.data + gl->surface_color_index;

    nugl__camera_t *pcam = gl->cameras.data + camera._gl.index;
    nu_mat4_t       view = nu_lookat(info->eye, info->center, info->up);
    float aspect = (float)surface_color->size.x / (float)surface_color->size.y;
    nu_mat4_t projection
        = nu_perspective(nu_radian(info->fov), aspect, info->near, info->far);

    pcam->view       = view;
    pcam->projection = projection;
    pcam->vp         = nu_mat4_mul(projection, view);

    return NU_ERROR_NONE;
}
static nu_error_t
nugl__create_camera (nu_renderer_t          *ctx,
                     const nu_camera_info_t *info,
                     nu_camera_handle_t     *camera)
{
    nugl__context_t *gl = nugl__ctx(ctx);

    nu_vec_push(&gl->cameras, &gl->allocator);
    camera->_gl.index = gl->cameras.size - 1;

    nu_error_t error = nugl__update_camera(ctx, *camera, info);
    NU_ERROR_CHECK(error, return error);
    return NU_ERROR_NONE;
}
static nu_error_t
nugl__delete_camera (nu_renderer_t *ctx, nu_camera_handle_t camera)
{
    return NU_ERROR_NONE;
}
static nu_error_t
nugl__create_mesh (nu_renderer_t        *ctx,
                   const nu_mesh_info_t *info,
                   nu_mesh_handle_t     *mesh)
{
    NU_ASSERT(info->positions);

    nugl__context_t *gl = nugl__ctx(ctx);

    nu_vec_push(&gl->meshes, &gl->allocator);
    mesh->_gl.index     = gl->meshes.size - 1;
    nugl__mesh_t *pmesh = nu_vec_last(&gl->meshes);

    pmesh->vertex_count = info->count;

    // Create VAO
    glGenVertexArrays(1, &pmesh->vao);
    glBindVertexArray(pmesh->vao);

    // Create VBO
    glGenBuffers(1, &pmesh->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, pmesh->vbo);

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
    glVertexAttribPointer(
        0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * NUGL__VERTEX_SIZE, (void *)0);
    glEnableVertexAttribArray(0);
    // UV
    glVertexAttribPointer(1,
                          2,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(float) * NUGL__VERTEX_SIZE,
                          (void *)(sizeof(float) * NU_VEC3_SIZE));
    glEnableVertexAttribArray(1);
    // Normal
    glVertexAttribPointer(
        2,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(float) * NUGL__VERTEX_SIZE,
        (void *)(sizeof(float) * (NU_VEC3_SIZE + NU_VEC2_SIZE)));
    glEnableVertexAttribArray(2);

    // Unbind buffers
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return NU_ERROR_NONE;
}
static nu_error_t
nugl__delete_mesh (nu_renderer_t *ctx, nu_mesh_handle_t mesh)
{
    nugl__mesh_t *pmesh = nugl__ctx(ctx)->meshes.data + mesh._gl.index;
    glDeleteVertexArrays(1, &pmesh->vao);
    glDeleteBuffers(1, &pmesh->vbo);
    return NU_ERROR_NONE;
}
static nu_error_t
nugl__create_texture (nu_renderer_t           *ctx,
                      const nu_texture_info_t *info,
                      nu_texture_handle_t     *texture)
{
    nugl__context_t *gl = nugl__ctx(ctx);

    nu_vec_push(&gl->textures, &gl->allocator);
    texture->_gl.index    = gl->textures.size - 1;
    nugl__texture_t *ptex = nu_vec_last(&gl->textures);

    ptex->size = info->size;

    glGenTextures(1, &ptex->texture);
    glBindTexture(GL_TEXTURE_2D, ptex->texture);

    switch (info->format)
    {
        case NU_TEXTURE_FORMAT_COLOR:
            glTexImage2D(GL_TEXTURE_2D,
                         0,
                         GL_RGBA,
                         info->size.x,
                         info->size.y,
                         0,
                         GL_RGBA,
                         GL_UNSIGNED_BYTE,
                         info->colors);
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
nugl__delete_texture (nu_renderer_t *ctx, nu_texture_handle_t texture)
{
    nugl__texture_t *ptex = nugl__ctx(ctx)->textures.data + texture._gl.index;
    glDeleteTextures(1, &ptex->texture);
    return NU_ERROR_NONE;
}
static nu_error_t
nugl__create_cubemap (nu_renderer_t           *ctx,
                      const nu_cubemap_info_t *info,
                      nu_cubemap_handle_t     *cubemap)
{
    nugl__context_t *gl = nugl__ctx(ctx);

    nu_vec_push(&gl->cubemaps, &gl->allocator);
    cubemap->_gl.index     = gl->cubemaps.size - 1;
    nugl__cubemap_t *pcube = nu_vec_last(&gl->cubemaps);

    glGenTextures(1, &pcube->texture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, pcube->texture);
    const nu_color_t *colors[] = {
        info->colors_posx, info->colors_negx, info->colors_posy,
        info->colors_negy, info->colors_posz, info->colors_negz,
    };
    for (nu_size_t i = 0; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                     0,
                     GL_RGBA,
                     info->size,
                     info->size,
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

    return NU_ERROR_NONE;
}
static nu_error_t
nugl__delete_cubemap (nu_renderer_t *ctx, nu_cubemap_handle_t cubemap)
{
    return NU_ERROR_NONE;
}
static nu_error_t
nugl__update_material (nu_renderer_t            *ctx,
                       nu_material_handle_t      material,
                       const nu_material_info_t *info)
{
    nugl__context_t  *gl  = nugl__ctx(ctx);
    nugl__material_t *mat = gl->materials.data + material._gl.index;
    NU_ASSERT(info->type == mat->type);
    NU_ASSERT(material._gl.index < gl->materials.size);
    switch (mat->type)
    {
        case NU_MATERIAL_MESH: {
            if (info->mesh.color0)
            {
                mat->mesh.texture0
                    = gl->textures.data[info->mesh.color0->_gl.index].texture;
            }
            if (info->mesh.color1)
            {
                mat->mesh.texture1
                    = gl->textures.data[info->mesh.color1->_gl.index].texture;
            }
            mat->mesh.uv_transform = info->mesh.uv_transform;
        }
        break;
        case NU_MATERIAL_CANVAS: {
            if (info->canvas.color0)
            {
                mat->canvas.texture0
                    = gl->textures.data[info->canvas.color0->_gl.index].texture;
            }
            mat->canvas.wrap_mode = info->canvas.wrap_mode;
        }
        break;
    }
    return NU_ERROR_NONE;
}
static nu_error_t
nugl__create_material (nu_renderer_t            *ctx,
                       const nu_material_info_t *info,
                       nu_material_handle_t     *material)
{
    nugl__context_t *gl = nugl__ctx(ctx);

    nu_vec_push(&gl->materials, &gl->allocator);
    material->_gl.index = gl->materials.size - 1;

    // Keep material type
    (gl->materials.data + material->_gl.index)->type = info->type;

    nugl__update_material(ctx, *material, info);
    return NU_ERROR_NONE;
}
static nu_error_t
nugl__delete_material (nu_renderer_t *ctx, nu_material_handle_t material)
{
    return NU_ERROR_NONE;
}
static nu_error_t
nugl__create_flat_renderpass (nugl__context_t         *gl,
                              nugl__renderpass_flat_t *pass)
{
    nu_vec_init(&pass->cmds, &gl->allocator, 128);
    return NU_ERROR_NONE;
}
static nu_error_t
nugl__create_canvas_renderpass (nugl__context_t           *gl,
                                nugl__renderpass_canvas_t *pass)
{
    nu_vec_init(&pass->cmds, &gl->allocator, 128);
    nu_vec_init(&pass->blit_transfer, &gl->allocator, 32);

    // Create VAO
    GLuint vao;
    glGenVertexArrays(1, &vao);

    // Create VBO
    GLuint vbo;
    glGenBuffers(1, &vbo);

    // Configure VAO
    glBindVertexArray(vao);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);

    glVertexAttribDivisor(0, 1);
    glVertexAttribDivisor(1, 1);
    glVertexAttribDivisor(2, 1);
    glVertexAttribDivisor(3, 1);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribIPointer(0,
                           1,
                           GL_UNSIGNED_INT,
                           sizeof(nugl__gpu_blit_t),
                           (void *)(offsetof(nugl__gpu_blit_t, pos)));
    glVertexAttribIPointer(1,
                           1,
                           GL_UNSIGNED_INT,
                           sizeof(nugl__gpu_blit_t),
                           (void *)(offsetof(nugl__gpu_blit_t, tex)));
    glVertexAttribIPointer(2,
                           1,
                           GL_UNSIGNED_INT,
                           sizeof(nugl__gpu_blit_t),
                           (void *)(offsetof(nugl__gpu_blit_t, size)));
    glVertexAttribPointer(3,
                          1,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(nugl__gpu_blit_t),
                          (void *)(offsetof(nugl__gpu_blit_t, depth)));
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);

    pass->blit_vbo      = vbo;
    pass->blit_vbo_size = 0;
    pass->blit_vao      = vao;
    pass->depth         = 0;

    return NU_ERROR_NONE;
}
static nu_error_t
nugl__create_renderpass (nu_renderer_t              *ctx,
                         const nu_renderpass_info_t *info,
                         nu_renderpass_handle_t     *pass)
{
    nu_error_t       error;
    nugl__context_t *gl = nugl__ctx(ctx);

    nu_vec_push(&gl->passes, &gl->allocator);
    nu_u32_t            index = gl->passes.size - 1;
    nugl__renderpass_t *data  = gl->passes.data + index;
    data->type                = info->type;
    data->reset_after_submit  = info->reset_after_submit;

    // Allocate command buffer
    switch (data->type)
    {
        case NU_RENDERPASS_UNLIT:
            break;
        case NU_RENDERPASS_FLAT: {
            error = nugl__create_flat_renderpass(gl, &data->flat);
            NU_ERROR_CHECK(error, return error);
        }
        break;
        case NU_RENDERPASS_TRANSPARENT:
            break;
        case NU_RENDERPASS_WIREFRAME:
            break;
        case NU_RENDERPASS_SKYBOX:
            break;
        case NU_RENDERPASS_CANVAS: {
            error = nugl__create_canvas_renderpass(gl, &data->canvas);
            NU_ERROR_CHECK(error, return error);
        }
        break;
    }

    pass->_gl.index = index;

    return NU_ERROR_NONE;
}
static nu_error_t
nugl__delete_renderpass (nu_renderer_t *ctx, nu_renderpass_handle_t pass)
{
    return NU_ERROR_NONE;
}

#define NUGL__MIN_DEPTH       0.0
#define NUGL__MAX_DEPTH       1000.0
#define NUGL__DEPTH_INCREMENT 0.05
static void
nugl__canvas_add_blit (nugl__context_t           *gl,
                       nugl__renderpass_canvas_t *pass,
                       nu_ivec2_t                 pos,
                       nu_uvec2_t                 tex,
                       nu_uvec2_t                 size)
{
    nu_vec_push(&pass->blit_transfer, &gl->allocator);
    nugl__gpu_blit_t *blit = nu_vec_last(&pass->blit_transfer);
    blit->pos              = ((nu_u32_t)pos.y << 16) | (nu_u32_t)pos.x;
    blit->tex              = (tex.y << 16) | tex.x;
    blit->size             = (size.y << 16) | size.x;
    blit->depth
        = (pass->depth - NUGL__MIN_DEPTH) / (NUGL__MAX_DEPTH - NUGL__MIN_DEPTH);
}
static void
nugl__canvas_blit_rect (nugl__context_t           *gl,
                        nugl__renderpass_canvas_t *pass,
                        GLuint                     texture,
                        nu_rect_t                  extent,
                        nu_rect_t                  tex_extent,
                        nu_texture_wrap_mode_t     wrap_mode)
{
    nu_u32_t blit_count = 0;
    switch (wrap_mode)
    {
        case NU_TEXTURE_WRAP_CLAMP: {
            nugl__canvas_add_blit(gl,
                                  pass,
                                  extent.p,
                                  nu_uvec2(tex_extent.p.x, tex_extent.p.y),
                                  nu_uvec2_min(extent.s, tex_extent.s));
            blit_count = 1;
        }
        break;
        case NU_TEXTURE_WRAP_REPEAT: {
            // Calculate blit count
            nu_size_t full_hblit_count = extent.s.x / tex_extent.s.x;
            nu_size_t full_vblit_count = extent.s.y / tex_extent.s.y;
            blit_count                 = full_hblit_count * full_vblit_count;

            // Insert full blits
            for (nu_size_t y = 0; y < full_vblit_count; ++y)
            {
                for (nu_size_t x = 0; x < full_hblit_count; ++x)
                {
                    nu_i32_t pos_x = extent.p.x + (x * tex_extent.s.x);
                    nu_i32_t pos_y = extent.p.y + (y * tex_extent.s.y);
                    nugl__canvas_add_blit(
                        gl,
                        pass,
                        nu_ivec2(pos_x, pos_y),
                        nu_uvec2(tex_extent.p.x, tex_extent.p.y),
                        tex_extent.s);
                }
            }

            // Insert partial blits
            nu_size_t partial_hblit_size = extent.s.x % tex_extent.s.x;
            nu_size_t partial_vblit_size = extent.s.y % tex_extent.s.y;

            if (partial_hblit_size)
            {
                for (nu_size_t y = 0; y < full_vblit_count; ++y)
                {
                    nu_i32_t pos_x
                        = extent.p.x + (full_hblit_count * tex_extent.s.x);
                    nu_i32_t   pos_y = extent.p.y + (y * tex_extent.s.y);
                    nu_uvec2_t size
                        = nu_uvec2(partial_hblit_size, tex_extent.s.y);
                    nugl__canvas_add_blit(
                        gl,
                        pass,
                        nu_ivec2(pos_x, pos_y),
                        nu_uvec2(tex_extent.p.x, tex_extent.p.y),
                        size);
                    ++blit_count;
                }
            }
            if (partial_vblit_size)
            {
                for (nu_size_t x = 0; x < full_hblit_count; ++x)
                {
                    nu_i32_t pos_x = extent.p.x + (x * tex_extent.s.x);
                    nu_i32_t pos_y
                        = extent.p.y + (full_vblit_count * tex_extent.s.y);
                    nu_uvec2_t size
                        = nu_uvec2(tex_extent.s.x, partial_vblit_size);
                    nugl__canvas_add_blit(
                        gl,
                        pass,
                        nu_ivec2(pos_x, pos_y),
                        nu_uvec2(tex_extent.p.x, tex_extent.p.y),
                        size);
                    ++blit_count;
                }
            }
            if (partial_hblit_size && partial_vblit_size)
            {
                nu_i32_t pos_x
                    = extent.p.x + (full_hblit_count * tex_extent.s.x);
                nu_i32_t pos_y
                    = extent.p.y + (full_vblit_count * tex_extent.s.y);
                nu_uvec2_t size
                    = nu_uvec2(partial_hblit_size, partial_vblit_size);
                nugl__canvas_add_blit(gl,
                                      pass,
                                      nu_ivec2(pos_x, pos_y),
                                      nu_uvec2(tex_extent.p.x, tex_extent.p.y),
                                      size);
                ++blit_count;
            }
        }
        break;
        case NU_TEXTURE_WRAP_MIRROR:
            break;
    }
    pass->depth += NUGL__DEPTH_INCREMENT;

    nugl__canvas_command_t *last = nu_vec_last(&pass->cmds);
    if (last && last->type == NUGL__CANVAS_BLIT
        && last->blit.texture == texture)
    {
        last->blit.instance_count += blit_count;
    }
    else
    {
        nu_vec_push(&pass->cmds, &gl->allocator);
        last                      = nu_vec_last(&pass->cmds);
        last->type                = NUGL__CANVAS_BLIT;
        last->blit.texture        = texture;
        last->blit.instance_start = pass->blit_transfer.size - blit_count;
        last->blit.instance_count = blit_count;
    }
}

static GLuint
nugl__find_or_create_framebuffer (nu_renderer_t *ctx,
                                  GLuint         color,
                                  GLuint         depth)
{
    nugl__context_t *gl = nugl__ctx(ctx);
    for (nu_size_t i = 0; i < gl->targets.size; ++i)
    {
        const nugl__rendertarget_t *target = gl->targets.data + i;
        if (target->color == color && target->depth == depth)
        {
            return target->fbo;
        }
    }

    NU_INFO(&ctx->_logger,
            "new framebuffer created for color: %d depth: %d",
            color,
            depth);

    nu_vec_push(&gl->targets, &gl->allocator);
    nugl__rendertarget_t *target = nu_vec_last(&gl->targets);
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
nugl__prepare_color_depth (nu_renderer_t         *ctx,
                           nugl__renderpass_t    *pass,
                           const nugl__texture_t *color_target,
                           const nugl__texture_t *depth_target)
{
    pass->color_target = color_target ? color_target->texture : 0;
    pass->depth_target = depth_target ? depth_target->texture : 0;
    if (color_target || depth_target)
    {
        pass->fbo = nugl__find_or_create_framebuffer(
            ctx, pass->color_target, pass->depth_target);
        pass->fbo_size
            = pass->color_target ? color_target->size : depth_target->size;
    }
    else
    {
        pass->fbo      = 0;
        pass->fbo_size = NU_UVEC2_ZERO;
    }
}
static void
nugl__submit_renderpass (nu_renderer_t                *ctx,
                         nu_renderpass_handle_t        pass,
                         const nu_renderpass_submit_t *info)
{
    nugl__context_t *gl = nugl__ctx(ctx);

    nu_vec_push(&gl->passes_order, &gl->allocator);
    *(nu_vec_last(&gl->passes_order)) = pass._gl.index;

    nugl__renderpass_t *ppass = gl->passes.data + pass._gl.index;
    switch (ppass->type)
    {
        case NU_RENDERPASS_FLAT: {
            const nugl__texture_t *color_target
                = info->flat.color_target
                      ? gl->textures.data + info->flat.color_target->_gl.index
                      : NU_NULL;
            const nugl__texture_t *depth_target
                = info->flat.depth_target
                      ? gl->textures.data + info->flat.depth_target->_gl.index
                      : NU_NULL;

            if (info->flat.clear_color)
            {
                ppass->flat.clear_color     = *info->flat.clear_color;
                ppass->flat.has_clear_color = NU_TRUE;
            }
            else
            {
                ppass->flat.has_clear_color = NU_FALSE;
            }

            ppass->flat.camera = info->flat.camera._gl.index;
            nugl__prepare_color_depth(ctx, ppass, color_target, depth_target);
        }
        break;
        case NU_RENDERPASS_SKYBOX: {
            const nugl__texture_t *color_target
                = info->skybox.color_target
                      ? gl->textures.data + info->skybox.color_target->_gl.index
                      : NU_NULL;
            const nugl__texture_t *depth_target
                = info->skybox.depth_target
                      ? gl->textures.data + info->skybox.depth_target->_gl.index
                      : NU_NULL;

            ppass->skybox.camera = info->skybox.camera._gl.index;
            nugl__prepare_color_depth(ctx, ppass, color_target, depth_target);

            ppass->skybox.cubemap
                = gl->cubemaps.data[info->skybox.cubemap._gl.index].texture;
            ppass->skybox.rotation = nu_quat_mat3(info->skybox.rotation);
        }
        break;
        case NU_RENDERPASS_CANVAS: {
            const nugl__texture_t *color_target
                = info->canvas.color_target
                      ? gl->textures.data + info->canvas.color_target->_gl.index
                      : NU_NULL;
            nugl__prepare_color_depth(ctx, ppass, color_target, NU_NULL);
        }
        break;
        default:
            break;
    }
}
static void
nugl__reset_renderpass (nu_renderer_t *ctx, nu_renderpass_handle_t pass)
{
    nugl__context_t    *gl    = nugl__ctx(ctx);
    nugl__renderpass_t *ppass = gl->passes.data + pass._gl.index;
    nugl__reset_renderpass_typed(gl, ppass);
}
static void
nugl__draw (nu_renderer_t         *ctx,
            nu_renderpass_handle_t pass,
            nu_material_handle_t   material,
            nu_mesh_handle_t       mesh,
            nu_mat4_t              transform)
{
    nugl__context_t *gl = nugl__ctx(ctx);

    nugl__renderpass_t *ppass = gl->passes.data + pass._gl.index;
    nugl__mesh_t       *pmesh = gl->meshes.data + mesh._gl.index;
    nugl__material_t   *pmat  = gl->materials.data + material._gl.index;
    NU_ASSERT(pmat->type == NU_MATERIAL_MESH);
    // TODO: check command validity
    switch (ppass->type)
    {
        case NU_RENDERPASS_FLAT: {
            nu_vec_push(&ppass->flat.cmds, &gl->allocator);
            nugl__mesh_command_t *cmd = nu_vec_last(&ppass->flat.cmds);
            cmd->type                 = NUGL__DRAW;
            cmd->transform            = transform;
            cmd->vao                  = pmesh->vao;
            cmd->vbo                  = pmesh->vbo;
            cmd->vcount               = pmesh->vertex_count;
            cmd->texture0             = pmat->mesh.texture0;
            cmd->texture1             = pmat->mesh.texture1;
            cmd->uv_transform         = pmat->mesh.uv_transform;
        }
        break;
        default:
            return;
    }
}
static void
nugl__blit (nu_renderer_t         *ctx,
            nu_renderpass_handle_t pass,
            nu_material_handle_t   material,
            nu_rect_t              extent,
            nu_rect_t              tex_extent)
{
    nugl__context_t *gl = nugl__ctx(ctx);

    nugl__renderpass_t *ppass = gl->passes.data + pass._gl.index;
    nugl__material_t   *pmat  = gl->materials.data + material._gl.index;
    NU_ASSERT(pmat->type == NU_MATERIAL_CANVAS);

    switch (ppass->type)
    {
        case NU_RENDERPASS_CANVAS: {
            nugl__canvas_blit_rect(gl,
                                   &ppass->canvas,
                                   pmat->canvas.texture0,
                                   extent,
                                   tex_extent,
                                   pmat->canvas.wrap_mode);
        }
        break;
        default:
            break;
    }
}

#endif
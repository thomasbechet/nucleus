#ifndef NU_GL_IMPL_H
#define NU_GL_IMPL_H

#include <nucleus/internal.h>

static void
nugl__reset_renderpass_typed (nugl__renderpass_t *pass)
{
    switch (pass->type)
    {
        case NU_RENDERPASS_FLAT: {
            NU_VEC_CLEAR(&pass->flat.cmds);
        }
        break;
        case NU_RENDERPASS_SKYBOX: {
            NU_VEC_CLEAR(&pass->skybox.cmds);
        }
        break;
        case NU_RENDERPASS_CANVAS: {
            NU_VEC_CLEAR(&pass->canvas.cmds);
            NU_VEC_CLEAR(&pass->canvas.blit_transfer);
            pass->canvas.depth = 0;
        }
        break;
        default:
            break;
    }
}
static void
nugl__write_canvas_buffers (nugl__renderpass_canvas_t *pass)
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
nugl__compile_shader (const nu_char_t *vert,
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
        nu_char_t *log = (nu_char_t *)malloc(sizeof(nu_char_t) * max_length);
        glGetShaderInfoLog(vertex_shader, max_length, &max_length, log);
        NU_ERROR("%s", log);

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
        nu_char_t *log = (nu_char_t *)malloc(sizeof(nu_char_t) * max_length);
        glGetShaderInfoLog(fragment_shader, max_length, &max_length, log);
        NU_ERROR("%s", log);

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
    if (type == GL_DEBUG_TYPE_OTHER) // Skip other messages
    {
        return;
    }
    switch (severity)
    {
        case GL_DEBUG_SEVERITY_HIGH:
            NU_ERROR("GL: %s, message = %s",
                     nugl__message_type_string(type),
                     message);
            break;
        case GL_DEBUG_SEVERITY_MEDIUM:
            NU_ERROR("GL: %s, message = %s",
                     nugl__message_type_string(type),
                     message);
            break;
        case GL_DEBUG_SEVERITY_LOW:
            NU_WARNING("GL: %s, message = %s",
                       nugl__message_type_string(type),
                       message);
            break;
        case GL_DEBUG_SEVERITY_NOTIFICATION:
            NU_INFO("GL: %s, message = %s",
                    nugl__message_type_string(type),
                    message);
            break;
    }
}

static nu_error_t
nugl__init (void)
{
    nu_error_t error;

    nu__gl_t *gl = &_ctx.gl;

    // Initialize containers
    NU_VEC_INIT(16, &gl->cameras);
    NU_VEC_INIT(16, &gl->meshes);
    NU_VEC_INIT(16, &gl->textures);
    NU_VEC_INIT(16, &gl->cubemaps);
    NU_VEC_INIT(16, &gl->materials);
    NU_VEC_INIT(16, &gl->targets);
    NU_VEC_INIT(16, &gl->passes);
    NU_VEC_INIT(16, &gl->passes_order);

    // Initialize GL functions
    if (!gladLoadGL((GLADloadfunc)RGFW_getProcAddress))
    {
        NU_ERROR("failed to load GL functions");
        return NU_ERROR_BACKEND;
    }

    // During init, enable debug output
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(MessageCallback, NU_NULL);

    // Compile programs
    error = nugl__compile_shader(
        nugl__shader_blit_vert, nugl__shader_blit_frag, &gl->blit_program);
    NU_ERROR_CHECK(error, return error);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    error = nugl__compile_shader(
        nugl__shader_flat_vert, nugl__shader_flat_frag, &gl->flat_program);
    NU_ERROR_CHECK(error, return error);

    error = nugl__compile_shader(nugl__shader_skybox_vert,
                                 nugl__shader_skybox_frag,
                                 &gl->skybox_program);
    NU_ERROR_CHECK(error, return error);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);
    error = nugl__compile_shader(nugl__shader_canvas_blit_vert,
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
nugl__free (void)
{
    return NU_ERROR_NONE;
}
static nu_error_t
nugl__render (const nu_rect_t *global_viewport, const nu_rect_t *viewport)
{
    nu__gl_t *gl = &_ctx.gl;

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
                                    pass->fbo_size.data);

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
                nugl__camera_t  *cam = gl->cameras.data + pass->skybox.camera;
                nugl__cubemap_t *cubemap
                    = gl->cubemaps.data + NU_HANDLE_INDEX(pass->skybox.cubemap);

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
                glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap->texture);

                glDrawArrays(GL_TRIANGLES, 0, 3);

                glUseProgram(0);
            }
            break;
            case NU_RENDERPASS_CANVAS: {
                // Update buffers
                nugl__write_canvas_buffers(&pass->canvas);
                glUseProgram(gl->canvas_blit_program);

                glDisable(GL_DEPTH_TEST);
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

                glUniform2uiv(glGetUniformLocation(gl->canvas_blit_program,
                                                   "viewport_size"),
                              1,
                              pass->fbo_size.data);
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
            nugl__reset_renderpass_typed(pass);
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
    NU_VEC_CLEAR(&gl->passes_order);

    return NU_ERROR_NONE;
}
static nu_texture_t
nugl__create_surface_color (nu_uvec2_t size)
{
    nu__gl_t *gl = &_ctx.gl;

    nugl__texture_t *ptex   = NU_VEC_PUSH(&gl->textures);
    gl->surface_color_index = gl->textures.size - 1;
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
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    return NU_HANDLE_MAKE(nu_texture_t, (nu_size_t)gl->surface_color_index);
}

static nu_camera_t
nugl__camera_create (void)
{
    nu__gl_t *gl = &_ctx.gl;
    (void)NU_VEC_PUSH(&gl->cameras);
    return NU_HANDLE_MAKE(nu_camera_t, gl->cameras.size - 1);
}
static void
nugl__camera_proj (nu_camera_t camera, nu_mat4_t proj)
{
    nu__gl_t       *gl   = &_ctx.gl;
    nugl__camera_t *pcam = gl->cameras.data + NU_HANDLE_INDEX(camera);

    pcam->projection = proj;
    pcam->vp         = nu_mat4_mul(proj, pcam->view);
}
static void
nugl__camera_view (nu_camera_t camera, nu_mat4_t view)
{
    nu__gl_t       *gl   = &_ctx.gl;
    nugl__camera_t *pcam = gl->cameras.data + NU_HANDLE_INDEX(camera);

    pcam->view = view;
    pcam->vp   = nu_mat4_mul(pcam->projection, view);
}
static nu_mesh_t
nugl__mesh_create (nu_size_t count)
{
    nu__gl_t *gl = &_ctx.gl;

    nugl__mesh_t *pmesh  = NU_VEC_PUSH(&gl->meshes);
    nu_mesh_t     handle = NU_HANDLE_MAKE(nu_mesh_t, gl->meshes.size - 1);

    pmesh->vertex_count = count;

    // Create VAO
    glGenVertexArrays(1, &pmesh->vao);
    glBindVertexArray(pmesh->vao);

    // Create VBO
    glGenBuffers(1, &pmesh->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, pmesh->vbo);

    // Format vertices
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(float) * count * NUGL__VERTEX_SIZE,
                 NU_NULL,
                 GL_STATIC_DRAW);

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

    return handle;
}
static void
nugl__mesh_delete (nu_mesh_t mesh)
{
    nu__gl_t     *gl    = &_ctx.gl;
    nugl__mesh_t *pmesh = gl->meshes.data + NU_HANDLE_INDEX(mesh);
    glDeleteVertexArrays(1, &pmesh->vao);
    glDeleteBuffers(1, &pmesh->vbo);
}
static void
nugl__mesh_update (nu_mesh_t        mesh,
                   const nu_vec3_t *positions,
                   const nu_vec2_t *uvs,
                   const nu_vec3_t *normals)
{
    nugl__mesh_t *pmesh = _ctx.gl.meshes.data + NU_HANDLE_INDEX(mesh);
    glBindBuffer(GL_ARRAY_BUFFER, pmesh->vbo);
    float *ptr = (float *)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    NU_ASSERT(ptr);
    for (nu_size_t i = 0; i < pmesh->vertex_count; ++i)
    {
        if (positions)
        {
            ptr[i * NUGL__VERTEX_SIZE + 0] = positions[i].x;
            ptr[i * NUGL__VERTEX_SIZE + 1] = positions[i].y;
            ptr[i * NUGL__VERTEX_SIZE + 2] = positions[i].z;
        }
        if (uvs)
        {
            ptr[i * NUGL__VERTEX_SIZE + 3] = uvs[i].x;
            ptr[i * NUGL__VERTEX_SIZE + 4] = uvs[i].y;
        }
        if (normals)
        {
            ptr[i * NUGL__VERTEX_SIZE + 5] = normals[i].x;
            ptr[i * NUGL__VERTEX_SIZE + 6] = normals[i].y;
            ptr[i * NUGL__VERTEX_SIZE + 7] = normals[i].z;
        }
    }
    glUnmapBuffer(GL_ARRAY_BUFFER);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
static nu_texture_t
nugl__texture_create (nu_uvec2_t          size,
                      nu_texture_format_t format,
                      nu_texture_usage_t  usage,
                      const nu_color_t   *colors)
{
    nu__gl_t *gl = &_ctx.gl;

    nugl__texture_t *ptex = NU_VEC_PUSH(&gl->textures);
    nu_texture_t handle   = NU_HANDLE_MAKE(nu_texture_t, gl->textures.size - 1);

    ptex->size = size;

    glGenTextures(1, &ptex->texture);
    glBindTexture(GL_TEXTURE_2D, ptex->texture);

    switch (format)
    {
        case NU_TEXTURE_FORMAT_COLOR:
            glTexImage2D(GL_TEXTURE_2D,
                         0,
                         GL_RGBA,
                         size.x,
                         size.y,
                         0,
                         GL_RGBA,
                         GL_UNSIGNED_BYTE,
                         colors);
            break;
        case NU_TEXTURE_FORMAT_DEPTH:
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
static nu_cubemap_t
nugl__cubemap_create (nu_u32_t           size,
                      nu_texture_usage_t usage,
                      const nu_color_t **colors)
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
static void
nugl__material_color0 (nu_material_t material, nu_texture_t color0)
{
    nugl__material_t *mat = _ctx.gl.materials.data + NU_HANDLE_INDEX(material);
    switch (mat->type)
    {
        case NU_MATERIAL_MESH:
            mat->mesh.texture0 = color0;
            break;
        case NU_MATERIAL_CANVAS:
            mat->canvas.texture0 = color0;
            break;
    }
}
static void
nugl__material_color1 (nu_material_t material, nu_texture_t color1)
{
    nugl__material_t *mat = _ctx.gl.materials.data + NU_HANDLE_INDEX(material);
    switch (mat->type)
    {
        case NU_MATERIAL_MESH:
            mat->mesh.texture1 = color1;
            break;
        default:
            break;
    }
}
static void
nugl__material_uv_transform (nu_material_t material, nu_mat3_t transform)
{
    nugl__material_t *mat = _ctx.gl.materials.data + NU_HANDLE_INDEX(material);
    switch (mat->type)
    {
        case NU_MATERIAL_MESH:
            mat->mesh.uv_transform = transform;
            break;
        default:
            break;
    }
}
static void
nugl__material_wrap_mode (nu_material_t material, nu_texture_wrap_mode_t mode)
{
    nugl__material_t *mat = _ctx.gl.materials.data + NU_HANDLE_INDEX(material);
    switch (mat->type)
    {
        case NU_MATERIAL_CANVAS:
            mat->canvas.wrap_mode = mode;
            break;
        default:
            break;
    }
}
static nu_material_t
nugl__material_create (nu_material_type_t type)
{
    nu__gl_t *gl = &_ctx.gl;

    nugl__material_t *p = NU_VEC_PUSH(&gl->materials);
    nu_material_t     handle
        = NU_HANDLE_MAKE(nu_material_t, gl->materials.size - 1);

    p->type = type;
    switch (p->type)
    {
        case NU_MATERIAL_MESH:
            p->mesh.texture0     = NU_NULL;
            p->mesh.texture1     = NU_NULL;
            p->mesh.uv_transform = nu_mat3_identity();
            break;
        case NU_MATERIAL_CANVAS:
            p->canvas.texture0  = NU_NULL;
            p->canvas.wrap_mode = NU_TEXTURE_WRAP_CLAMP;
            break;
    }

    return handle;
}
static nu_error_t
nugl__renderpass_create_flat (nugl__renderpass_flat_t *pass)
{
    NU_VEC_INIT(128, &pass->cmds);
    return NU_ERROR_NONE;
}
static nu_error_t
nugl__renderpass_create_canvas (nugl__renderpass_canvas_t *pass)
{
    NU_VEC_INIT(128, &pass->cmds);
    NU_VEC_INIT(32, &pass->blit_transfer);

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
static nu_renderpass_t
nugl__renderpass_create (nu_renderpass_type_t type,
                         nu_bool_t            reset_after_submit)
{
    nu_error_t error;
    nu__gl_t  *gl = &_ctx.gl;

    nugl__renderpass_t *data  = NU_VEC_PUSH(&gl->passes);
    nu_size_t           index = gl->passes.size - 1;
    data->type                = type;
    data->reset_after_submit  = reset_after_submit;

    // Allocate command buffer
    switch (data->type)
    {
        case NU_RENDERPASS_FLAT: {
            error = nugl__renderpass_create_flat(&data->flat);
            NU_ERROR_CHECK(error, return NU_NULL);
        }
        break;
        case NU_RENDERPASS_SKYBOX:
            break;
        case NU_RENDERPASS_CANVAS: {
            error = nugl__renderpass_create_canvas(&data->canvas);
            NU_ERROR_CHECK(error, return NU_NULL);
        }
        break;
    }

    return NU_HANDLE_MAKE(nu_renderpass_t, index);
}

static void
nugl__canvas_add_blit (nugl__renderpass_canvas_t *pass,
                       nu_ivec2_t                 pos,
                       nu_uvec2_t                 tex,
                       nu_uvec2_t                 size)
{
    nugl__gpu_blit_t *blit = NU_VEC_PUSH(&pass->blit_transfer);
    blit->pos              = ((nu_u32_t)pos.y << 16) | (nu_u32_t)pos.x;
    blit->tex              = (tex.y << 16) | tex.x;
    blit->size             = (size.y << 16) | size.x;
    blit->depth
        = (pass->depth - NUGL__MIN_DEPTH) / (NUGL__MAX_DEPTH - NUGL__MIN_DEPTH);
}
static void
nugl__canvas_blit_rect (nugl__renderpass_canvas_t *pass,
                        GLuint                     texture,
                        nu_rect_t                  extent,
                        nu_rect_t                  tex_extent,
                        nu_texture_wrap_mode_t     wrap_mode)
{
    nu_u32_t blit_count = 0;
    switch (wrap_mode)
    {
        case NU_TEXTURE_WRAP_CLAMP: {
            nugl__canvas_add_blit(pass,
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
                nugl__canvas_add_blit(pass,
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

    nugl__canvas_command_t *last = NU_VEC_LAST(&pass->cmds);
    if (last && last->type == NUGL__CANVAS_BLIT
        && last->blit.texture == texture)
    {
        last->blit.instance_count += blit_count;
    }
    else
    {
        last                      = NU_VEC_PUSH(&pass->cmds);
        last->type                = NUGL__CANVAS_BLIT;
        last->blit.texture        = texture;
        last->blit.instance_start = pass->blit_transfer.size - blit_count;
        last->blit.instance_count = blit_count;
    }
}

static GLuint
nugl__find_or_create_framebuffer (GLuint color, GLuint depth)
{
    nu__gl_t *gl = &_ctx.gl;
    for (nu_size_t i = 0; i < gl->targets.size; ++i)
    {
        const nugl__rendertarget_t *target = gl->targets.data + i;
        if (target->color == color && target->depth == depth)
        {
            return target->fbo;
        }
    }

    NU_INFO("new framebuffer created for color: %d depth: %d", color, depth);

    nugl__rendertarget_t *target = NU_VEC_PUSH(&gl->targets);
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
nugl__prepare_color_depth (nugl__renderpass_t *pass,
                           nu_texture_t        color_target,
                           nu_texture_t        depth_target)
{
    nu__gl_t        *gl = &_ctx.gl;
    nugl__texture_t *color
        = color_target ? gl->textures.data + NU_HANDLE_INDEX(color_target)
                       : NU_NULL;
    nugl__texture_t *depth
        = depth_target ? gl->textures.data + NU_HANDLE_INDEX(depth_target)
                       : NU_NULL;
    if (color_target || depth_target)
    {
        pass->fbo = nugl__find_or_create_framebuffer(
            color ? color->texture : 0, depth ? depth->texture : 0);
        pass->fbo_size = color ? color->size : depth->size;
    }
    else
    {
        pass->fbo      = 0;
        pass->fbo_size = NU_UVEC2_ZERO;
    }
}
static void
nugl__renderpass_clear_color (nu_renderpass_t pass, nu_color_t *color)
{
    nu__gl_t           *gl    = &_ctx.gl;
    nugl__renderpass_t *ppass = gl->passes.data + NU_HANDLE_INDEX(pass);
    if (color)
    {
        ppass->flat.clear_color     = *color;
        ppass->flat.has_clear_color = NU_TRUE;
    }
    else
    {
        ppass->flat.has_clear_color = NU_FALSE;
    }
}
static void
nugl__renderpass_camera (nu_renderpass_t pass, nu_camera_t camera)
{
    NU_ASSERT(camera);
    nu__gl_t           *gl    = &_ctx.gl;
    nugl__renderpass_t *ppass = gl->passes.data + NU_HANDLE_INDEX(pass);
    ppass->flat.camera        = NU_HANDLE_INDEX(camera);
}
static void
nugl__renderpass_skybox_cubemap (nu_renderpass_t pass, nu_cubemap_t cubemap)
{
    nu__gl_t           *gl    = &_ctx.gl;
    nugl__renderpass_t *ppass = gl->passes.data + NU_HANDLE_INDEX(pass);
    NU_ASSERT(ppass->type == NU_RENDERPASS_SKYBOX);
    ppass->skybox.cubemap = cubemap;
}
static void
nugl__renderpass_skybox_rotation (nu_renderpass_t pass, nu_quat_t rot)
{
    nu__gl_t           *gl    = &_ctx.gl;
    nugl__renderpass_t *ppass = gl->passes.data + NU_HANDLE_INDEX(pass);
    NU_ASSERT(ppass->type == NU_RENDERPASS_SKYBOX);
    ppass->skybox.rotation = nu_quat_mat3(rot);
}
static void
nugl__renderpass_target_color (nu_renderpass_t pass, nu_texture_t color)
{
    nu__gl_t           *gl    = &_ctx.gl;
    nugl__renderpass_t *ppass = gl->passes.data + NU_HANDLE_INDEX(pass);
    ppass->color_target       = color;
}
static void
nugl__renderpass_target_depth (nu_renderpass_t pass, nu_texture_t depth)
{
    nu__gl_t           *gl    = &_ctx.gl;
    nugl__renderpass_t *ppass = gl->passes.data + NU_HANDLE_INDEX(pass);
    ppass->depth_target       = depth;
}
static void
nugl__renderpass_reset (nu_renderpass_t pass)
{
    nu__gl_t           *gl    = &_ctx.gl;
    nugl__renderpass_t *ppass = gl->passes.data + NU_HANDLE_INDEX(pass);
    nugl__reset_renderpass_typed(ppass);
}
static void
nugl__renderpass_submit (nu_renderpass_t pass)
{
    nu__gl_t *gl = &_ctx.gl;

    *NU_VEC_PUSH(&gl->passes_order) = NU_HANDLE_INDEX(pass);

    nugl__renderpass_t *ppass = gl->passes.data + NU_HANDLE_INDEX(pass);
    switch (ppass->type)
    {
        case NU_RENDERPASS_FLAT: {
            NU_ASSERT(ppass->color_target && ppass->depth_target);
            nugl__prepare_color_depth(
                ppass, ppass->color_target, ppass->depth_target);
        }
        break;
        case NU_RENDERPASS_SKYBOX: {
            NU_ASSERT(ppass->color_target && ppass->depth_target);
            nugl__prepare_color_depth(
                ppass, ppass->color_target, ppass->depth_target);
        }
        break;
        case NU_RENDERPASS_CANVAS: {
            NU_ASSERT(ppass->color_target);
            nugl__prepare_color_depth(ppass, ppass->color_target, NU_NULL);
        }
        break;
        default:
            break;
    }
}

static void
nugl__draw_mesh (nu_renderpass_t pass,
                 nu_material_t   material,
                 nu_mesh_t       mesh,
                 nu_mat4_t       transform)
{
    nu__gl_t *gl = &_ctx.gl;

    NU_ASSERT(NU_HANDLE_INDEX(material) < gl->materials.size);
    nugl__renderpass_t *ppass = gl->passes.data + NU_HANDLE_INDEX(pass);
    nugl__mesh_t       *pmesh = gl->meshes.data + NU_HANDLE_INDEX(mesh);
    nugl__material_t   *pmat  = gl->materials.data + NU_HANDLE_INDEX(material);
    NU_ASSERT(pmat->type == NU_MATERIAL_MESH);
    // TODO: check command validity
    switch (ppass->type)
    {
        case NU_RENDERPASS_FLAT: {
            nugl__mesh_command_t *cmd = NU_VEC_PUSH(&ppass->flat.cmds);
            cmd->type                 = NUGL__DRAW;
            cmd->transform            = transform;
            cmd->vao                  = pmesh->vao;
            cmd->vbo                  = pmesh->vbo;
            cmd->vcount               = pmesh->vertex_count;
            cmd->texture0             = pmat->mesh.texture0
                                            ? (gl->textures.data
                                   + NU_HANDLE_INDEX(pmat->mesh.texture0))
                                      ->texture
                                            : 0;
            cmd->texture1             = pmat->mesh.texture1
                                            ? (gl->textures.data
                                   + NU_HANDLE_INDEX(pmat->mesh.texture1))
                                      ->texture
                                            : 0;
            cmd->uv_transform         = pmat->mesh.uv_transform;
        }
        break;
        default:
            return;
    }
}
static void
nugl__draw_blit (nu_renderpass_t pass,
                 nu_material_t   material,
                 nu_rect_t       extent,
                 nu_rect_t       tex_extent)
{
    nu__gl_t *gl = &_ctx.gl;

    nugl__renderpass_t *ppass = gl->passes.data + NU_HANDLE_INDEX(pass);
    nugl__material_t   *pmat  = gl->materials.data + NU_HANDLE_INDEX(material);
    NU_ASSERT(pmat->type == NU_MATERIAL_CANVAS);

    switch (ppass->type)
    {
        case NU_RENDERPASS_CANVAS: {
            nugl__canvas_blit_rect(
                &ppass->canvas,
                (_ctx.gl.textures.data + NU_HANDLE_INDEX(pmat->canvas.texture0))
                    ->texture,
                extent,
                tex_extent,
                pmat->canvas.wrap_mode);
        }
        break;
        default:
            break;
    }
}

static void
nugl__setup_api (nu__renderer_api_t *api)
{
    api->init                 = nugl__init;
    api->free                 = nugl__free;
    api->render               = nugl__render;
    api->create_surface_color = nugl__create_surface_color;

    api->camera_create = nugl__camera_create;
    api->camera_view   = nugl__camera_view;
    api->camera_proj   = nugl__camera_proj;

    api->mesh_create = nugl__mesh_create;
    api->mesh_delete = nugl__mesh_delete;
    api->mesh_update = nugl__mesh_update;

    api->texture_create = nugl__texture_create;
    api->texture_delete = nugl__texture_delete;

    api->cubemap_create = nugl__cubemap_create;

    api->material_create       = nugl__material_create;
    api->material_color0       = nugl__material_color0;
    api->material_color1       = nugl__material_color1;
    api->material_uv_transform = nugl__material_uv_transform;
    api->material_wrap_mode    = nugl__material_wrap_mode;

    api->renderpass_create          = nugl__renderpass_create;
    api->renderpass_clear_color     = nugl__renderpass_clear_color;
    api->renderpass_camera          = nugl__renderpass_camera;
    api->renderpass_skybox_cubemap  = nugl__renderpass_skybox_cubemap;
    api->renderpass_skybox_rotation = nugl__renderpass_skybox_rotation;
    api->renderpass_target_color    = nugl__renderpass_target_color;
    api->renderpass_target_depth    = nugl__renderpass_target_depth;
    api->renderpass_reset           = nugl__renderpass_reset;
    api->renderpass_submit          = nugl__renderpass_submit;

    api->draw_mesh = nugl__draw_mesh;
    api->draw_blit = nugl__draw_blit;
}

#endif

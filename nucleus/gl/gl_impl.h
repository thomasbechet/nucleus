#ifndef NUGL_IMPL_H
#define NUGL_IMPL_H

#include <nucleus/internal.h>
#include <nucleus/gl/texture_impl.h>
#include <nucleus/gl/material_impl.h>
#include <nucleus/gl/light_impl.h>
#include <nucleus/gl/mesh_impl.h>
#include <nucleus/gl/camera_impl.h>
#include <nucleus/gl/renderpass_impl.h>

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
    NU_ASSERT(severity != GL_DEBUG_SEVERITY_HIGH);
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
    error = nugl__compile_shader(nugl__shader_screen_blit_vert,
                                 nugl__shader_screen_blit_frag,
                                 &gl->screen_blit_program);
    NU_ERROR_CHECK(error, return error);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    error = nugl__compile_shader(
        nugl__shader_unlit_vert, nugl__shader_unlit_frag, &gl->unlit_program);
    NU_ERROR_CHECK(error, return error);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    error = nugl__compile_shader(
        nugl__shader_lit_vert, nugl__shader_lit_frag, &gl->lit_program);
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

    glEnableVertexAttribArray(0);
    error = nugl__compile_shader(nugl__shader_wireframe_vert,
                                 nugl__shader_wireframe_frag,
                                 &gl->wireframe_program);
    NU_ERROR_CHECK(error, return error);

    glEnableVertexAttribArray(0);
    error = nugl__compile_shader(nugl__shader_shadow_vert,
                                 nugl__shader_shadow_frag,
                                 &gl->shadow_program);
    NU_ERROR_CHECK(error, return error);

    // Create nearest sampler
    glGenSamplers(1, &gl->nearest_sampler);
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
nugl__render (nu_b2i_t global_viewport, nu_b2i_t viewport)
{
    nu__gl_t *gl = &_ctx.gl;

    for (nu_u32_t i = 0; i < gl->passes_order.size; ++i)
    {
        nu_u32_t            pass_index = gl->passes_order.data[i];
        nugl__renderpass_t *pass       = &gl->passes.data[pass_index];
        nugl__execute_renderpass(pass);
    }

    // Blit surface
    glDisable(GL_DEPTH_TEST);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_FRAMEBUFFER_SRGB);
    nu_v4_t clear
        = nu_color_to_vec4(nu_color_to_linear(nu_color(25, 27, 43, 255)));
    glUseProgram(gl->screen_blit_program);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    nu_v2u_t size = nu_b2i_size(viewport);
    glViewport(viewport.min.x, viewport.min.y, size.x, size.y);
    glClearColor(clear.x, clear.y, clear.z, clear.w);
    glClear(GL_COLOR_BUFFER_BIT);
    glBindTexture(GL_TEXTURE_2D,
                  gl->textures.data[gl->surface_color_index].texture);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glUseProgram(0);

    // Reset for next frame
    NU_VEC_CLEAR(&gl->passes_order);

    return NU_ERROR_NONE;
}
static nu_texture_t
nugl__create_surface_color (nu_v2u_t size)
{
    nu__gl_t *gl = &_ctx.gl;

    nugl__texture_t *ptex   = NU_VEC_PUSH(&gl->textures);
    gl->surface_color_index = gl->textures.size - 1;
    ptex->size              = nu_v3u(size.x, size.y, 0);

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

#endif

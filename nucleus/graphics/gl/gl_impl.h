#ifndef NUGL_IMPL_H
#define NUGL_IMPL_H

#include <nucleus/internal.h>
#include <nucleus/graphics/gl/shader_data.h>
#include <nucleus/graphics/gl/texture_impl.h>
#include <nucleus/graphics/gl/mesh_impl.h>
#include <nucleus/graphics/gl/renderpass_impl.h>

static nu_error_t
nugl__compile_shader (nu_str_t source, GLuint shader_type, GLuint *shader)
{
    GLint success;
    *shader                 = glCreateShader(shader_type);
    const GLchar *psource[] = { (const GLchar *)source.data };
    const GLint   psize[]   = { source.size };
    glShaderSource(*shader, 1, psource, psize);
    glCompileShader(*shader);
    glGetShaderiv(*shader, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE)
    {
        nu_scope_push();
        GLint max_length = 0;
        glGetShaderiv(*shader, GL_INFO_LOG_LENGTH, &max_length);
        GLchar *log = (GLchar *)nu_scope_alloc(sizeof(GLchar) * max_length);
        glGetShaderInfoLog(*shader, max_length, &max_length, log);
        NU_ERROR("%s", log);
        nu_scope_pop();
        glDeleteShader(*shader);
        return NU_ERROR_SHADER_COMPILATION;
    }
    return NU_ERROR_NONE;
}
static nu_error_t
nugl__compile_program (nu_str_t vert, nu_str_t frag, GLuint *program)
{
    GLuint vertex_shader, fragment_shader;
    GLint  success;

    nu_error_t error;

    error = nugl__compile_shader(vert, GL_VERTEX_SHADER, &vertex_shader);
    NU_CHECK_ERROR(error, goto cleanup0);

    error = nugl__compile_shader(frag, GL_FRAGMENT_SHADER, &fragment_shader);
    NU_CHECK_ERROR(error, goto cleanup1);

    *program = glCreateProgram();
    glAttachShader(*program, vertex_shader);
    glAttachShader(*program, fragment_shader);

    glLinkProgram(*program);
    glGetProgramiv(*program, GL_LINK_STATUS, &success);
    if (success == GL_FALSE)
    {
        GLint max_length = 0;
        glGetProgramiv(*program, GL_INFO_LOG_LENGTH, &max_length);
        nu_scope_push();
        GLchar *log = (GLchar *)nu_scope_alloc(sizeof(GLchar) * max_length);
        glGetProgramInfoLog(*program, max_length, &max_length, log);
        NU_ERROR("%s", log);
        nu_scope_pop();

        glDeleteProgram(*program);
        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);
        return NU_ERROR_SHADER_COMPILATION;
    }

    glDeleteShader(fragment_shader);
cleanup1:
    glDeleteShader(vertex_shader);
cleanup0:
    return error;
}

static const GLchar *
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

    nu__gl_t *gl = &_ctx.graphics.gl;

    // Initialize containers
    NU_VEC_ALLOC(&gl->targets, 16);
    NU_VEC_ALLOC(&gl->passes_order, 64);

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
    error = nugl__compile_program(nugl__shader_screen_blit_vert,
                                  nugl__shader_screen_blit_frag,
                                  &gl->screen_blit_program);
    NU_CHECK_ERROR(error, return error);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    error = nugl__compile_program(
        nugl__shader_unlit_vert, nugl__shader_unlit_frag, &gl->unlit_program);
    NU_CHECK_ERROR(error, return error);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    error = nugl__compile_program(
        nugl__shader_lit_vert, nugl__shader_lit_frag, &gl->lit_program);
    NU_CHECK_ERROR(error, return error);
    glUseProgram(gl->lit_program);
    glUniform1i(glGetUniformLocation(gl->lit_program, "texture0"), 0);
    glUniform1i(glGetUniformLocation(gl->lit_program, "shadow_map"), 1);

    error = nugl__compile_program(nugl__shader_skybox_vert,
                                  nugl__shader_skybox_frag,
                                  &gl->skybox_program);
    NU_CHECK_ERROR(error, return error);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);
    error = nugl__compile_program(nugl__shader_canvas_blit_vert,
                                  nugl__shader_canvas_blit_frag,
                                  &gl->canvas_blit_program);
    NU_CHECK_ERROR(error, return error);

    glEnableVertexAttribArray(0);
    error = nugl__compile_program(nugl__shader_wireframe_vert,
                                  nugl__shader_wireframe_frag,
                                  &gl->wireframe_program);
    NU_CHECK_ERROR(error, return error);

    glEnableVertexAttribArray(0);
    error = nugl__compile_program(nugl__shader_shadow_vert,
                                  nugl__shader_shadow_frag,
                                  &gl->shadow_program);
    NU_CHECK_ERROR(error, return error);

    // Create nearest sampler
    glGenSamplers(1, &gl->nearest_sampler);
    glSamplerParameteri(gl->nearest_sampler, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glSamplerParameteri(gl->nearest_sampler, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    return NU_ERROR_NONE;
}
static nu_error_t
nugl__free (void)
{
    nu__gl_t *gl = &_ctx.graphics.gl;
    glDeleteProgram(gl->screen_blit_program);
    glDeleteProgram(gl->unlit_program);
    glDeleteProgram(gl->lit_program);
    glDeleteProgram(gl->skybox_program);
    glDeleteProgram(gl->canvas_blit_program);
    glDeleteProgram(gl->wireframe_program);
    glDeleteProgram(gl->shadow_program);
    glDeleteSamplers(1, &gl->nearest_sampler);
    return NU_ERROR_NONE;
}
static nu_error_t
nugl__render (nu_b2i_t global_viewport, nu_b2i_t viewport)
{
    nu__gl_t *gl = &_ctx.graphics.gl;

    for (nu_u32_t i = 0; i < gl->passes_order.size; ++i)
    {
        nu__renderpass_t *pass = (nu__renderpass_t *)gl->passes_order.data[i];
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
                  ((nu__texture_t *)_ctx.graphics.surface_color)->gl.texture);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glUseProgram(0);

    // Reset for next frame
    NU_VEC_CLEAR(&gl->passes_order);

    return NU_ERROR_NONE;
}
static void
nugl__init_surface_texture (nu__texture_t *tex)
{
    nu__gl_t *gl = &_ctx.graphics.gl;

    glGenTextures(1, &tex->gl.texture);
    glBindTexture(GL_TEXTURE_2D, tex->gl.texture);
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_SRGB,
                 tex->size.x,
                 tex->size.y,
                 0,
                 GL_RGB,
                 GL_UNSIGNED_BYTE,
                 NU_NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

#endif

#ifndef NUGL_CANVAS_IMPL_H
#define NUGL_CANVAS_IMPL_H

#include <nucleus/internal.h>

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

static nu_error_t
nugl__canvas_create (nugl__renderpass_canvas_t *pass)
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

static void
nugl__canvas_render (nugl__renderpass_t *pass)
{
    nu__gl_t *gl = &_ctx.gl;
    // Update buffers
    nugl__write_canvas_buffers(&pass->canvas);
    glUseProgram(gl->canvas_blit_program);

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glUniform2uiv(
        glGetUniformLocation(gl->canvas_blit_program, "viewport_size"),
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
                glDrawArraysInstancedBaseInstance(GL_TRIANGLES,
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
static void
nugl__canvas_reset (nugl__renderpass_t *pass)
{
}

#endif

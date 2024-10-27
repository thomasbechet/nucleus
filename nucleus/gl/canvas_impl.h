#ifndef NUGL_CANVAS_IMPL_H
#define NUGL_CANVAS_IMPL_H

#include <nucleus/internal.h>

static void
nugl__canvas_add_blit (nugl__renderpass_canvas_t *pass,
                       nu_v2i_t                   pos,
                       nu_v2u_t                   tex,
                       nu_v2u_t                   size)
{
    nugl__gpu_blit_t *blit = NU_VEC_PUSH(&pass->blit_transfer);
    blit->pos              = ((nu_u32_t)pos.y << 16) | (nu_u32_t)pos.x;
    blit->tex              = (tex.y << 16) | tex.x;
    blit->size             = (size.y << 16) | size.x;
    blit->depth
        = (pass->depth - NUGL__MIN_DEPTH) / (NUGL__MAX_DEPTH - NUGL__MIN_DEPTH);
}

static void
nugl__canvas_reset (nugl__renderpass_canvas_t *pass)
{
    NU_VEC_CLEAR(&pass->cmds);
    NU_VEC_CLEAR(&pass->blit_transfer);
    pass->depth = 0;
}
static void
nugl__canvas_init (nugl__renderpass_canvas_t *pass)
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

    nugl__canvas_reset(pass);
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
nugl__canvas_draw_blit (nu__renderpass_t *pass,
                        nu_material_t     material,
                        nu_b2i_t          extent,
                        nu_b2i_t          tex_extent)
{
    NU_ASSERT(material);
    const nu__material_t *pmat
        = _ctx.graphics.materials.data + NU_HANDLE_INDEX(material);
    NU_ASSERT(pmat->type == NU_MATERIAL_CANVAS);

    nu_u32_t blit_count = 0;
    switch (pmat->canvas.wrap_mode)
    {
        case NU_TEXTURE_WRAP_CLAMP: {
            nugl__canvas_add_blit(
                &pass->gl.canvas,
                extent.min,
                nu_v2u(tex_extent.min.x, tex_extent.min.y),
                nu_v2u_min(nu_b2i_size(extent), nu_b2i_size(tex_extent)));
            blit_count = 1;
        }
        break;
        case NU_TEXTURE_WRAP_REPEAT: {
            nu_v2u_t extent_s     = nu_b2i_size(extent);
            nu_v2u_t tex_extent_s = nu_b2i_size(tex_extent);
            // Calculate blit count
            nu_size_t full_hblit_count = extent_s.x / tex_extent_s.x;
            nu_size_t full_vblit_count = extent_s.y / tex_extent_s.y;
            blit_count                 = full_hblit_count * full_vblit_count;

            // Insert full blits
            for (nu_size_t y = 0; y < full_vblit_count; ++y)
            {
                for (nu_size_t x = 0; x < full_hblit_count; ++x)
                {
                    nu_i32_t pos_x = extent.min.x + (x * tex_extent_s.x);
                    nu_i32_t pos_y = extent.min.y + (y * tex_extent_s.y);
                    nugl__canvas_add_blit(
                        &pass->gl.canvas,
                        nu_v2i(pos_x, pos_y),
                        nu_v2u(tex_extent.min.x, tex_extent.min.y),
                        tex_extent_s);
                }
            }

            // Insert partial blits
            nu_size_t partial_hblit_size = extent_s.x % tex_extent_s.x;
            nu_size_t partial_vblit_size = extent_s.y % tex_extent_s.y;

            if (partial_hblit_size)
            {
                for (nu_size_t y = 0; y < full_vblit_count; ++y)
                {
                    nu_i32_t pos_x
                        = extent.min.x + (full_hblit_count * tex_extent_s.x);
                    nu_i32_t pos_y = extent.min.y + (y * tex_extent_s.y);
                    nu_v2u_t size  = nu_v2u(partial_hblit_size, tex_extent_s.y);
                    nugl__canvas_add_blit(
                        &pass->gl.canvas,
                        nu_v2i(pos_x, pos_y),
                        nu_v2u(tex_extent.min.x, tex_extent.min.y),
                        size);
                    ++blit_count;
                }
            }
            if (partial_vblit_size)
            {
                for (nu_size_t x = 0; x < full_hblit_count; ++x)
                {
                    nu_i32_t pos_x = extent.min.x + (x * tex_extent_s.x);
                    nu_i32_t pos_y
                        = extent.min.y + (full_vblit_count * tex_extent_s.y);
                    nu_v2u_t size = nu_v2u(tex_extent_s.x, partial_vblit_size);
                    nugl__canvas_add_blit(
                        &pass->gl.canvas,
                        nu_v2i(pos_x, pos_y),
                        nu_v2u(tex_extent.min.x, tex_extent.min.y),
                        size);
                    ++blit_count;
                }
            }
            if (partial_hblit_size && partial_vblit_size)
            {
                nu_i32_t pos_x
                    = extent.min.x + (full_hblit_count * tex_extent_s.x);
                nu_i32_t pos_y
                    = extent.min.y + (full_vblit_count * tex_extent_s.y);
                nu_v2u_t size = nu_v2u(partial_hblit_size, partial_vblit_size);
                nugl__canvas_add_blit(
                    &pass->gl.canvas,
                    nu_v2i(pos_x, pos_y),
                    nu_v2u(tex_extent.min.x, tex_extent.min.y),
                    size);
                ++blit_count;
            }
        }
        break;
        case NU_TEXTURE_WRAP_MIRROR:
            break;
    }
    pass->gl.canvas.depth += NUGL__DEPTH_INCREMENT;

    nugl__canvas_command_t *last = NU_VEC_LAST(&pass->gl.canvas.cmds);
    NU_ASSERT(pmat->canvas.texture0);
    GLuint texture
        = _ctx.graphics.textures.data[NU_HANDLE_INDEX(pmat->canvas.texture0)]
              .gl.texture;
    if (last && last->type == NUGL__CANVAS_BLIT
        && last->blit.texture == texture)
    {
        last->blit.instance_count += blit_count;
    }
    else
    {
        last               = NU_VEC_PUSH(&pass->gl.canvas.cmds);
        last->type         = NUGL__CANVAS_BLIT;
        last->blit.texture = texture;
        last->blit.instance_start
            = pass->gl.canvas.blit_transfer.size - blit_count;
        last->blit.instance_count = blit_count;
    }
}

static void
nugl__canvas_render (nu__renderpass_t *pass)
{
    nu__gl_t *gl = &_ctx.gl;
    // Update buffers
    nugl__write_canvas_buffers(&pass->gl.canvas);
    glUseProgram(gl->canvas_blit_program);

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glUniform2uiv(
        glGetUniformLocation(gl->canvas_blit_program, "viewport_size"),
        1,
        pass->gl.fbo_size.data);
    glBindVertexArray(pass->gl.canvas.blit_vao);
    for (nu_size_t c = 0; c < pass->gl.canvas.cmds.size; ++c)
    {
        nugl__canvas_command_t *cmd = pass->gl.canvas.cmds.data + c;
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

#endif

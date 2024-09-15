#ifndef NU_FONT_IMPL_H
#define NU_FONT_IMPL_H

#include <nucleus/internal.h>

nu_font_t
nu_font_create_default (void)
{
    nu_error_t error;

    nu_size_t   index;
    nu__font_t *font = NU_POOL_ADD(&_ctx.graphics.fonts, &index);

    // Find min/max characters
    font->min_char             = (nu_char_t)127;
    font->max_char             = -128;
    const nu_size_t char_count = sizeof(nu__font_data_chars);
    for (nu_size_t i = 0; i < char_count; ++i)
    {
        font->min_char = NU_MIN(font->min_char, nu__font_data_chars[i]);
        font->max_char = NU_MAX(font->max_char, nu__font_data_chars[i]);
    }

    const nu_size_t pixel_per_glyph
        = NU__FONT_DATA_WIDTH * NU__FONT_DATA_HEIGHT;

    font->glyphs_count = font->max_char - font->min_char + 1;
    font->glyph_size   = nu_uvec2(NU__FONT_DATA_WIDTH, NU__FONT_DATA_HEIGHT);
    font->glyphs
        = (nu_rect_t *)nu_alloc(sizeof(nu_rect_t) * font->glyphs_count);
    nu_check(font->glyphs, return NU_NULL);

    nu_assert(((sizeof(nu__font_data) * 8) / pixel_per_glyph) == char_count);

    // Load default font data into image
    nu_uvec2_t image_size
        = nu_uvec2(NU__FONT_DATA_WIDTH * char_count, NU__FONT_DATA_HEIGHT);
    nu_image_t  image      = nu_image_create(image_size);
    nu_color_t *image_data = nu_image_colors(image);

    nu_rect_t extent = nu_rect(0, 0, NU__FONT_DATA_WIDTH, NU__FONT_DATA_HEIGHT);
    for (nu_size_t ci = 0; ci < char_count; ++ci)
    {
        for (nu_size_t p = 0; p < pixel_per_glyph; ++p)
        {
            nu_size_t bit_offset = ci * pixel_per_glyph + p;
            nu_assert((bit_offset / 8) < sizeof(nu__font_data));
            nu_byte_t byte    = nu__font_data[bit_offset / 8];
            nu_byte_t bit_set = (byte & (1 << (7 - (p % 8)))) != 0;

            nu_size_t px = extent.p.x + p % NU__FONT_DATA_WIDTH;
            nu_size_t py = extent.p.y + p / NU__FONT_DATA_WIDTH;
            nu_size_t pi = py * image_size.x + px;

            nu_byte_t color = bit_set ? 0xFF : 0x00;
            nu_assert(pi < (image_size.x * image_size.y));
            image_data[pi].r = color;
            image_data[pi].g = color;
            image_data[pi].b = color;
            image_data[pi].a = color;
        }
        nu_size_t gi = nu__font_data_chars[ci] - font->min_char;
        nu_assert(gi < font->glyphs_count);
        font->glyphs[gi] = extent;
        extent.p.x += NU__FONT_DATA_WIDTH;
    }

    // Create renderer image
    font->texture = nu_texture_create_image(image);

    // Create material
    {
        nu_material_info_t info;
        info.type             = NU_MATERIAL_CANVAS;
        info.canvas.color0    = &font->texture;
        info.canvas.wrap_mode = NU_TEXTURE_WRAP_CLAMP;
        font->material        = nu_material_create(&info);
    }

    // Free resources
    nu_image_delete(image);

    return nu_handle_make(nu_font_t, index);
}
void
nu_font_delete (nu_font_t handle)
{
    nu_size_t   index = nu_handle_index(handle);
    nu__font_t *font  = &_ctx.graphics.fonts.data[index];
    nu_free(font->glyphs, sizeof(nu_rect_t) * font->glyphs_count);
    nu_texture_delete(font->texture);
    nu_material_delete(font->material);
}
void
nu_draw_text (nu_renderpass_t  pass,
              nu_font_t        handle,
              const nu_char_t *text,
              nu_size_t        n,
              nu_ivec2_t       pos)
{
    nu_size_t   index = nu_handle_index(handle);
    nu__font_t *font  = &_ctx.graphics.fonts.data[index];
    nu_rect_t   extent
        = nu_rect(pos.x, pos.y, font->glyph_size.x, font->glyph_size.y);
    for (nu_size_t i = 0; i < n; ++i)
    {
        nu_char_t c = text[i];
        if (c == '\n')
        {
            extent.p.x = pos.x;
            extent.p.y += font->glyph_size.y;
            continue;
        }
        if (c < font->min_char || c > font->max_char)
        {
            continue;
        }
        nu_size_t gi         = c - font->min_char;
        nu_rect_t tex_extent = font->glyphs[gi];
        nu_draw_blit(pass, font->material, extent, tex_extent);
        extent.p.x += font->glyph_size.x;
    }
}

#endif

#ifndef NU_FONT_IMPL_H
#define NU_FONT_IMPL_H

#include <nucleus/graphics/font.h>
#include <nucleus/graphics/image.h>
#include <nucleus/graphics/font_data.h>

nu_error_t
nu_font_init_default (nu_renderer_t  renderer,
                      nu_allocator_t alloc,
                      nu_font_t     *font)
{
    nu_error_t error;

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
    font->glyphs = nu_alloc(alloc, sizeof(nu_rect_t) * font->glyphs_count);
    NU_CHECK(font->glyphs, return NU_ERROR_ALLOCATION);

    NU_ASSERT(((sizeof(nu__font_data) * 8) / pixel_per_glyph) == char_count);

    // Load default font data into image
    nu_image_t image;
    error = nu_image_init(
        alloc,
        nu_uvec2(NU__FONT_DATA_WIDTH * char_count, NU__FONT_DATA_HEIGHT),
        &image);
    NU_ERROR_CHECK(error, return error);

    nu_rect_t extent = nu_rect(0, 0, NU__FONT_DATA_WIDTH, NU__FONT_DATA_HEIGHT);
    for (nu_size_t ci = 0; ci < char_count; ++ci)
    {
        for (nu_size_t p = 0; p < pixel_per_glyph; ++p)
        {
            nu_size_t bit_offset = ci * pixel_per_glyph + p;
            NU_ASSERT((bit_offset / 8) < sizeof(nu__font_data));
            nu_byte_t byte    = nu__font_data[bit_offset / 8];
            nu_byte_t bit_set = (byte & (1 << (7 - (p % 8)))) != 0;

            nu_size_t px = extent.p.x + p % NU__FONT_DATA_WIDTH;
            nu_size_t py = extent.p.y + p / NU__FONT_DATA_WIDTH;
            nu_size_t pi = py * image.size.x + px;

            nu_byte_t color = bit_set ? 0xFF : 0x00;
            NU_ASSERT(pi < (image.size.x * image.size.y));
            image.data[pi].r = color;
            image.data[pi].g = color;
            image.data[pi].b = color;
            image.data[pi].a = color;
        }
        nu_size_t gi = nu__font_data_chars[ci] - font->min_char;
        NU_ASSERT(gi < font->glyphs_count);
        font->glyphs[gi] = extent;
        extent.p.x += NU__FONT_DATA_WIDTH;
    }

    // Create renderer image
    {
        nu_texture_info_t info;
        info.usage  = NU_TEXTURE_USAGE_SAMPLE;
        info.format = NU_TEXTURE_FORMAT_COLOR;
        info.size   = image.size;
        info.colors = image.data;
        error       = nu_texture_create(renderer, &info, &font->texture);
        NU_ERROR_CHECK(error, return error);
    }

    // Create material
    {
        nu_material_info_t info = NU_MATERIAL_INFO_DEFAULT_CANVAS;
        info.canvas.color0      = &font->texture;
        info.canvas.wrap_mode   = NU_TEXTURE_WRAP_CLAMP;
        error = nu_material_create(renderer, &info, &font->material);
        NU_ERROR_CHECK(error, return error);
    }

    // Free resources
    nu_image_free(&image, alloc);

    return NU_ERROR_NONE;
}
void
nu_font_free (nu_font_t *font, nu_renderer_t renderer, nu_allocator_t alloc)
{
    nu_free(alloc, font->glyphs, sizeof(nu_rect_t) * font->glyphs_count);
    nu_texture_delete(renderer, font->texture);
    nu_material_delete(renderer, font->material);
}
void
nu_draw_text (nu_renderer_t          ctx,
              nu_renderpass_handle_t pass,
              const nu_char_t       *text,
              nu_size_t              n,
              nu_ivec2_t             pos,
              const nu_font_t       *font)
{
    nu_rect_t extent
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
        nu_draw_blit(ctx, pass, font->material, extent, tex_extent);
        extent.p.x += font->glyph_size.x;
    }
}

#endif

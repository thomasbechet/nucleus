#ifndef NU_FONT_IMPL_H
#define NU_FONT_IMPL_H

#include <nucleus/internal.h>

static void
nu__font_handler (nu_object_hook_t hook, void *data)
{
}
nu_font_t
nu_font_new_default (void)
{
    nu_error_t error;

    nu__font_t *font = (nu__font_t *)nu_object_new(_ctx.graphics.obj_font);

    // Find min/max characters
    font->min_char             = 127;
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
    font->glyph_size   = nu_v2u(NU__FONT_DATA_WIDTH, NU__FONT_DATA_HEIGHT);
    font->glyphs
        = (nu_b2i_t *)nu_scope_alloc(sizeof(nu_b2i_t) * font->glyphs_count);
    NU_CHECK(font->glyphs, return NU_NULL);

    NU_ASSERT(((sizeof(nu__font_data) * 8) / pixel_per_glyph) == char_count);

    // Load default font data into image
    nu_v3u_t image_size
        = nu_v3u(NU__FONT_DATA_WIDTH * char_count, NU__FONT_DATA_HEIGHT, 1);
    // TODO: avoid temporary image ?
    nu_image_t image      = nu_image_new(NU_IMAGE_RGBA, image_size, 1);
    nu_byte_t *image_data = nu_image_data(image, 0);

    nu_b2i_t extent
        = nu_b2i_xywh(0, 0, NU__FONT_DATA_WIDTH, NU__FONT_DATA_HEIGHT);
    for (nu_size_t ci = 0; ci < char_count; ++ci)
    {
        for (nu_size_t p = 0; p < pixel_per_glyph; ++p)
        {
            nu_size_t bit_offset = ci * pixel_per_glyph + p;
            NU_ASSERT((bit_offset / 8) < sizeof(nu__font_data));
            nu_byte_t byte    = nu__font_data[bit_offset / 8];
            nu_byte_t bit_set = (byte & (1 << (7 - (p % 8)))) != 0;

            nu_size_t px = extent.min.x + p % NU__FONT_DATA_WIDTH;
            nu_size_t py = extent.min.y + p / NU__FONT_DATA_WIDTH;
            nu_size_t pi = py * image_size.x + px;

            nu_byte_t color = bit_set ? 0xFF : 0x00;
            NU_ASSERT(pi < (image_size.x * image_size.y));
            image_data[pi * 4 + 0] = color;
            image_data[pi * 4 + 1] = color;
            image_data[pi * 4 + 2] = color;
            image_data[pi * 4 + 3] = color;
        }
        nu_size_t gi = nu__font_data_chars[ci] - font->min_char;
        NU_ASSERT(gi < font->glyphs_count);
        font->glyphs[gi] = extent;
        extent = nu_b2i_translate(extent, nu_v2i(NU__FONT_DATA_WIDTH, 0));
    }

    // Create renderer image
    font->texture = nu_texture_new_from_image(NU_TEXTURE_COLORMAP, image);

    // Create material
    font->material = nu_material_new(NU_MATERIAL_CANVAS);
    nu_material_set_texture(font->material, font->texture);
    nu_material_set_wrap_mode(font->material, NU_TEXTURE_WRAP_CLAMP);

    return (nu_font_t)font;
}

void
nu_draw_text (nu_renderpass_t pass,
              nu_str_t        str,
              nu_font_t       handle,
              nu_v2i_t        pos)
{
    nu__font_t *font = (nu__font_t *)handle;
    nu_b2i_t    extent
        = nu_b2i_xywh(pos.x, pos.y, font->glyph_size.x, font->glyph_size.y);
    nu_size_t  it = 0;
    nu_wchar_t c;
    while (nu_str_next(str, &it, &c))
    {
        if (c == '\n')
        {
            extent = nu_b2i_moveto(
                extent, nu_v2i(pos.x, extent.min.y + font->glyph_size.y));
            continue;
        }
        if (c < font->min_char || c > font->max_char)
        {
            continue;
        }
        nu_size_t gi         = c - font->min_char;
        nu_b2i_t  tex_extent = font->glyphs[gi];
        nu_draw_blit(pass, extent, tex_extent, font->material);
        extent = nu_b2i_translate(extent, nu_v2i(font->glyph_size.x, 0));
    }
}

#endif

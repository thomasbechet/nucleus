#ifndef NU_FONT_H
#define NU_FONT_H

#include <nucleus/graphics.h>

typedef struct
{
    nu_renderer_t  renderer;
    nu_allocator_t allocator;
    nu_texture_t   texture;
    nu_material_t  material;
    nu_rect_t     *glyphs;
    nu_size_t      glyphs_count;
    nu_uvec2_t     glyph_size;
    nu_char_t      min_char;
    nu_char_t      max_char;
} nu__font_t;

NU_DEFINE_HANDLE_OBJECT(nu_font_t, nu__font_t);

NU_API nu_error_t nu_font_create_default(nu_renderer_t  renderer,
                                         nu_allocator_t allocator,
                                         nu_font_t     *font);
NU_API void       nu_font_delete(nu_font_t font);
NU_API void       nu_draw_text(nu_renderer_t    renderer,
                               nu_renderpass_t  pass,
                               nu_font_t        font,
                               const nu_char_t *text,
                               nu_size_t        n,
                               nu_ivec2_t       pos);

#endif

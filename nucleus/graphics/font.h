#ifndef NU_FONT_H
#define NU_FONT_H

#include <nucleus/graphics.h>

typedef struct
{
    nu_texture_handle_t  texture;
    nu_material_handle_t material;
    nu_rect_t           *glyphs;
    nu_size_t            glyphs_count;
    nu_uvec2_t           glyph_size;
    nu_char_t            min_char;
    nu_char_t            max_char;
} nu_font_t;

NU_API nu_error_t nu_font_init_default(nu_renderer_t  renderer,
                                       nu_allocator_t alloc,
                                       nu_font_t     *font);
NU_API void       nu_font_free(nu_font_t     *font,
                               nu_renderer_t  renderer,
                               nu_allocator_t alloc);
NU_API void       nu_draw_text(nu_renderer_t          ctx,
                               nu_renderpass_handle_t pass,
                               const nu_char_t       *text,
                               nu_size_t              n,
                               nu_ivec2_t             pos,
                               const nu_font_t       *font);

#endif

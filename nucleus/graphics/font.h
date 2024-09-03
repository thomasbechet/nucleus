#ifndef NU_FONT_H
#define NU_FONT_H

#include <nucleus/graphics/renderpass.h>

NU_DEFINE_HANDLE(nu_font_t);

NU_API nu_error_t nu_font_create_default(nu_font_t *font);
NU_API void       nu_font_delete(nu_font_t font);
NU_API void       nu_draw_text(nu_renderpass_t  pass,
                               nu_font_t        font,
                               const nu_char_t *text,
                               nu_size_t        n,
                               nu_ivec2_t       pos);

#endif

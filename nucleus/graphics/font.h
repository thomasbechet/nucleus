#ifndef NU_FONT_H
#define NU_FONT_H

#include <nucleus/graphics/api.h>
#include <nucleus/graphics/font_data.h>

typedef struct
{
    nu_texture_t  texture;
    nu_material_t material;
    nu_b2i_t     *glyphs;
    nu_size_t     glyphs_count;
    nu_v2u_t      glyph_size;
    nu_wchar_t    min_char;
    nu_wchar_t    max_char;
} nu__font_t;

typedef NU_POOL(nu__font_t) nu__font_pool_t;

#endif

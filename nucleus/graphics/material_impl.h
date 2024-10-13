#ifndef NU_MATERIAL_IMPL_H
#define NU_MATERIAL_IMPL_H

#include <nucleus/internal.h>

nu_material_t
nu_material_create (nu_material_type_t type)
{
#ifdef NU_BUILD_GL
    return nugl__material_create(type);
#endif
}
void
nu_material_delete (nu_material_t material)
{
#ifdef NU_BUILD_GL
    nugl__material_delete(material);
#endif
}
void
nu_material_set_color (nu_material_t material, nu_color_t color)
{
#ifdef NU_BUILD_GL
    nugl__material_set_color(material, color);
}
#endif
void
nu_material_set_texture (nu_material_t material, nu_texture_t texture)
{
#ifdef NU_BUILD_GL
    nugl__material_set_texture(material, texture);
#endif
}
void
nu_material_set_uv_transform (nu_material_t material, nu_m3_t mat)
{
#ifdef NU_BUILD_GL
    nugl__material_set_uv_transform(material, mat);
#endif
}
void
nu_material_set_wrap_mode (nu_material_t material, nu_texture_wrap_mode_t mode)
{
#ifdef NU_BUILD_GL
    nugl__material_set_wrap_mode(material, mode);
#endif
}

#endif

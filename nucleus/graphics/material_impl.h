#ifndef NU_MATERIAL_IMPL_H
#define NU_MATERIAL_IMPL_H

#include <nucleus/internal.h>

nu_material_t
nu_material_create (nu_material_type_t type)
{
    nu_size_t       index;
    nu__material_t *mat = NU_POOL_ADD(&_ctx.graphics.materials, &index);
    mat->type           = type;
    switch (mat->type)
    {
        case NU_MATERIAL_SURFACE:
            mat->surface.color        = NU_COLOR_WHITE;
            mat->surface.texture0     = NU_NULL;
            mat->surface.texture1     = NU_NULL;
            mat->surface.uv_transform = nu_m3_identity();
            break;
        case NU_MATERIAL_CANVAS:
            mat->canvas.texture0  = NU_NULL;
            mat->canvas.wrap_mode = NU_TEXTURE_WRAP_CLAMP;
            break;
    }
    return NU_HANDLE_MAKE(nu_material_t, index);
}
void
nu_material_delete (nu_material_t material)
{
    NU_POOL_REMOVE(&_ctx.graphics.materials, NU_HANDLE_INDEX(material));
}
void
nu_material_set_color (nu_material_t material, nu_color_t color)
{
    nu__material_t *mat
        = _ctx.graphics.materials.data + NU_HANDLE_INDEX(material);
    switch (mat->type)
    {
        case NU_MATERIAL_SURFACE:
            mat->surface.color = color;
            break;
        default:
            NU_ERROR("cannot set color on non surface material");
            break;
    }
}
void
nu_material_set_texture (nu_material_t material, nu_texture_t texture)
{
    nu__material_t *mat
        = _ctx.graphics.materials.data + NU_HANDLE_INDEX(material);
    switch (mat->type)
    {
        case NU_MATERIAL_SURFACE:
            mat->surface.texture0 = texture;
            break;
        case NU_MATERIAL_CANVAS:
            mat->canvas.texture0 = texture;
            break;
    }
}
void
nu_material_set_uv_transform (nu_material_t material, nu_m3_t transform)
{
    nu__material_t *mat
        = _ctx.graphics.materials.data + NU_HANDLE_INDEX(material);
    switch (mat->type)
    {
        case NU_MATERIAL_SURFACE:
            mat->surface.uv_transform = transform;
            break;
        default:
            NU_ERROR("cannot set uv transform on non surface material");
            break;
    }
}
void
nu_material_set_wrap_mode (nu_material_t material, nu_texture_wrap_mode_t mode)
{
    nu__material_t *mat
        = _ctx.graphics.materials.data + NU_HANDLE_INDEX(material);
    switch (mat->type)
    {
        case NU_MATERIAL_CANVAS:
            mat->canvas.wrap_mode = mode;
            break;
        default:
            NU_ERROR("cannot set wrap mode on non canvas material");
            break;
    }
}

#endif

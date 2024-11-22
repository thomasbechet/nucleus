#ifndef NU_MATERIAL_IMPL_H
#define NU_MATERIAL_IMPL_H

#include <nucleus/internal.h>

static void
nu__material_handler (nu_object_hook_t hook, void *data)
{
}
nu_material_t
nu_material_new (nu_material_type_t type)
{
    nu__material_t *mat = nu_object_new(_ctx.graphics.obj_material);
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
    return (nu_material_t)mat;
}
nu_material_t
nu_material_new_color (nu_material_type_t type, nu_color_t color)
{
    nu_material_t mat = nu_material_new(type);
    nu_material_set_color(mat, color);
    return mat;
}
void
nu_material_set_color (nu_material_t material, nu_color_t color)
{
    nu__material_t *mat = (nu__material_t *)material;
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
    nu__material_t *mat = (nu__material_t *)material;
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
    nu__material_t *mat = (nu__material_t *)material;
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
    nu__material_t *mat = (nu__material_t *)material;
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

static nu_m3_t
nu__material_surface_uv_transform (nu_material_t material)
{
    if (material)
    {
        nu__material_t *mat = (nu__material_t *)material;
        return mat->surface.uv_transform;
    }
    return nu_m3_identity();
}
static GLuint
nu__material_surface_texture0 (nu_material_t material)
{
    if (material)
    {
        nu__material_t *mat = (nu__material_t *)material;
        nu_texture_t    tex = mat->surface.texture0;
        if (tex)
        {
            return ((nu__texture_t *)tex)->gl.texture;
        }
    }
    return 0;
}
static nu_color_t
nu__material_surface_color (nu_material_t material, nu_color_t vdefault)
{
    if (material)
    {
        nu__material_t *mat = (nu__material_t *)material;
        return mat->surface.color;
    }
    return vdefault;
}

#endif

#ifndef NUGL_MATERIAL_IMPL_H
#define NUGL_MATERIAL_IMPL_H

#include <nucleus/internal.h>

static void
nugl__material_set_color (nu_material_t material, nu_color_t color)
{
    nugl__material_t *mat = _ctx.gl.materials.data + NU_HANDLE_INDEX(material);
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
static void
nugl__material_set_texture (nu_material_t material, nu_texture_t texture)
{
    nugl__material_t *mat = _ctx.gl.materials.data + NU_HANDLE_INDEX(material);
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
static void
nugl__material_set_uv_transform (nu_material_t material, nu_mat3_t matrix)
{
    nugl__material_t *mat = _ctx.gl.materials.data + NU_HANDLE_INDEX(material);
    switch (mat->type)
    {
        case NU_MATERIAL_SURFACE:
            mat->surface.uv_transform = matrix;
            break;
        default:
            NU_ERROR("cannot set uv transform on non surface material");
            break;
    }
}
static void
nugl__material_set_wrap_mode (nu_material_t          material,
                              nu_texture_wrap_mode_t mode)
{
    nugl__material_t *mat = _ctx.gl.materials.data + NU_HANDLE_INDEX(material);
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
static nu_material_t
nugl__material_create (nu_material_type_t type)
{
    nu__gl_t *gl = &_ctx.gl;

    nugl__material_t *p = NU_VEC_PUSH(&gl->materials);
    nu_material_t     handle
        = NU_HANDLE_MAKE(nu_material_t, gl->materials.size - 1);

    p->type = type;
    switch (p->type)
    {
        case NU_MATERIAL_SURFACE:
            p->surface.color        = NU_COLOR_WHITE;
            p->surface.texture0     = NU_NULL;
            p->surface.texture1     = NU_NULL;
            p->surface.uv_transform = nu_mat3_identity();
            break;
        case NU_MATERIAL_CANVAS:
            p->canvas.texture0  = NU_NULL;
            p->canvas.wrap_mode = NU_TEXTURE_WRAP_CLAMP;
            break;
    }

    return handle;
}
static void
nugl__material_delete (nu_material_t mat)
{
}

static nu_mat3_t
nugl__material_surface_uv_transform (nu_material_t mat)
{
    if (mat)
    {
        return _ctx.gl.materials.data[NU_HANDLE_INDEX(mat)]
            .surface.uv_transform;
    }
    return nu_mat3_identity();
}
static GLuint
nugl__material_surface_texture0 (nu_material_t mat)
{
    if (mat)
    {
        nu_texture_t tex
            = _ctx.gl.materials.data[NU_HANDLE_INDEX(mat)].surface.texture0;
        if (tex)
        {
            return _ctx.gl.textures.data[NU_HANDLE_INDEX(tex)].texture;
        }
    }
    return 0;
}
static nu_color_t
nugl__material_surface_color (nu_material_t mat, nu_color_t vdefault)
{
    if (mat)
    {
        return _ctx.gl.materials.data[NU_HANDLE_INDEX(mat)].surface.color;
    }
    return vdefault;
}

#endif

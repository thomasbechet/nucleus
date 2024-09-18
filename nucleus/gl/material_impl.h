#ifndef NUGL_MATERIAL_IMPL_H
#define NUGL_MATERIAL_IMPL_H

#include <nucleus/internal.h>

static void
nugl__material_color0 (nu_material_t material, nu_texture_t color0)
{
    nugl__material_t *mat = _ctx.gl.materials.data + NU_HANDLE_INDEX(material);
    switch (mat->type)
    {
        case NU_MATERIAL_MESH:
            mat->mesh.texture0 = color0;
            break;
        case NU_MATERIAL_CANVAS:
            mat->canvas.texture0 = color0;
            break;
    }
}
static void
nugl__material_color1 (nu_material_t material, nu_texture_t color1)
{
    nugl__material_t *mat = _ctx.gl.materials.data + NU_HANDLE_INDEX(material);
    switch (mat->type)
    {
        case NU_MATERIAL_MESH:
            mat->mesh.texture1 = color1;
            break;
        default:
            break;
    }
}
static void
nugl__material_uv_transform (nu_material_t material, nu_mat3_t transform)
{
    nugl__material_t *mat = _ctx.gl.materials.data + NU_HANDLE_INDEX(material);
    switch (mat->type)
    {
        case NU_MATERIAL_MESH:
            mat->mesh.uv_transform = transform;
            break;
        default:
            break;
    }
}
static void
nugl__material_wrap_mode (nu_material_t material, nu_texture_wrap_mode_t mode)
{
    nugl__material_t *mat = _ctx.gl.materials.data + NU_HANDLE_INDEX(material);
    switch (mat->type)
    {
        case NU_MATERIAL_CANVAS:
            mat->canvas.wrap_mode = mode;
            break;
        default:
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
        case NU_MATERIAL_MESH:
            p->mesh.texture0     = NU_NULL;
            p->mesh.texture1     = NU_NULL;
            p->mesh.uv_transform = nu_mat3_identity();
            break;
        case NU_MATERIAL_CANVAS:
            p->canvas.texture0  = NU_NULL;
            p->canvas.wrap_mode = NU_TEXTURE_WRAP_CLAMP;
            break;
    }

    return handle;
}

#endif

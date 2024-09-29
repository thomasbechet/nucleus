#ifndef NUGL_MATERIAL_IMPL_H
#define NUGL_MATERIAL_IMPL_H

#include <nucleus/internal.h>

static void
nugl__material_color (nu_material_t          material,
                      nu_material_property_t prop,
                      nu_color_t             color)
{
    nugl__material_t *mat = _ctx.gl.materials.data + NU_HANDLE_INDEX(material);
    switch (mat->type)
    {
        case NU_MATERIAL_TYPE_SURFACE: {
            switch (prop)
            {
                case NU_MATERIAL_COLOR:
                    mat->surface.color = color;
                    return;
                default:
                    break;
            }
        }
        default:
            break;
    }
    NU_ERROR("unsupported material property");
}
static void
nugl__material_texture (nu_material_t          material,
                        nu_material_property_t prop,
                        nu_texture_t           texture)
{
    nugl__material_t *mat = _ctx.gl.materials.data + NU_HANDLE_INDEX(material);
    switch (mat->type)
    {
        case NU_MATERIAL_TYPE_SURFACE: {
            switch (prop)
            {
                case NU_MATERIAL_TEXTURE0:
                    mat->surface.texture0 = texture;
                    return;
                case NU_MATERIAL_TEXTURE1:
                    mat->surface.texture1 = texture;
                    return;
                default:
                    break;
            }
        }
        break;
        case NU_MATERIAL_TYPE_CANVAS: {
            switch (prop)
            {
                case NU_MATERIAL_TEXTURE0:
                    mat->canvas.texture0 = texture;
                    return;
                default:
                    break;
            }
        }
        break;
    }
    NU_ERROR("unsupported material property");
}
static void
nugl__material_mat3 (nu_material_t          material,
                     nu_material_property_t prop,
                     nu_mat3_t              matrix)
{
    nugl__material_t *mat = _ctx.gl.materials.data + NU_HANDLE_INDEX(material);
    switch (mat->type)
    {
        case NU_MATERIAL_TYPE_SURFACE: {
            switch (prop)
            {
                case NU_MATERIAL_UV_TRANSFORM:
                    mat->surface.uv_transform = matrix;
                    return;
                default:
                    break;
            }
        }
        default:
            break;
    }
    NU_ERROR("unsupported material property");
}
static void
nugl__material_wrap_mode (nu_material_t          material,
                          nu_material_property_t prop,
                          nu_texture_wrap_mode_t mode)
{
    nugl__material_t *mat = _ctx.gl.materials.data + NU_HANDLE_INDEX(material);
    switch (mat->type)
    {
        case NU_MATERIAL_TYPE_CANVAS: {
            switch (prop)
            {
                case NU_MATERIAL_WRAP_MODE:
                    mat->canvas.wrap_mode = mode;
                    return;
                default:
                    break;
            }
        }
        default:
            break;
    }
    NU_ERROR("unsupported material property");
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
        case NU_MATERIAL_TYPE_SURFACE:
            p->surface.color        = NU_COLOR_WHITE;
            p->surface.texture0     = NU_NULL;
            p->surface.texture1     = NU_NULL;
            p->surface.uv_transform = nu_mat3_identity();
            break;
        case NU_MATERIAL_TYPE_CANVAS:
            p->canvas.texture0  = NU_NULL;
            p->canvas.wrap_mode = NU_TEXTURE_WRAP_CLAMP;
            break;
    }

    return handle;
}

#endif

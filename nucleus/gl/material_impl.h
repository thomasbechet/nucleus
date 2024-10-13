#ifndef NUGL_MATERIAL_IMPL_H
#define NUGL_MATERIAL_IMPL_H

#include <nucleus/internal.h>

static nu_m3_t
nugl__material_surface_uv_transform (nu_material_t mat)
{
    if (mat)
    {
        return _ctx.graphics.materials.data[NU_HANDLE_INDEX(mat)]
            .surface.uv_transform;
    }
    return nu_m3_identity();
}
static GLuint
nugl__material_surface_texture0 (nu_material_t mat)
{
    if (mat)
    {
        nu_texture_t tex = _ctx.graphics.materials.data[NU_HANDLE_INDEX(mat)]
                               .surface.texture0;
        if (tex)
        {
            return _ctx.graphics.textures.data[NU_HANDLE_INDEX(tex)].gl.texture;
        }
    }
    return 0;
}
static nu_color_t
nugl__material_surface_color (nu_material_t mat, nu_color_t vdefault)
{
    if (mat)
    {
        return _ctx.graphics.materials.data[NU_HANDLE_INDEX(mat)].surface.color;
    }
    return vdefault;
}

#endif

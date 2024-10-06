#ifndef NUGL_SHADOW_IMPL_H
#define NUGL_SHADOW_IMPL_H

#include <nucleus/internal.h>

static void
nugl__shadow_reset (nugl__renderpass_shadow_t *pass)
{
    NU_VEC_CLEAR(&pass->cmds);
}
static void
nugl__shadow_create (nugl__renderpass_shadow_t *pass)
{
    NU_VEC_INIT(128, &pass->cmds);

    nugl__shadow_reset(pass);
}
static void
nugl__shadow_set_depth_map (nugl__renderpass_shadow_t *pass,
                            nu_texture_t               texture)
{
}
static void
nugl__shadow_render (nugl__renderpass_t *pass)
{
    nu__gl_t *gl = &_ctx.gl;
}

#endif

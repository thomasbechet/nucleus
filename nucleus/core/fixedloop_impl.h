#ifndef NU_FIXEDLOOP_IMPL_H
#define NU_FIXEDLOOP_IMPL_H

#include <nucleus/internal.h>

static void
nu__fixedloop_cleanup (void *data)
{
    nu_fixedloop_t handle = data;
    for (nu_size_t i = 0; i < _ctx.core.fixedloops.size; ++i)
    {
        if (handle == _ctx.core.fixedloops.data[i])
        {
            NU_VEC_SWAP_REMOVE(&_ctx.core.fixedloops, i);
        }
    }
}

nu_fixedloop_t
nu_fixedloop_new (nu_fixedloop_pfn_t callback, nu_f32_t timestep)
{
    nu_fixedloop_t *handle = NU_VEC_PUSH(&_ctx.core.fixedloops);
    if (!handle)
    {
        NU_ERROR("max fixedloop count reached");
        return NU_NULL;
    }
    nu__fixedloop_t *loop = nu_object_new(_ctx.core.obj_fixedloop);
    *handle               = (nu_fixedloop_t)loop;

    loop->active   = NU_TRUE;
    loop->timestep = timestep;
    loop->acc      = 0.0;
    loop->callback = callback;

    return *handle;
}
void
nu_fixedloop_update (nu_f32_t dt)
{
    for (nu_size_t i = 0; i < _ctx.core.fixedloops.size; ++i)
    {
        nu__fixedloop_t *loop = (nu__fixedloop_t *)_ctx.core.fixedloops.data[i];
        loop->acc += dt;

        if (loop->acc >= loop->timestep)
        {
            loop->acc -= loop->timestep;
            // TODO: prevent creation of fixedloop during fixedloop update
            loop->callback(loop->timestep);
        }
    }
}

#endif

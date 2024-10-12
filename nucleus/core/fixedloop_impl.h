#ifndef NU_FIXEDLOOP_IMPL_H
#define NU_FIXEDLOOP_IMPL_H

#include <nucleus/internal.h>

nu_fixedloop_t
nu_fixedloop_create (nu_fixedloop_callback_t callback, nu_f32_t timestep)
{
    nu_size_t        index;
    nu__fixedloop_t *loop = NU_POOL_ADD(&_ctx.core.fixedloops, &index);

    loop->used     = NU_TRUE;
    loop->active   = NU_TRUE;
    loop->timestep = timestep;
    loop->acc      = 0.0;
    loop->callback = callback;

    return NU_HANDLE_MAKE(nu_fixedloop_t, index);
}
void
nu_fixedloop_delete (nu_fixedloop_t loop)
{
    NU_POOL_REMOVE(&_ctx.core.fixedloops, NU_HANDLE_INDEX(loop));
}
void
nu_fixedloop_update (nu_f32_t dt)
{
    for (nu_size_t i = 0; i < _ctx.core.fixedloops.capacity; ++i)
    {
        nu__fixedloop_t *loop = _ctx.core.fixedloops.data + i;
        if (!loop->used)
        {
            continue;
        }
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

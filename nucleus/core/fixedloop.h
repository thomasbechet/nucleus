#ifndef NU_FIXEDLOOP_H
#define NU_FIXEDLOOP_H

#include <nucleus/core/api.h>

typedef struct
{
    nu_bool_t               active;
    nu_f32_t                timestep;
    nu_f32_t                acc;
    nu_fixedloop_callback_t callback;
    nu_fixedloop_t          next;
    nu_fixedloop_t          prev;
} nu__fixedloop_t;

#endif

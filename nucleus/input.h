#ifndef NU_INPUT_H
#define NU_INPUT_H

#include <nucleus/types.h>

NU_API nu_error_t nu_update_action(nu_context_t *ctx, nu_action_t *action);
NU_API nu_error_t nu_update_range(nu_context_t *ctx, nu_range_t *range);

#ifdef NU_IMPLEMENTATION

nu_error_t nu_update_action(nu_context_t *ctx, nu_action_t *action)
{
    return NU_ERROR_NONE;
}
nu_error_t nu_update_range(nu_context_t *ctx, nu_range_t *range)
{
    return NU_ERROR_NONE;
}

#endif

#endif

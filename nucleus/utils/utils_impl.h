#ifndef NU_UTILS_IMPL_H
#define NU_UTILS_IMPL_H

#include <nucleus/utils/geometry_impl.h>
#include <nucleus/utils/controller_impl.h>

static nu_error_t
nu__utils_init (void)
{
    NU_POOL_INIT(1, &_ctx.utils.controllers);
    NU_POOL_INIT(1, &_ctx.utils.geometries);
    return NU_ERROR_NONE;
}
static nu_error_t
nu__utils_free (void)
{
    // TODO: free resources
    NU_POOL_FREE(&_ctx.utils.controllers);
    NU_POOL_FREE(&_ctx.utils.geometries);
    return NU_ERROR_NONE;
}

#endif

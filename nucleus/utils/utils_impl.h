#ifndef NU_UTILS_IMPL_H
#define NU_UTILS_IMPL_H

#include <nucleus/utils/geometry_impl.h>
#include <nucleus/utils/loader_impl.h>
#include <nucleus/utils/controller_impl.h>
#include <nucleus/utils/ui_impl.h>

static nu_error_t
nu__utils_init (void)
{
    nu_pool_init(1, &_ctx.utils.uis);
    nu_pool_init(1, &_ctx.utils.controllers);
    return NU_ERROR_NONE;
}
static nu_error_t
nu__utils_free (void)
{
    // TODO: free resources
    nu_pool_free(&_ctx.utils.uis);
    nu_pool_free(&_ctx.utils.controllers);
    return NU_ERROR_NONE;
}

#endif

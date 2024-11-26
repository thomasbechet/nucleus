#ifndef NU_CORE_IMPL_H
#define NU_CORE_IMPL_H

#include <nucleus/internal.h>
#include <nucleus/core/color_impl.h>
#include <nucleus/core/logger_impl.h>
#include <nucleus/core/memory_impl.h>
#include <nucleus/core/time_impl.h>
#include <nucleus/core/string_impl.h>
#include <nucleus/core/ds_impl.h>
#include <nucleus/core/math_impl.h>
#include <nucleus/core/fixedloop_impl.h>
#include <nucleus/core/app_impl.h>
#include <nucleus/core/scope_impl.h>

static nu_error_t
nu__core_init (void)
{
    _ctx.core.allocator    = nu__allocator_stdlib();
    _ctx.core.logger.level = NU_LOG_INFO;
    nu__scope_init();
    _ctx.core.obj_fixedloop = nu_object_register(
        NU_STR(NU_FIXEDLOOP), sizeof(nu__fixedloop_t), nu__fixedloop_cleanup);
    NU_FIXEDVEC_ALLOC(&_ctx.core.fixedloops, 10);
    return NU_ERROR_NONE;
}
static nu_error_t
nu__core_free (void)
{
    nu__scope_free();
    return NU_ERROR_NONE;
}

#endif

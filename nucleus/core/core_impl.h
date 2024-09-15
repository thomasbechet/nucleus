#ifndef NU_CORE_IMPL_H
#define NU_CORE_IMPL_H

#include <nucleus/internal.h>
#include <nucleus/core/color_impl.h>
#include <nucleus/core/logger_impl.h>
#include <nucleus/core/memory_impl.h>
#include <nucleus/core/time_impl.h>
#include <nucleus/core/string_impl.h>
#include <nucleus/core/ds_impl.h>
#include <nucleus/core/table_impl.h>
#include <nucleus/core/math_impl.h>
#include <nucleus/core/context_impl.h>

static nu_error_t
nu__core_init (void)
{
    _ctx.core.allocator    = nu__allocator_stdlib();
    _ctx.core.logger.level = NU_LOG_DEBUG;
    nu_pool_init(10, &_ctx.core.tables);
    return NU_ERROR_NONE;
}
static nu_error_t
nu__core_free (void)
{
    nu_pool_free(&_ctx.core.tables);
    return NU_ERROR_NONE;
}

#endif

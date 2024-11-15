#ifndef NU_CORE_H
#define NU_CORE_H

#include <nucleus/core/logger.h>
#include <nucleus/core/memory.h>
#include <nucleus/core/fixedloop.h>
#include <nucleus/core/app.h>
#include <nucleus/core/object.h>

typedef struct
{
    nu__object_type_t object_types[NU_OBJECT_TYPE_MAX];
    nu_size_t         object_types_size;
    nu__scope_t       scopes[NU_SCOPE_MAX];
    nu_size_t         scopes_size;
    nu_scope_t        active_scope;

    nu__app_t            app;
    nu__logger_t         logger;
    nu_allocator_t       allocator;
    nu__fixedloop_pool_t fixedloops;
} nu__core_t;

static nu_error_t nu__core_init(void);
static nu_error_t nu__core_free(void);

#endif

#ifndef NU_CORE_H
#define NU_CORE_H

#include <nucleus/core/logger.h>
#include <nucleus/core/memory.h>
#include <nucleus/core/fixedloop.h>
#include <nucleus/core/app.h>
#include <nucleus/core/scope.h>

typedef struct
{

    nu__app_t      app;
    nu__logger_t   logger;
    nu_allocator_t allocator;
    nu_object_type_t    obj_fixedloop;
    NU_FIXEDVEC(nu_fixedloop_t) fixedloops;
    nu__scope_manager_t scope;
} nu__core_t;

static nu_error_t nu__core_init(void);
static nu_error_t nu__core_free(void);

#endif

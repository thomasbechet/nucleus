#ifndef NU_CORE_H
#define NU_CORE_H

#include <nucleus/core/logger.h>
#include <nucleus/core/memory.h>
#include <nucleus/core/fixedloop.h>
#include <nucleus/core/table.h>
#include <nucleus/core/app.h>

typedef struct
{
    nu__app_t            app;
    nu__logger_t         logger;
    nu_allocator_t       allocator;
    nu__fixedloop_pool_t fixedloops;
    nu__table_pool_t     tables;
} nu__core_t;

static nu_error_t nu__core_init(void);
static nu_error_t nu__core_free(void);

#endif

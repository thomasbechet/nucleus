#ifndef NU_CORE_INTERNAL_H
#define NU_CORE_INTERNAL_H

#include <nucleus/core/memory.h>
#include <nucleus/core/logger.h>

static nu_allocator_t nu__allocator_stdlib(void);

typedef struct
{
    nu_log_level_t level;
} nu__logger_t;

typedef struct
{
    nu__logger_t   logger;
    nu_allocator_t allocator;
} nu__core_t;

#endif

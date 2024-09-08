#ifndef NU_CORE_INTERNAL_H
#define NU_CORE_INTERNAL_H

#include <nucleus/core/memory.h>
#include <nucleus/core/logger.h>
#include <nucleus/core/ds.h>
#include <nucleus/core/table.h>

//////////////////////////////////////////////////////////////////////////
//////                          Allocator                           //////
//////////////////////////////////////////////////////////////////////////

static nu_allocator_t nu__allocator_stdlib(void);

//////////////////////////////////////////////////////////////////////////
//////                          Logger                              //////
//////////////////////////////////////////////////////////////////////////

typedef struct
{
    nu_log_level_t level;
} nu__logger_t;

//////////////////////////////////////////////////////////////////////////
//////                          Table                               //////
//////////////////////////////////////////////////////////////////////////

typedef struct
{
    nu_table_type_t type;
    union
    {
        nu_u32_t *ints;
        float    *floats;
    };
} nu__table_entry_t;

typedef struct
{
    nu_size_t size;
} nu__table_t;

typedef nu_pool(nu__table_t) nu__table_pool_t;

//////////////////////////////////////////////////////////////////////////
//////                          Module                              //////
//////////////////////////////////////////////////////////////////////////

typedef struct
{
    nu__logger_t     logger;
    nu_allocator_t   allocator;
    nu__table_pool_t tables;
} nu__core_t;

static nu_error_t nu__core_init(void);
static nu_error_t nu__core_free(void);

#endif

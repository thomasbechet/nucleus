#ifndef NU_TABLE_H
#define NU_TABLE_H

#include <nucleus/core/api.h>

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

#endif

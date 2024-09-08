#ifndef NU_TABLE_H
#define NU_TABLE_H

#include <nucleus/core/string.h>

NU_DEFINE_HANDLE(nu_table_t);

typedef enum
{
    NU_TABLE_INT,
    NU_TABLE_FLOAT,
} nu_table_type_t;

typedef struct
{
    const nu_char_t **names;
    nu_table_type_t  *types;
    nu_size_t         size;
} nu_table_info_t;

NU_API nu_table_t nu_table_create(const nu_table_info_t *info);
NU_API nu_table_t nu_table_delete(nu_table_t table);
NU_API nu_size_t  nu_table_size(nu_table_t table);
NU_API nu_u32_t  *nu_table_int(nu_table_t table, const nu_char_t *name);
NU_API float     *nu_table_float(nu_table_t table, const nu_char_t *name);

#endif

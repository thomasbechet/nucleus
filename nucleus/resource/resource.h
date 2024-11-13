#ifndef NU_RES_H
#define NU_RES_H

#include <nucleus/resource/api.h>

typedef struct
{
    nu_str_t              name;
    nu_uid_t              uid;
    nu_resource_handler_t handler;
} nu__resource_type_t;

typedef struct
{
    nu_uid_t type;
    nu_uid_t group;
    nu_uid_t uid;
    void    *data;
} nu__resource_entry_t;

typedef struct
{
    NU_VEC(nu__resource_type_t) types;
    NU_VEC(nu__resource_entry_t) entries;
} nu__resource_t;

static nu_error_t nu__resource_init(void);
static nu_error_t nu__resource_free(void);

#endif

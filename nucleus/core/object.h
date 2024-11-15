#ifndef NU_OBJECT_H
#define NU_OBJECT_H

#include <nucleus/core/api.h>

typedef struct
{
    nu_str_t            name;
    nu_size_t           size;
    nu_object_handler_t handler;
} nu__object_type_t;

typedef struct
{
    void *data;
    void *ptr;
    void *end;
} nu__scope_t;

#endif

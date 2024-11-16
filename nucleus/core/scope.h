#ifndef NU_SCOPE_H
#define NU_SCOPE_H

#include <nucleus/core/api.h>

typedef struct
{
    nu_str_t            name;
    nu_size_t           size;
    nu_object_handler_t handler;
} nu__object_t;

typedef struct nu__object_header
{
    struct nu__object_header *prev;
    nu__object_t             *type;
} nu__object_header_t;

typedef struct nu_scope
{
    nu_str_t             name;
    nu__object_header_t *last_object;
    nu_byte_t           *start;
    nu_byte_t           *ptr;
    nu_byte_t           *end;
    nu_scope_t           prev;
} nu__scope_t;

typedef struct
{
    nu__object_t objects[NU_OBJECT_MAX];
    nu_size_t    objects_count;
    nu__scope_t  scopes[NU_SCOPE_MAX];
    nu_size_t    scopes_count;
    nu_scope_t   active_scope;
    nu_scope_t   last_scope;
} nu__scope_manager_t;

static void nu__scope_init(void);
static void nu__scope_cleanup_all(void);

#endif

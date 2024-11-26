#ifndef NU_SCOPE_H
#define NU_SCOPE_H

#include <nucleus/core/api.h>
#ifdef NU_BUILD_SERIA
#include <nucleus/seria/api.h>
#endif

typedef struct
{
    nu_str_t            name;
    nu_size_t           size;
    nu_object_cleanup_t cleanup;
#ifdef NU_BUILD_SERIA
    nu_object_seria_load_t load;
    nu_object_seria_save_t save;
#endif
} nu__object_type_t;

typedef struct nu__scope_header
{
    struct nu__scope_header *prev;
    nu_object_type_id_t      object_type; // if NULL, interpreted as frame stack
} nu__scope_header_t;

typedef struct
{
    nu_uid_t uid;
} nu__object_header_t;

typedef struct nu_scope
{
    nu_str_t            name;
    nu__scope_header_t *last_header;
    nu_byte_t          *start;
    nu_byte_t          *ptr;
    nu_byte_t          *end;
    nu_scope_id_t       prev;
    nu_bool_t           auto_cleanup;
} nu__scope_t;

typedef struct
{
    nu__object_type_t objects[NU_OBJECT_MAX];
    nu_size_t         objects_count;
    nu__scope_t       scopes[NU_SCOPE_MAX];
    nu_size_t         scopes_count;
    nu_scope_id_t     last_scope;
    nu_scope_id_t     core_scope;
    nu_scope_id_t     active_scope; // TODO: per thread scope
} nu__scope_manager_t;

static void nu__scope_init(void);
static void nu__scope_free(void);
static void nu__scope_cleanup_all_auto(void);

static nu__scope_t       *nu__scope(nu_scope_id_t id);
static nu__object_type_t *nu__object_type(nu_object_type_id_t id);

#endif

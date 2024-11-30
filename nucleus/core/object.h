#ifndef NU_OBJECT_H
#define NU_OBJECT_H

#include <nucleus/core/api.h>
#ifdef NU_BUILD_SERIA
#include <nucleus/seria/api.h>
#endif

typedef struct
{
    nu_str_t                name;
    nu_size_t               size;
    nu_object_cleanup_pfn_t cleanup;
#ifdef NU_BUILD_SERIA
    nu_object_seria_load_t load;
    nu_object_seria_save_t save;
#endif
} nu__object_type_t;

typedef enum
{
    NU__OBJECT_TAGGED = 1,
} nu__object_flag_t;

typedef struct nu__scope_header
{
    struct nu__scope_header *prev;
    nu_object_type_t         type; // if NULL, interpreted as frame stack
    nu__object_flag_t        flags;
} nu__scope_header_t;

typedef struct nu_scope
{
    nu_str_t            name;
    nu__scope_header_t *last_header;
    nu_byte_t          *start;
    nu_byte_t          *ptr;
    nu_byte_t          *end;
    nu_scope_t          prev;
    nu_bool_t           auto_cleanup;
} nu__scope_t;

typedef struct
{
    nu_object_t object;
    nu_uid_t    tag;
} nu__object_tag_t;

typedef struct
{
    nu_scope_t last_scope;
    nu_scope_t core_scope;
    nu_scope_t active_scope; // TODO: per thread scope
    NU_VEC(nu__object_tag_t) tags;
    nu_object_type_t obj_object_type;
    nu_object_type_t obj_scope;
} nu__object_manager_t;

static void nu__object_init(void);
static void nu__object_free(void);
static void nu__scope_cleanup_all_auto(void);

static nu__scope_t       *nu__scope(nu_scope_t scope);
static nu__object_type_t *nu__object_type(nu_object_type_t type);

#endif

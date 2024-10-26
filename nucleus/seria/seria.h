#ifndef NU_SERIA_H
#define NU_SERIA_H

#include <nucleus/seria/api.h>
#ifdef NU_BUILD_JSMN
#include <nucleus/seria/json.h>
#endif

typedef enum
{
    NU__SERIA_PRIMITIVE,
    NU__SERIA_STRUCT,
    NU__SERIA_ENUM,
} nu__seria_kind_t;

typedef struct
{
    const nu_char_t *name;
    nu_seria_type_t  type;
    nu_size_t        offset;
    nu_size_t        count;
    nu_seria_flag_t  flags;
} nu__seria_struct_field_t;

typedef struct
{
    const nu_char_t *name;
    nu_u32_t         value;
} nu__seria_enum_value_t;

typedef struct
{
    nu__seria_kind_t kind;
    const nu_char_t *name;
    nu_size_t        size;
    union
    {
        NU_VEC(nu__seria_struct_field_t) fields;
        NU_VEC(nu__seria_enum_value_t) values;
        nu_seria_primitive_t primitive;
    };
} nu__seria_type_t;

typedef struct
{
    union
    {
#ifdef NU_BUILD_JSMN
        nu__seria_json_t json;
#endif
    };
    nu_seria_format_t format;
} nu__seria_instance_t;

typedef struct
{
    NU_POOL(nu__seria_instance_t) instances;
    NU_VEC(nu__seria_type_t) types;
} nu__seria_t;

static void nu__seria_init(void);
static void nu__seria_free(void);

#endif

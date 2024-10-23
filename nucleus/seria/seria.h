#ifndef NU_SERIA_H
#define NU_SERIA_H

#include <nucleus/seria/api.h>
#include <nucleus/seria/reflect.h>

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
    nu_size_t        size;
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
    NU_VEC(nu__seria_struct_field_t) fields;
    NU_VEC(nu__seria_enum_value_t) values;
    nu_seria_primitive_t primitive;
} nu__seria_type_t;

static const nu_char_t *nu__seria_names[] = { "bool", "f32", "u32" };

typedef struct
{
    union
    {
        nu__seria_reflect_t reflect;
    };
} nu__seria_ctx_t;

typedef struct
{
    nu__seria_ctx_t ctx;
    nu_bool_t       is_open;
} nu__seria_instance_t;

typedef struct
{
    NU_POOL(nu__seria_instance_t) instances;
    NU_VEC(nu__seria_type_t) types;
} nu__seria_t;

static void nu__seria_init(void);
static void nu__seria_free(void);

#endif

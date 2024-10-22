#ifndef NU_SERIA_H
#define NU_SERIA_H

#include <nucleus/seria/api.h>
#include <nucleus/seria/reflect.h>

typedef enum
{
    NU__SERIA_BOOL,
    NU__SERIA_F32,
    NU__SERIA_U32,
} nu__seria_primitive_t;

typedef struct
{
    const nu_char_t  *name;
    nu_seria_layout_t layout;
    nu_size_t         offset;
    nu_size_t         size;
} nu__seria_accessor_t;

typedef struct
{
    const nu_char_t *name;
    nu_size_t        size;
    NU_VEC(nu__seria_accessor_t) accessors;
    nu_bool_t is_primitive;
    nu_int_t  primitive;
} nu__seria_layout_t;

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
    NU_VEC(nu__seria_layout_t) layouts;
} nu__seria_t;

static void nu__seria_init(void);
static void nu__seria_free(void);

#endif

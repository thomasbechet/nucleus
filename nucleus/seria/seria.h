#ifndef NU_SERIA_H
#define NU_SERIA_H

#include <nucleus/seria/api.h>

typedef enum
{
    NU__SERIA_PRIMITIVE,
    NU__SERIA_STRUCT,
    NU__SERIA_ENUM,
} nu__seria_kind_t;

typedef struct
{
    nu_str_t name;
    union
    {
        nu_seria_layout_t   layout;
        nu_object_type_id_t type;
    };
    nu_size_t       offset;
    nu_size_t       size;
    nu_seria_flag_t flags;
    nu_bool_t       is_objref;
} nu__seria_struct_field_t;

typedef struct
{
    nu_str_t name;
    nu_u32_t value;
} nu__seria_enum_value_t;

typedef struct
{
    nu__seria_kind_t kind;
    nu_str_t         name;
    nu_size_t        size;
    union
    {
        nu_seria_primitive_t primitive;
        struct
        {
            nu_size_t start;
            nu_size_t count;
        } fields;
        struct
        {
            nu_size_t start;
            nu_size_t count;
        } values;
    };
} nu__seria_layout_t;

typedef struct
{
    nu_bool_t       opened;
    nu_bool_t       fileopen;
    nu_str_t        filename;
    nu_seria_mode_t mode;
    nu_byte_t      *bytes;
    nu_byte_t      *end;
    nu_byte_t      *ptr;
} nu__seria_ctx_t;

typedef struct
{
    nu_object_type_id_t obj_seria;
    NU_FIXEDVEC(nu__seria_layout_t) layouts;
    NU_FIXEDVEC(nu__seria_struct_field_t) struct_fields;
    NU_FIXEDVEC(nu__seria_enum_value_t) enum_values;
    nu_seria_layout_t primitive_layouts[NU_SERIA_PRIMITIVE_COUNT];
} nu__seria_t;

static nu_byte_t *nu__seria_load_bytes(nu_str_t filename, nu_size_t *size);
static void       nu__seria_write_bytes(nu_str_t         filename,
                                        const nu_byte_t *bytes,
                                        nu_size_t        size);

static nu_byte_t nu__seria_read_1b(nu__seria_ctx_t *ctx);
static nu_u32_t  nu__seria_read_4b(nu__seria_ctx_t *ctx);
static void      nu__seria_write_1b(nu__seria_ctx_t *ctx, nu_byte_t v);
static void      nu__seria_write_4b(nu__seria_ctx_t *ctx, nu_u32_t v);

static void nu__seria_init(void);

#endif

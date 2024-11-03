#ifndef NU_SERIA_H
#define NU_SERIA_H

#include <nucleus/seria/api.h>
#include <nucleus/seria/nbin.h>
#ifdef NU_BUILD_SERIA_JSON
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
    nu_str_t        name;
    nu_seria_type_t type;
    nu_size_t       offset;
    nu_size_t       size;
    nu_seria_flag_t flags;
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
        NU_VEC(nu__seria_struct_field_t) fields;
        NU_VEC(nu__seria_enum_value_t) values;
        nu_seria_primitive_t primitive;
    };
} nu__seria_type_t;

typedef struct
{
    nu_seria_format_t format;
    nu_bool_t         opened;
    nu_bool_t         owned;
    nu_seria_mode_t   mode;
    nu_byte_t        *bytes;
    nu_byte_t        *end;
    nu_byte_t        *ptr;
    nu_seria_type_t   buf_type;
    nu_size_t         buf_remaining_size;
    union
    {
        nu__seria_nbin_t nbin;
#ifdef NU_BUILD_SERIA_JSON
        nu__seria_json_t json;
#endif
    };
} nu__seria_ctx_t;

typedef struct
{
    NU_POOL(nu__seria_ctx_t) instances;
    NU_VEC(nu__seria_type_t) types;
} nu__seria_t;

static nu_byte_t *nu__seria_load_bytes(nu_str_t filename, nu_size_t *size);
static void       nu__seria_write_bytes(nu_str_t         filename,
                                        const nu_byte_t *bytes,
                                        nu_size_t        size);

static void     nu__seria_seek(nu__seria_ctx_t *ctx, nu_size_t offset);
static nu_u32_t nu__seria_read_4b(nu__seria_ctx_t *ctx);
static void     nu__seria_write_4b(nu__seria_ctx_t *ctx, nu_u32_t v);

static void nu__seria_init(void);
static void nu__seria_free(void);

#endif

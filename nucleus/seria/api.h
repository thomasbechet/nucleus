#ifndef NU_SERIA_API_H
#define NU_SERIA_API_H

#include <nucleus/core/api.h>

#define NU_SERIA_STRUCT(layout, struct, ...)                                 \
    {                                                                        \
        typedef struct sstruct;                                              \
        const nu_seria_struct_field_t struct##_fields[] = { __VA_ARGS__ };   \
        (layout) = nu_seria_register_struct(NU_STR(#struct),                 \
                                            sizeof(struct),                  \
                                            struct##_fields,                 \
                                            NU_ARRAY_SIZE(struct##_fields)); \
    }
#define NU_SERIA_FIELD_STRUCT(field, field_struct, field_count)   \
    {                                                             \
        .name   = NU_STR(#field),                                 \
        .type   = { .kind = NU_SERIA_STRUCT, .s = field_struct }, \
        .count  = field_count,                                    \
        .flags  = NU_SERIA_REQUIRED,                              \
        .offset = offsetof(sstruct, field),                       \
    }
#define NU_SERIA_FIELD_ENUM(field, field_enum, field_count)   \
    {                                                         \
        .name   = NU_STR(#field),                             \
        .type   = { .kind = NU_SERIA_ENUM, .e = field_enum }, \
        .count  = field_count,                                \
        .flags  = NU_SERIA_REQUIRED,                          \
        .offset = offsetof(sstruct, field),                   \
    }
#define NU_SERIA_FIELD_PRIMITIVE(field, field_primitive, field_count)   \
    {                                                                   \
        .name   = NU_STR(#field),                                       \
        .type   = { .kind = NU_SERIA_PRIMITIVE, .p = field_primitive }, \
        .count  = field_count,                                          \
        .flags  = NU_SERIA_REQUIRED,                                    \
        .offset = offsetof(sstruct, field),                             \
    }
#define NU_SERIA_FIELD_REF(field, field_ref, field_count)   \
    {                                                       \
        .name   = NU_STR(#field),                           \
        .type   = { .kind = NU_SERIA_REF, .r = field_ref }, \
        .count  = field_count,                              \
        .flags  = NU_SERIA_REQUIRED,                        \
        .offset = offsetof(sstruct, field),                 \
    }

#define NU_SERIA_ENUM(layout, enum, ...)                                 \
    {                                                                    \
        const nu_seria_enum_value_t enum##_values[] = { __VA_ARGS__ };   \
        (layout) = nu_seria_register_enum(NU_STR(#enum),                 \
                                          sizeof(enum),                  \
                                          enum##_values,                 \
                                          NU_ARRAY_SIZE(enum##_values)); \
    }
#define NU_SERIA_VALUE(value_name, enum_value) \
    { .name = NU_STR(#value_name), .value = enum_value }

#define NU_VEC_READ(v, seria, layout)                         \
    {                                                         \
        nu_u32_t size = nu_seria_read_u32((seria));           \
        NU_VEC_RESIZE(v, size);                               \
        nu_seria_read((ser), (layout), (v)->size, (v)->data); \
    }
#define NU_VEC_WRITE(v, seria, layout)                           \
    {                                                            \
        nu_seria_write_u32((v)->size);                           \
        nu_seria_write((seria), (layout), (v)->size, (v)->data); \
    }

#define NU_DEFINE_SERIA_PRIMITIVE(ident, type)                              \
    NU_API void nu_seria_read_##ident(                                      \
        nu_seria_t seria, nu_str_t name, nu_size_t size, type *data);       \
    NU_API void nu_seria_write_##ident(                                     \
        nu_seria_t seria, nu_str_t name, nu_size_t size, const type *data); \
    NU_API type nu_seria_read_1##ident(nu_seria_t seria, nu_str_t name);    \
    NU_API void nu_seria_write_1##ident(                                    \
        nu_seria_t seria, nu_str_t name, type v);

NU_DEFINE_OBJECT(nu_seria_t);
NU_DEFINE_OBJECT(nu_seria_struct_t);
NU_DEFINE_OBJECT(nu_seria_enum_t);

typedef enum
{
    NU_SERIA_BYTE,
    NU_SERIA_U32,
    NU_SERIA_F32,
    NU_SERIA_V3,
    NU_SERIA_Q4,
} nu_seria_primitive_t;

typedef struct
{
    enum
    {
        NU_SERIA_PRIMITIVE,
        NU_SERIA_STRUCT,
        NU_SERIA_ENUM,
        NU_SERIA_REF,
        NU_SERIA_STR,
    } kind;
    union
    {
        nu_seria_primitive_t p;
        nu_seria_struct_t    s;
        nu_seria_enum_t      e;
        nu_object_type_t     r;
        nu_size_t            c; // string capacity
    };
} nu_seria_type_t;

typedef enum
{
    NU_SERIA_READ,
    NU_SERIA_WRITE
} nu_seria_mode_t;

typedef enum
{
    NU_SERIA_OPTIONAL,
    NU_SERIA_REQUIRED,
} nu_seria_flag_t;

typedef void (*nu_object_seria_load_t)(nu_seria_t seria, void *data);
typedef void (*nu_object_seria_save_t)(nu_seria_t seria, const void *data);

typedef struct
{
    nu_str_t        name;
    nu_seria_type_t type;
    nu_size_t       offset;
    nu_size_t       count;
    nu_seria_flag_t flags;
} nu_seria_struct_field_t;

typedef struct
{
    nu_str_t name;
    nu_u32_t value;
} nu_seria_enum_value_t;

NU_API nu_seria_struct_t
nu_seria_register_struct(nu_str_t                       name,
                         nu_size_t                      size,
                         const nu_seria_struct_field_t *fields,
                         nu_size_t                      field_count);
NU_API nu_seria_struct_t
nu_seria_register_enum(nu_str_t                     name,
                       nu_size_t                    size,
                       const nu_seria_enum_value_t *values,
                       nu_size_t                    value_count);

NU_API nu_size_t nu_seria_primitive_size(nu_seria_primitive_t primitive);
NU_API nu_size_t nu_seria_struct_size(nu_seria_struct_t sstruct);

NU_API void nu_seria_dump_struct_type(nu_seria_struct_t s);
NU_API void nu_seria_dump_enum_type(nu_seria_enum_t e);
NU_API void nu_seria_dump_struct(nu_seria_struct_t s,
                                 nu_size_t         count,
                                 void             *data);
NU_API void nu_seria_dump_enum(nu_seria_enum_t e, nu_size_t count, void *data);

NU_API nu_seria_t nu_seria_new_file(nu_str_t        filename,
                                    nu_seria_mode_t mode,
                                    nu_size_t       buffer_size);
NU_API nu_seria_t nu_seria_new_bytes(nu_seria_mode_t mode,
                                     nu_byte_t      *bytes,
                                     nu_size_t       size);

NU_API void nu_seria_begin(nu_seria_t seria);
NU_API void nu_seria_end(nu_seria_t seria);

NU_API void nu_seria_write_struct(nu_seria_t        seria,
                                  nu_str_t          name,
                                  nu_seria_struct_t s,
                                  nu_size_t         count,
                                  const void       *data);
NU_API void nu_seria_read_struct(nu_seria_t        seria,
                                 nu_str_t          name,
                                 nu_seria_struct_t s,
                                 nu_size_t         count,
                                 void             *data);
NU_API void nu_seria_write_enum(nu_seria_t      seria,
                                nu_str_t        name,
                                nu_seria_enum_t e,
                                nu_size_t       count,
                                const void     *data);
NU_API void nu_seria_read_enum(nu_seria_t      seria,
                               nu_str_t        name,
                               nu_seria_enum_t e,
                               nu_size_t       count,
                               void           *data);
NU_API void nu_seria_write_primitive(nu_seria_t           seria,
                                     nu_str_t             name,
                                     nu_seria_primitive_t primitive,
                                     nu_size_t            count,
                                     const void          *data);
NU_API void nu_seria_read_primitive(nu_seria_t           seria,
                                    nu_str_t             name,
                                    nu_seria_primitive_t primitive,
                                    nu_size_t            count,
                                    void                *data);
NU_API void nu_seria_write_ref(nu_seria_t         seria,
                               nu_str_t           name,
                               nu_size_t          count,
                               const nu_object_t *ref);
NU_API void nu_seria_read_ref(nu_seria_t       seria,
                              nu_str_t         name,
                              nu_object_type_t type,
                              nu_size_t        count,
                              nu_object_t     *ref);
NU_API void nu_seria_write_str(nu_seria_t       seria,
                               nu_str_t         name,
                               nu_size_t        capacity,
                               nu_size_t        count,
                               const nu_byte_t *buffer);
NU_API void nu_seria_read_str(nu_seria_t seria,
                              nu_str_t   name,
                              nu_size_t  capacity,
                              nu_size_t  count,
                              nu_byte_t *buffer);

NU_DEFINE_SERIA_PRIMITIVE(byte, nu_byte_t);
NU_DEFINE_SERIA_PRIMITIVE(u32, nu_u32_t);
NU_DEFINE_SERIA_PRIMITIVE(f32, nu_f32_t);
NU_DEFINE_SERIA_PRIMITIVE(v3, nu_v3_t);
NU_DEFINE_SERIA_PRIMITIVE(q4, nu_q4_t);

NU_API void nu_seria_write_1str(nu_seria_t seria, nu_str_t name, nu_str_t str);
NU_API nu_str_t nu_seria_read_1str(nu_seria_t seria,
                                   nu_str_t   name,
                                   nu_size_t  capacity,
                                   nu_byte_t *buffer);

NU_API void        nu_object_set_seria(nu_object_type_t       type,
                                       nu_object_seria_load_t load,
                                       nu_object_seria_save_t save);
NU_API nu_object_t nu_object_load(nu_object_type_t type, nu_seria_t seria);
NU_API void        nu_object_save(nu_object_t object, nu_seria_t seria);

#endif

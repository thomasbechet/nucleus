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
#define NU_SERIA_FIELD(field, field_layout, field_size) \
    { .name      = NU_STR(#field),                      \
      .layout    = field_layout,                        \
      .size      = field_size,                          \
      .flags     = NU_SERIA_REQUIRED,                   \
      .offset    = offsetof(sstruct, field),            \
      .is_objref = NU_FALSE }
#define NU_SERIA_OBJREF(field, object_type, field_size) \
    { .name      = NU_STR(#field),                      \
      .type      = object_type,                         \
      .size      = field_size,                          \
      .flags     = NU_SERIA_REQUIRED,                   \
      .offset    = offsetof(sstruct, field),            \
      .is_objref = NU_TRUE }

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

#define NU_DEFINE_SERIA_PRIMITIVE(ident, type)               \
    NU_API void nu_seria_read_##ident(                       \
        nu_seria_t seria, nu_size_t size, type *data);       \
    NU_API void nu_seria_write_##ident(                      \
        nu_seria_t seria, nu_size_t size, const type *data); \
    NU_API type nu_seria_read_1##ident(nu_seria_t seria);    \
    NU_API void nu_seria_write_1##ident(nu_seria_t seria, type v);

NU_DEFINE_OBJECT(nu_seria_t);
NU_DEFINE_OBJECT(nu_seria_layout_t);

#define NU_SERIA        "seria"
#define NU_SERIA_LAYOUT "seria_layout"

#define NU_SERIA_BYTE nu_seria_primitive(NU_SERIA_PRIMITIVE_BYTE)
#define NU_SERIA_U32  nu_seria_primitive(NU_SERIA_PRIMITIVE_U32)
#define NU_SERIA_F32  nu_seria_primitive(NU_SERIA_PRIMITIVE_F32)
#define NU_SERIA_STR  nu_seria_primitive(NU_SERIA_PRIMITIVE_STR)
#define NU_SERIA_V3   nu_seria_primitive(NU_SERIA_PRIMITIVE_V3)
#define NU_SERIA_Q4   nu_seria_primitive(NU_SERIA_PRIMITIVE_Q4)

typedef enum
{
    NU_SERIA_READ,
    NU_SERIA_WRITE
} nu_seria_mode_t;

typedef enum
{
    NU_SERIA_PRIMITIVE_BYTE  = 0,
    NU_SERIA_PRIMITIVE_U32   = 1,
    NU_SERIA_PRIMITIVE_F32   = 2,
    NU_SERIA_PRIMITIVE_STR   = 3,
    NU_SERIA_PRIMITIVE_V3    = 4,
    NU_SERIA_PRIMITIVE_Q4    = 5,
    NU_SERIA_PRIMITIVE_COUNT = 6
} nu_seria_primitive_t;

typedef enum
{
    NU_SERIA_OPTIONAL,
    NU_SERIA_REQUIRED,
} nu_seria_flag_t;

typedef void (*nu_object_seria_load_t)(nu_seria_t seria, void *data);
typedef void (*nu_object_seria_save_t)(nu_seria_t seria, const void *data);

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
} nu_seria_struct_field_t;

typedef struct
{
    nu_str_t name;
    nu_u32_t value;
} nu_seria_enum_value_t;

NU_API nu_seria_layout_t
nu_seria_register_struct(nu_str_t                       name,
                         nu_size_t                      size,
                         const nu_seria_struct_field_t *fields,
                         nu_size_t                      field_count);
NU_API nu_seria_layout_t
                         nu_seria_register_enum(nu_str_t                     name,
                                                nu_size_t                    size,
                                                const nu_seria_enum_value_t *values,
                                                nu_size_t                    value_count);
NU_API nu_seria_layout_t nu_seria_find_layout(nu_str_t name);
NU_API nu_str_t          nu_seria_name(nu_seria_layout_t layout);
NU_API nu_size_t         nu_seria_size(nu_seria_layout_t layout);

NU_API void nu_seria_dump_layout(nu_seria_layout_t layout);
NU_API void nu_seria_dump_values(nu_seria_layout_t layout,
                                 nu_size_t         size,
                                 void             *data);

NU_API nu_seria_t nu_seria_new_file(nu_str_t        filename,
                                    nu_seria_mode_t mode,
                                    nu_size_t       buffer_size);
NU_API nu_seria_t nu_seria_new_bytes(nu_seria_mode_t mode,
                                     nu_byte_t      *bytes,
                                     nu_size_t       size);

NU_API void nu_seria_write(nu_seria_t        seria,
                           nu_seria_layout_t layout,
                           nu_size_t         size,
                           const void       *data);
NU_API void nu_seria_read(nu_seria_t        seria,
                          nu_seria_layout_t layout,
                          nu_size_t         size,
                          void             *data);

NU_API nu_seria_layout_t nu_seria_primitive(nu_seria_primitive_t primitive);

NU_DEFINE_SERIA_PRIMITIVE(byte, nu_byte_t);
NU_DEFINE_SERIA_PRIMITIVE(u32, nu_u32_t);
NU_DEFINE_SERIA_PRIMITIVE(f32, nu_f32_t);
NU_DEFINE_SERIA_PRIMITIVE(v3, nu_v3_t);
NU_DEFINE_SERIA_PRIMITIVE(q4, nu_q4_t);

NU_API void        nu_seria_write_str(nu_seria_t seria, nu_str_t str);
NU_API nu_str_t    nu_seria_read_str(nu_seria_t seria,
                                     nu_size_t  capacity,
                                     nu_byte_t *buffer);
NU_API void        nu_seria_write_objref(nu_seria_t seria, nu_object_t objref);
NU_API nu_object_t nu_seria_read_objref(nu_seria_t          seria,
                                        nu_object_type_id_t type);

NU_API void        nu_object_set_seria(nu_object_type_id_t    type,
                                       nu_object_seria_load_t load,
                                       nu_object_seria_save_t save);
NU_API nu_object_t nu_object_load(nu_object_type_id_t type, nu_seria_t seria);
NU_API void        nu_object_save(nu_object_t object, nu_seria_t seria);

#endif

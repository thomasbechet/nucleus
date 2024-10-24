#ifndef NU_SERIA_API_H
#define NU_SERIA_API_H

#include <nucleus/core/api.h>

#ifdef NU_BUILD_JSMN
#include <nucleus/external/jsmn/jsmn.h>
#endif

#define NU_SERIA_STRUCT(name, struct, ...)                                  \
    {                                                                       \
        typedef struct stype;                                               \
        nu_seria_type_t t = nu_seria_register_struct(name, sizeof(struct)); \
        __VA_ARGS__                                                         \
    }

#define NU_SERIA_STRUCT_FIELD(name, type, size, flags, field) \
    nu_seria_register_struct_field(                           \
        t, name, type, size, flags, offsetof(stype, field));

#define NU_SERIA_ENUM(name, enum, ...)                                  \
    {                                                                   \
        nu_seria_type_t t = nu_seria_register_enum(name, sizeof(enum)); \
        __VA_ARGS__                                                     \
    }

#define NU_SERIA_ENUM_VALUE(name, value) \
    nu_seria_register_enum_value(t, name, value)

NU_DEFINE_HANDLE(nu_seria_t);
NU_DEFINE_HANDLE(nu_seria_type_t);

typedef enum
{
    NU_SERIA_PRIMITIVE_U32 = 0,
    NU_SERIA_PRIMITIVE_F32 = 1,
    NU_SERIA_PRIMITIVE_V3  = 2,
    NU_SERIA_PRIMITIVE_Q4  = 3,
} nu_seria_primitive_t;

typedef enum
{
    NU_SERIA_OPTIONAL,
    NU_SERIA_REQUIRED,
} nu_seria_flag_t;

static const nu_char_t *nu_seria_primitive_names[]
    = { "u32", "f32", "v3", "q4" };

#define NU_SERIA_U32 NU_HANDLE_MAKE(nu_seria_type_t, NU_SERIA_PRIMITIVE_U32)
#define NU_SERIA_F32 NU_HANDLE_MAKE(nu_seria_type_t, NU_SERIA_PRIMITIVE_F32)
#define NU_SERIA_V3  NU_HANDLE_MAKE(nu_seria_type_t, NU_SERIA_PRIMITIVE_V3)
#define NU_SERIA_Q4  NU_HANDLE_MAKE(nu_seria_type_t, NU_SERIA_PRIMITIVE_Q4)

NU_API nu_seria_t nu_seria_create(void);
NU_API void       nu_seria_delete(nu_seria_t seria);

NU_API void nu_seria_open_dumper(nu_seria_t seria);
#ifdef NU_BUILD_JSMN
NU_API void nu_seria_open_json(nu_seria_t       seria,
                               const jsmntok_t *toks,
                               nu_size_t        toks_count);
#endif
NU_API void nu_seria_close(nu_seria_t seria);

NU_API nu_seria_type_t nu_seria_register_struct(const nu_char_t *name,
                                                nu_size_t        size);
NU_API void            nu_seria_register_struct_field(nu_seria_type_t  type,
                                                      const nu_char_t *name,
                                                      nu_seria_type_t  fieldtype,
                                                      nu_size_t        size,
                                                      nu_seria_flag_t  flags,
                                                      nu_size_t        offset);
NU_API nu_seria_type_t nu_seria_register_enum(const nu_char_t *name,
                                              nu_size_t        size);
NU_API void            nu_seria_register_enum_value(nu_seria_type_t  type,
                                                    const nu_char_t *name,
                                                    nu_u32_t         value);
NU_API nu_seria_type_t nu_seria_type(const nu_char_t *name);

NU_API void nu_seria_dump_types(void);
NU_API void nu_seria_dump(nu_seria_type_t type, nu_size_t size, void *data);

typedef nu_u64_t nu_seria_tag_t;

NU_API nu_seria_tag_t nu_seria_begin_tag(nu_seria_t seria);
NU_API nu_size_t      nu_seria_read(nu_seria_t      seria,
                                    nu_seria_type_t type,
                                    nu_size_t       capacity,
                                    void           *data);
NU_API void           nu_seria_write(nu_seria_t      seria,
                                     nu_seria_type_t type,
                                     nu_size_t       count,
                                     void           *data);

#endif

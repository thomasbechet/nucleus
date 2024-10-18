#ifndef NU_SERIA_API_H
#define NU_SERIA_API_H

#include <nucleus/core/api.h>

#define NU_SERIA_FIELD(struct, field, type, count...)                  \
    {                                                                  \
        .name = #field, type, .offset = offsetof(struct, field), count \
    }

#define NU_SERIA(name, struct, ...)                                   \
    static nu_seria_field_t name##_seria_fields[] = { __VA_ARGS__ };  \
    static nu_seria_t       name##_seria                              \
        = { #name,                                                    \
            sizeof(struct),                                           \
            name##_seria_fields,                                      \
            sizeof(name##_seria_fields) / sizeof(nu_seria_field_t) }; \
    const nu_seria_t *nu_##name##_seria(void)                         \
    {                                                                 \
        return &name##_seria;                                         \
    }

typedef enum
{
    NU_SERIA_BOOL,
    NU_SERIA_U32,
    NU_SERIA_F32,
    NU_SERIA_V3U,
    NU_SERIA_V3F,
    NU_SERIA_Q4,
    NU_SERIA_STR,
    NU_SERIA_OBJ,
} nu_seria_type_t;

struct nu_serial;

typedef struct
{
    const nu_char_t *name;
    nu_seria_type_t  type;
    nu_size_t        offset;
    nu_size_t        count; // if array or string type
                            // must be 0 or 1 for non array or string type
    struct nu_seria *seria; // if object type
} nu_seria_field_t;

typedef struct nu_seria
{
    const nu_char_t        *name;
    nu_size_t               size;
    const nu_seria_field_t *fields;
    nu_size_t               field_count;
} nu_seria_t;

NU_API void nu_seria_bin_read(const nu_seria_t *seria,
                              void             *data,
                              const nu_byte_t  *bytes,
                              nu_size_t         size);

#endif

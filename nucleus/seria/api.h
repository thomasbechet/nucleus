#ifndef NU_SERIA_API_H
#define NU_SERIA_API_H

#include <nucleus/core/api.h>

#ifdef NU_BUILD_JSMN
#include <nucleus/external/jsmn/jsmn.h>
#endif

#define NU_SERIA_FIELD(field, type, count)      \
    nu_seria_field(seria, #field, type, count); \
    nu_seria_data(seria, &v->field);

#define NU_SERIA_FIELD_OBJ(field, count, fn)            \
    nu_seria_field(seria, #field, NU_SERIA_OBJ, count); \
    fn(seria, &v->field);

#define NU_SERIA(name, struct, ...)                    \
    void nu_seria_##name(nu_seria_t seria, void *data) \
    {                                                  \
        struct *v = (struct *)data;                    \
        nu_seria_begin(seria, #name);                  \
        __VA_ARGS__                                    \
        nu_seria_end(seria);                           \
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

typedef enum
{
    NU_SERIA_READ,
    NU_SERIA_WRITE,
} nu_seria_mode_t;

NU_DEFINE_HANDLE(nu_seria_t);

NU_API nu_seria_t nu_seria_create(void);
NU_API void       nu_seria_delete(nu_seria_t seria);

NU_API void nu_seria_open_dumper(nu_seria_t seria);
#ifdef NU_BUILD_JSMN
NU_API void nu_seria_open_json(nu_seria_t       seria,
                               const jsmntok_t *toks,
                               nu_size_t        toks_count);
#endif
NU_API void nu_seria_close(nu_seria_t seria);

NU_API void nu_seria_begin(nu_seria_t seria, const nu_char_t *name);
NU_API void nu_seria_end(nu_seria_t seria);
NU_API void nu_seria_field(nu_seria_t       seria,
                           const nu_char_t *name,
                           nu_seria_type_t  type,
                           nu_size_t        size);
NU_API void nu_seria_data(nu_seria_t seria, void *data);

#endif

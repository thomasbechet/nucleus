#ifndef NU_SERIA_API_H
#define NU_SERIA_API_H

#include <nucleus/core/api.h>

#ifdef NU_BUILD_JSMN
#include <nucleus/external/jsmn/jsmn.h>
#endif

#define NU_SERIA_ACCESS(field, layout, size) \
    nu_seria_layout_access(layout, #field, offsetof(type, field), layout, size);

#define NU_SERIA(struct, ...)                                  \
    {                                                          \
        typedef struct type;                                   \
        nu_seria_layout_t layout                               \
            = nu_seria_layout_create(#struct, sizeof(struct)); \
        __VA_ARGS__                                            \
    }

#define NU_SERIA_LAYOUT(struct) nu_seria_layout(#struct)

NU_DEFINE_HANDLE(nu_seria_t);
NU_DEFINE_HANDLE(nu_seria_layout_t);

NU_API nu_seria_t nu_seria_create(void);
NU_API void       nu_seria_delete(nu_seria_t seria);

NU_API void nu_seria_open_dumper(nu_seria_t seria);
#ifdef NU_BUILD_JSMN
NU_API void nu_seria_open_json(nu_seria_t       seria,
                               const jsmntok_t *toks,
                               nu_size_t        toks_count);
#endif
NU_API void nu_seria_close(nu_seria_t seria);

NU_API nu_seria_layout_t nu_seria_layout_create(const nu_char_t *name,
                                                nu_size_t        size);
NU_API void              nu_seria_layout_access(nu_seria_layout_t layout,
                                                const nu_char_t  *name,
                                                nu_size_t         offset,
                                                nu_seria_layout_t access_layout,
                                                nu_size_t         size);
NU_API nu_seria_layout_t nu_seria_layout(const nu_char_t *name);

NU_API nu_size_t nu_seria_read(nu_seria_t        seria,
                               nu_seria_layout_t layout,
                               nu_size_t         capacity,
                               void             *data);

#endif

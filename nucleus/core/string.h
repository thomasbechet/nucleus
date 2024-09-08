#ifndef NU_STRING_H
#define NU_STRING_H

#include <nucleus/core/config.h>

#define NU_MATCH(a, b) (nu_strcmp(a, b) == 0)

NU_API nu_size_t  nu_strnlen(const nu_char_t *str, nu_size_t maxlen);
NU_API nu_size_t  nu_strlen(const nu_char_t *str);
NU_API nu_size_t  nu_strncmp(const nu_char_t *s1,
                             const nu_char_t *s2,
                             nu_size_t        n);
NU_API nu_size_t  nu_strcmp(const nu_char_t *s1, const nu_char_t *s2);
NU_API nu_char_t *nu_strncpy(nu_char_t *dst, const nu_char_t *src, nu_size_t n);

NU_API nu_u32_t nu_hash(const nu_char_t *s);
NU_API nu_u32_t nu_hashn(const nu_char_t *s, nu_size_t n);

typedef nu_u32_t nu_uid_t;

#define nu_uid(name) nu_hash(name)

typedef enum
{
    NU_FILENAME_EXT_GLTF,
    NU_FILENAME_EXT_PNG,
    NU_FILENAME_EXT_JPEG,
    NU_FILENAME_EXT_UNKNOWN,
} nu_filename_ext_t;

NU_API nu_filename_ext_t nu_filename_ext(const nu_char_t *filename);

#endif

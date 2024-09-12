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
NU_API nu_int_t   nu_snprintf(nu_char_t       *s,
                              nu_size_t        n,
                              const nu_char_t *format,
                              ...);

NU_API nu_u32_t nu_hash(const nu_char_t *s);
NU_API nu_u32_t nu_hashn(const nu_char_t *s, nu_size_t n);

typedef nu_u32_t nu_uid_t;

#define nu_uid(name) nu_hash(name)

#define NUEXT_PATH_MAX 256

typedef enum
{
    NUEXT_EXTENSION_GLTF,
    NUEXT_EXTENSION_PNG,
    NUEXT_EXTENSION_JPEG,
    NUEXT_EXTENSION_UNKNOWN,
} nuext_extension_t;

NU_API nuext_extension_t nuext_path_extension(const nu_char_t *filename);
NU_API void nuext_path_basename(const nu_char_t *path, nu_char_t *name);
NU_API void nuext_path_dirname(const nu_char_t *path, nu_char_t *name);
NU_API void nuext_path_concat(const nu_char_t *p1,
                              const nu_char_t *p2,
                              nu_char_t       *path);

#endif

#ifndef NU_STRING_H
#define NU_STRING_H

#include <nucleus/vm/types.h>
#include <nucleus/vm/platform.h>
#include <nucleus/vm/memory.h>
#include <nucleus/vm/math.h>

#define NU_IDENT_MAX   30
#define NU_IDENT_SIZE  (NU_IDENT_MAX + 2)
#define NU_MATCH(a, b) (nu_strcmp(a, b) == 0)

typedef nu_u8_t  nu_ident_t[NU_IDENT_SIZE];
typedef nu_u32_t nu_uid_t;

NU_API nu_size_t nu_ident_len(const nu_ident_t ident);
NU_API void      nu_ident_empty(nu_ident_t ident);
NU_API void      nu_ident_set(nu_ident_t ident, const nu_u8_t *s, nu_size_t n);
NU_API void      nu_ident_set_str(nu_ident_t ident, const char *s);
NU_API const nu_char_t *nu_ident_str(nu_ident_t ident);
NU_API nu_uid_t         nu_ident_uid(nu_ident_t ident);

NU_API nu_size_t nu_strnlen(const nu_char_t *str, nu_size_t maxlen);
NU_API nu_size_t nu_strlen(const nu_char_t *str);
NU_API nu_size_t nu_strncmp(const nu_char_t *s1,
                            const nu_char_t *s2,
                            nu_size_t        n);
NU_API nu_size_t nu_strcmp(const nu_char_t *s1, const nu_char_t *s2);

NU_API nu_uid_t nu_uid(const nu_char_t *str);
NU_API nu_uid_t nu_uidn(const nu_char_t *str, nu_size_t n);

/* Fast FNV1A hash algorithm taken from
 * https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function */

#define NU_FNV1A_HASH_32  0x811c9dc5
#define NU_FNV1A_PRIME_32 0x01000193

NU_API nu_u32_t nu_fnv1a_hash_32(const nu_u8_t *bytes, nu_size_t len);

#ifdef NU_IMPL

/*
 * Change the ident size. len must be in [0, NU_IDENT_MAX[ range.
 * If the string is expanded, filled data is undefined.
 */
static void
nu__ident_set_len (nu_ident_t ident, nu_size_t len)
{
    ident[NU_IDENT_MAX + 1] = (nu_u8_t)len;
    nu_memset(ident + len, 0, NU_IDENT_MAX - len);
}

nu_size_t
nu_ident_len (const nu_ident_t ident)
{
    return (nu_size_t)(*(ident + NU_IDENT_MAX + 1));
}

void
nu_ident_empty (nu_ident_t ident)
{
    nu_memset(ident, 0, NU_IDENT_SIZE);
    /* len implicitly set by memset */
}

void
nu_ident_set (nu_ident_t ident, const nu_u8_t *s, nu_size_t n)
{
    n = NU_MIN(NU_IDENT_SIZE, n);
    nu__ident_set_len(ident, n);
    nu_memcpy(ident, s, n);
}

void
nu_ident_set_str (nu_ident_t ident, const char *s)
{
    nu_size_t n = nu_strnlen(s, NU_IDENT_MAX);
    nu__ident_set_len(ident, n);
    nu_memcpy(ident, s, n);
}

const nu_char_t *
nu_ident_str (nu_ident_t ident)
{
    return (const nu_char_t *)ident;
}

nu_uid_t
nu_ident_uid (nu_ident_t ident)
{
    return nu_fnv1a_hash_32(ident, nu_ident_len(ident));
}

nu_size_t
nu_strnlen (const nu_char_t *str, nu_size_t maxlen)
{
    const nu_char_t *p = str;
    while (maxlen-- > 0 && *p)
    {
        p++;
    }
    return p - str;
}

nu_size_t
nu_strlen (const nu_char_t *str)
{
    const nu_char_t *p = str;
    while (*p)
    {
        p++;
    }
    return p - str;
}

nu_size_t
nu_strncmp (const nu_char_t *s1, const nu_char_t *s2, nu_size_t n)
{
    while (n && *s1 && (*s1 == *s2))
    {
        ++s1;
        ++s2;
        --n;
    }
    if (n == 0)
    {
        return 0;
    }
    else
    {
        return (*(nu_char_t *)s1 - *(nu_char_t *)s2);
    }
}
nu_size_t
nu_strcmp (const nu_char_t *s1, const nu_char_t *s2)
{
    while (*s1 && (*s1 == *s2))
    {
        s1++;
        s2++;
    }
    return *(nu_char_t *)s1 - *(nu_char_t *)s2;
}

nu_uid_t
nu_uid (const nu_char_t *str)
{
    return nu_fnv1a_hash_32((nu_u8_t *)str, nu_strlen(str));
}
nu_uid_t
nu_uidn (const nu_char_t *str, nu_size_t n)
{
    return nu_fnv1a_hash_32((nu_u8_t *)str, n);
}

nu_u32_t
nu_fnv1a_hash_32 (const nu_u8_t *bytes, nu_size_t len)
{
    nu_u32_t  hash = NU_FNV1A_HASH_32;
    nu_size_t i    = 0;
    while (i < len)
    {
        hash ^= (nu_u32_t)bytes[i];
        hash *= NU_FNV1A_PRIME_32;
        i++;
    }
    return hash;
}

#endif

#endif

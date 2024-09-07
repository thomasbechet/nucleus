#ifndef NU_STRING_IMPL_H
#define NU_STRING_IMPL_H

#include <nucleus/core/string.h>

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
void
nu_strncpy (nu_char_t *dst, const nu_char_t *src, nu_size_t n)
{
    while (n-- && (*dst++ = *src++))
        ;
}

nu_u32_t
nu_hash (const nu_char_t *s)
{
    return nu_hashn(s, nu_strlen(s));
}
nu_u32_t
nu_hashn (const nu_char_t *s, nu_size_t n)
{
    static const nu_u32_t FNV1A_HASH_32  = 0x811c9dc5;
    static const nu_u32_t FNV1A_PRIME_32 = 0x01000193;
    nu_u32_t              hash           = FNV1A_HASH_32;
    nu_size_t             i              = 0;
    while (i < n)
    {
        hash ^= s[i];
        hash *= FNV1A_PRIME_32;
        ++i;
    }
    return hash;
}

nu_filename_ext_t
nu_filename_ext (const nu_char_t *filename)
{
    const char *dot = strrchr(filename, '.');
    if (!dot || dot == filename)
    {
        return NU_FILENAME_EXT_UNKNOWN;
    }
    const nu_char_t *ext = dot + 1;
    if (NU_MATCH(ext, "gltf"))
    {
        return NU_FILENAME_EXT_GLTF;
    }
    else if (NU_MATCH(ext, "png"))
    {
        return NU_FILENAME_EXT_PNG;
    }
    else if (NU_MATCH(ext, "jpeg"))
    {
        return NU_FILENAME_EXT_JPEG;
    }
    return NU_FILENAME_EXT_UNKNOWN;
}

#endif

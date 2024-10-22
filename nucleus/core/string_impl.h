#ifndef NU_STRING_IMPL_H
#define NU_STRING_IMPL_H

#include <nucleus/internal.h>

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
nu_char_t *
nu_strncpy (nu_char_t *dst, const nu_char_t *src, nu_size_t n)
{
    if (n != 0)
    {
        char       *d = dst;
        const char *s = src;

        do
        {
            if ((*d++ = *s++) == 0)
            {
                /* NUL pad the remaining n-1 bytes */
                while (--n != 0)
                {
                    *d++ = 0;
                }
                break;
            }
        } while (--n != 0);
    }
    return dst;
}
nu_int_t
nu_snprintf (nu_char_t *s, nu_size_t n, const nu_char_t *format, ...)
{
#ifdef NU_STDLIB
    va_list args;
    va_start(args, format);
    nu_int_t r = nu_vsnprintf(s, n, format, args);
    va_end(args);
    return r;
#endif
}
nu_int_t
nu_vsnprintf (nu_char_t *s, nu_size_t n, const nu_char_t *format, va_list args)
{
#ifdef NU_STDLIB
    return vsnprintf(s, n, format, args);
#endif
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

nuext_extension_t
nuext_path_extension (const nu_char_t *filename)
{
    const char *dot = strrchr(filename, '.');
    if (!dot || dot == filename)
    {
        return NUEXT_EXTENSION_UNKNOWN;
    }
    const nu_char_t *ext = dot + 1;
    if (NU_MATCH(ext, "gltf"))
    {
        return NUEXT_EXTENSION_GLTF;
    }
    else if (NU_MATCH(ext, "png"))
    {
        return NUEXT_EXTENSION_PNG;
    }
    else if (NU_MATCH(ext, "jpeg"))
    {
        return NUEXT_EXTENSION_JPEG;
    }
    else if (NU_MATCH(ext, "json"))
    {
        return NUEXT_EXTENSION_JSON;
    }
    return NUEXT_EXTENSION_UNKNOWN;
}
void
nuext_path_basename (const nu_char_t *path, nu_char_t *name)
{
    nu_strncpy(name, path, NUEXT_PATH_MAX);
#ifdef NU_STDLIB
    name = basename((char *)name);
#endif
}
void
nuext_path_dirname (const nu_char_t *path, nu_char_t *name)
{
    nu_strncpy(name, path, NUEXT_PATH_MAX);
#ifdef NU_STDLIB
    name = dirname((char *)name);
#endif
}
void
nuext_path_concat (const nu_char_t *p1, const nu_char_t *p2, nu_char_t *path)
{
    nu_snprintf(path, NUEXT_PATH_MAX, "%s/%s", p1, p2);
}

#endif

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

#endif

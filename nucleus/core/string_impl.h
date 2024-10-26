#ifndef NU_STRING_IMPL_H
#define NU_STRING_IMPL_H

#include <nucleus/internal.h>

nu_size_t
nu_cstr_len (const char *str)
{
    const char *p = str;
    while (*p)
    {
        p++;
    }
    return p - str;
}
nu_str_t
nu_str_from_bytes (nu_byte_t *bytes, nu_size_t n)
{
    nu_str_t str;
    str.data = bytes;
    str.size = n;
    return str;
}
void
nu_str_to_cstr (nu_str_t str, char *chars, nu_size_t n)
{
    NU_ASSERT(str.size < n);
    nu_memset(chars, 0, n);
    nu_memcpy(chars, str.data, str.size);
}
nu_bool_t
nu_str_eq (nu_str_t s1, nu_str_t s2)
{
    return (s1.size == s2.size && nu_memcmp(s1.data, s2.data, s1.size) == 0);
}
nu_u32_t
nu_str_hash (nu_str_t s)
{
    static const nu_u32_t FNV1A_HASH_32  = 0x811c9dc5;
    static const nu_u32_t FNV1A_PRIME_32 = 0x01000193;
    nu_u32_t              hash           = FNV1A_HASH_32;
    nu_size_t             i              = 0;
    while (i < s.size)
    {
        hash ^= s.data[i];
        hash *= FNV1A_PRIME_32;
        ++i;
    }
    return hash;
}
nu_bool_t
nu_str_next (nu_str_t s, nu_size_t *it, nu_wchar_t *c)
{
    if (*it >= s.size)
    {
        return NU_FALSE;
    }
    *c = s.data[*it];
    ++(*it);
    return NU_TRUE;
}
nu_str_t
nu_snprintf (nu_byte_t *buf, nu_size_t n, nu_str_t format, ...)
{
#ifdef NU_STDLIB
    va_list args;
    va_start(args, format);
    nu_str_t r = nu_vsnprintf(buf, n, format, args);
    va_end(args);
    return r;
#endif
}
nu_str_t
nu_vsnprintf (nu_byte_t *buf, nu_size_t n, nu_str_t format, va_list args)
{
#ifdef NU_STDLIB
    int r = vsnprintf((char *)buf, n, (char *)format.data, args);
    return nu_str_from_bytes(buf, r);
#endif
}

nuext_extension_t
nuext_path_extension (nu_str_t filename)
{
    const char *dot
        = strrchr((const char *)filename.data, '.'); // safe with utf-8
    if (!dot || dot == (const char *)filename.data)
    {
        return NUEXT_EXTENSION_UNKNOWN;
    }
    nu_str_t ext
        = nu_str_from_bytes((nu_byte_t *)(dot + 1), nu_cstr_len(dot + 1));
    if (NU_MATCH(ext, NU_STR("gltf")))
    {
        return NUEXT_EXTENSION_GLTF;
    }
    else if (NU_MATCH(ext, NU_STR("png")))
    {
        return NUEXT_EXTENSION_PNG;
    }
    else if (NU_MATCH(ext, NU_STR("jpeg")))
    {
        return NUEXT_EXTENSION_JPEG;
    }
    else if (NU_MATCH(ext, NU_STR("json")))
    {
        return NUEXT_EXTENSION_JSON;
    }
    return NUEXT_EXTENSION_UNKNOWN;
}
nu_str_t
nuext_path_basename (nu_str_t path)
{
    for (nu_size_t n = path.size; n; --n)
    {
        if (path.data[n - 1] == '/')
        {
            return nu_str_from_bytes(path.data + n, path.size - n);
        }
    }
    return path;
}
nu_str_t
nuext_path_dirname (nu_str_t path)
{
    for (nu_size_t n = path.size; n; --n)
    {
        if (path.data[n - 1] == '/')
        {
            return nu_str_from_bytes(path.data, n);
        }
    }
    return path;
}
nu_str_t
nuext_path_concat (nu_byte_t *buf, nu_size_t n, nu_str_t p1, nu_str_t p2)
{
    return nu_snprintf(buf,
                       n,
                       NU_STR(NU_STR_FORMAT "/" NU_STR_FORMAT),
                       NU_STR_ARGS(p1),
                       NU_STR_ARGS(p2));
}

#endif

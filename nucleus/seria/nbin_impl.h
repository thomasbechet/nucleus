#ifndef NU_SERIA_NBIN_IMPL_H
#define NU_SERIA_NBIN_IMPL_H

#include <nucleus/internal.h>

static nu_u16_t
nu__seria_u16_le (nu_u16_t v)
{
    if (NU_BIG_ENDIAN)
    {
        return (v >> 8) | (v << 8);
    }
    else
    {
        return v;
    }
}
static nu_u32_t
nu__seria_u32_le (nu_u32_t v)
{
    if (NU_BIG_ENDIAN)
    {
        return ((v >> 24) & 0xff) |      // move byte 3 to byte 0
               ((v << 8) & 0xff0000) |   // move byte 1 to byte 2
               ((v >> 8) & 0xff00) |     // move byte 2 to byte 1
               ((v << 24) & 0xff000000); // byte 0 to byte 3
    }
    else
    {
        return v;
    }
}
static nu_u64_t
nu__seria_u64_le (nu_u64_t v)
{
    if (NU_BIG_ENDIAN)
    {
        v = ((v << 8) & 0xFF00FF00FF00FF00ULL)
            | ((v >> 8) & 0x00FF00FF00FF00FFULL);
        v = ((v << 16) & 0xFFFF0000FFFF0000ULL)
            | ((v >> 16) & 0x0000FFFF0000FFFFULL);
        return (v << 32) | (v >> 32);
    }
    else
    {
        return v;
    }
}

static void
nu__seria_nbin_open (nu__seria_nbin_t *nbin,
                     nu_seria_io_t     mode,
                     nu_byte_t        *bytes,
                     nu_size_t         size)
{
    if (mode == NU_SERIA_READ)
    {
        // TODO: read version
        // TODO: read root buffer
    }
    else
    {
    }
}
static void
nu__seria_nbin_close (nu__seria_nbin_t *nbin)
{
}

static void
nu__seria_nbin_seek (nu__seria_nbin_t *nbin, nu_seria_buffer_t buffer)
{
    if (buffer)
    {
        nbin->it = NU_HANDLE_INDEX(buffer);
    }
    else
    {
        nbin->it = nbin->root;
    }
    // TODO: read buffer size
}
static nu_size_t
nu__seria_nbin_read (nu__seria_nbin_t *nbin,
                     nu_seria_type_t   type,
                     nu_size_t         count,
                     void             *data)
{
    // TODO: check seek buffer defined
    return 0;
}
static void
nu__seria_nbin_write_root (nu__seria_nbin_t *nbin, nu_seria_buffer_t buffer)
{
}
static nu_seria_buffer_t
nu__seria_nbin_write (nu__seria_nbin_t *nbin,
                      nu_seria_type_t   type,
                      nu_size_t         count,
                      void             *data)
{
    return NU_NULL;
}

#endif

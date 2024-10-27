#ifndef NU_SERIA_NBIN_IMPL_H
#define NU_SERIA_NBIN_IMPL_H

#include <nucleus/internal.h>

static void
nu__seria_nbin_open (nu__seria_nbin_t *nbin,
                     const nu_byte_t  *bytes,
                     nu_size_t         size)
{
}
static void
nu__seria_nbin_close (nu__seria_nbin_t *nbin)
{
}

static void
nu__seria_nbin_seek (nu__seria_nbin_t *nbin, nu_seria_buffer_t buffer)
{
}
static nu_size_t
nu__seria_nbin_read (nu__seria_nbin_t *nbin,
                     nu_seria_type_t   type,
                     nu_size_t         count,
                     void             *data)
{
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

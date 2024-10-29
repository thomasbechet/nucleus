#ifndef NU_SERIA_NBIN_H
#define NU_SERIA_NBIN_H

#include <nucleus/seria/api.h>

typedef struct
{
    nu_seria_io_t mode;
    nu_byte_t    *bytes;
    nu_size_t     bytes_size;

    nu_seria_buffer_t root;
    nu_byte_t        *it;
    nu_byte_t        *end;
} nu__seria_nbin_t;

#endif

#ifndef NU_SERIA_NBIN_H
#define NU_SERIA_NBIN_H

#include <nucleus/seria/api.h>

typedef struct
{
    nu_byte_t *bytes;
    nu_size_t  bytes_size;

    nu_size_t root;
    nu_size_t it;
    nu_size_t it_remaining;
} nu__seria_nbin_t;

#endif

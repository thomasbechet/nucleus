#ifndef NU_SERIA_NBIN_H
#define NU_SERIA_NBIN_H

#include <nucleus/seria/api.h>

typedef struct
{
    nu_byte_t *bytes;
    nu_size_t  bytes_size;
    nu_bool_t  owned;
} nu__seria_nbin_t;

#endif

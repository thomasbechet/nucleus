#ifndef NU_SERIA_NBIN_H
#define NU_SERIA_NBIN_H

#include <nucleus/seria/api.h>

typedef struct
{
    nu_seria_mode_t mode;
    nu_byte_t      *bytes;
    nu_size_t       bytes_size;

    nu_byte_t      *start;
    nu_byte_t      *it;
    nu_byte_t      *end;
    nu_seria_type_t type;
} nu__seria_nbin_t;

#endif

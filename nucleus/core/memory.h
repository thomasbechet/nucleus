#ifndef NU_MEMORY_H
#define NU_MEMORY_H

#include <nucleus/core/api.h>

static void *nu__malloc(nu_size_t size);
static void  nu__free(void *p);

#endif

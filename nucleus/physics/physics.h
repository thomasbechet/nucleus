#ifndef NU_PHYSICS_H
#define NU_PHYSICS_H

#include <nucleus/core/api.h>

typedef struct
{
    int todo;
} nu__physics_t;

static nu_error_t nu__physics_init(void);
static void       nu__physics_free(void);

#endif

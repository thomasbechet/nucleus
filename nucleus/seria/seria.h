#ifndef NU_SERIA_H
#define NU_SERIA_H

#include <nucleus/seria/api.h>

typedef enum
{
    NU__SERIA_CLOSED,
    NU__SERIA_DUMPER,
    NU__SERIA_JSON,
} nu__seria_type_t;

typedef struct
{
    nu__seria_type_t type;
    union
    {
        struct
        {
            nu_size_t depth;
        } dumper;
        struct
        {
            const jsmntok_t *toks;
            nu_size_t        toks_count;
            nu_size_t        index;
        } json;
    };
} nu__seria_instance_t;

typedef struct
{
    NU_POOL(nu__seria_instance_t) instances;
} nu__seria_t;

static void nu__seria_init(void);
static void nu__seria_free(void);

#endif

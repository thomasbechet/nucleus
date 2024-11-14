#define NU_IMPLEMENTATION
#include <nucleus/nucleus.h>

typedef enum
{
    NU_OBJECT_TEXTURE,
    NU_OBJECT_IMAGE,
    NU_OBJECT_ECS
} nu_object_type_t;

typedef struct
{
    nu_str_t  name;
    nu_size_t size;
    void (*load)(void *data, nu_seria_t seria);
    void (*save)(void *data, nu_seria_t seria);
} nu_object_type_info_t;

typedef struct
{
    nu_object_type_t type;
    nu_size_t        offset;
} nu_object_t;

typedef struct
{
    nu_size_t index;
} nu_ref_t;

typedef struct
{
    nu_size_t    stack_size;
    nu_object_t *stack;
    void        *data;
} nu_engine_t;

static void
nu_engine_init (void)
{
}

void
init (void)
{
}

void
nu_app (void)
{
    nu_app_init_callback(init);
}

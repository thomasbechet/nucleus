#define NU_IMPLEMENTATION
#include <nucleus/nucleus.h>

static nu_ecs_t ecs;

static nu_ecs_id_t COMP_TRANSFORM;
static nu_ecs_id_t COMP_PLAYER;

typedef struct
{
    int     hello;
    nu_v3_t vector;
    nu_q4_t quat;
} subtype_t;

typedef struct
{
    nu_v3_t position;
    nu_q4_t rotation;
    nu_v3_t scale;
} transform_t;

typedef struct
{
    nu_u32_t stat;
    nu_v3_t  v;
} player_t;

void
init (void)
{
    NU_SERIA_REG_STRUCT(subtype_t, NU_SERIA_FIELD(hello, NU_SERIA_U32, 1);
                        NU_SERIA_FIELD(vector, NU_SERIA_V3, 1);
                        NU_SERIA_FIELD(quat, NU_SERIA_Q4, 1));
    NU_SERIA_REG_STRUCT(transform_t, NU_SERIA_FIELD(position, NU_SERIA_V3, 1);
                        NU_SERIA_FIELD(rotation, NU_SERIA_Q4, 1);
                        NU_SERIA_FIELD(scale, NU_SERIA_V3, 1););
    NU_SERIA_REG_STRUCT(player_t, NU_SERIA_FIELD(stat, NU_SERIA_U32, 1);
                        NU_SERIA_FIELD(v, NU_SERIA_V3, 1));

    COMP_TRANSFORM = NU_ECS_REG_COMPONENT(transform, transform_t);
    COMP_PLAYER    = NU_ECS_REG_COMPONENT(player, player_t);

    ecs = nu_ecs_create();

    nu_ecs_id_t  e = nu_ecs_add(ecs);
    transform_t *p = nu_ecs_set(ecs, e, COMP_TRANSFORM);
    p->scale       = NU_V3_ONES;
    p->position    = NU_V3_ONES;
    p->rotation    = nu_q4_identity();
    nu_ecs_set(ecs, e, COMP_PLAYER);
    e = nu_ecs_add(ecs);
    nu_ecs_set(ecs, e, COMP_TRANSFORM);

    nu_seria_dump_layouts();
    nu_seria_dump_values(NU_SERIA_LAYOUT(transform_t), 1, p);

    const nu_size_t bytes_size = 1 << 14;
    nu_byte_t      *bytes      = nu_alloc(bytes_size);
    nu_seria_t      ser        = nu_seria_create();
    nu_memset(bytes, 0, bytes_size);

    nu_seria_open_bytes(ser, NU_SERIA_WRITE, NU_SERIA_NBIN, bytes, bytes_size);
    nu_ecs_write(ecs, ser);
    nu_size_t n = nu_seria_close(ser);
    nu__seria_write_bytes(NU_STR("dump.bin"), bytes, n);

    nu_seria_open_bytes(ser, NU_SERIA_READ, NU_SERIA_NBIN, bytes, n);
    nu_ecs_read(ecs, ser);
    nu_seria_close(ser);
}

void
nu_app (void)
{
    nu_app_init_callback(init);
}

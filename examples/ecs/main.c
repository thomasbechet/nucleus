#define NU_IMPLEMENTATION
#include <nucleus/nucleus.h>

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

static nu_ecs_t ecs;

static nu_seria_layout_t SUBTYPE_LAYOUT;
static nu_seria_layout_t TRANSFORM_LAYOUT;
static nu_seria_layout_t PLAYER_LAYOUT;

static nu_ecs_id_t   COMP_TRANSFORM;
static nu_ecs_id_t   COMP_PLAYER;
static nu_scope_id_t SCOPE;

void
init (void)
{
    NU_SERIA_STRUCT(SUBTYPE_LAYOUT,
                    subtype_t,
                    NU_SERIA_FIELD(hello, NU_SERIA_U32, 1),
                    NU_SERIA_FIELD(vector, NU_SERIA_V3, 1),
                    NU_SERIA_FIELD(quat, NU_SERIA_Q4, 1));
    NU_SERIA_STRUCT(TRANSFORM_LAYOUT,
                    transform_t,
                    NU_SERIA_FIELD(position, NU_SERIA_V3, 1),
                    NU_SERIA_FIELD(rotation, NU_SERIA_Q4, 1),
                    NU_SERIA_FIELD(scale, NU_SERIA_V3, 1));
    NU_SERIA_STRUCT(PLAYER_LAYOUT,
                    player_t,
                    NU_SERIA_FIELD(stat, NU_SERIA_U32, 1),
                    NU_SERIA_FIELD(v, NU_SERIA_V3, 1));

    NU_ECS_COMPONENT(COMP_TRANSFORM, transform, TRANSFORM_LAYOUT);
    NU_ECS_COMPONENT(COMP_PLAYER, player, PLAYER_LAYOUT);
    SCOPE = nu_scope_register(NU_STR("main"), NU_MEM_4M);
    nu_scope_set_active(SCOPE);

    ecs = nu_ecs_new(512);

    nu_ecs_id_t  e = nu_ecs_add(ecs);
    transform_t *p = nu_ecs_set(ecs, e, COMP_TRANSFORM);
    p->scale       = NU_V3_ONES;
    p->position    = NU_V3_ONES;
    p->rotation    = nu_q4_identity();
    nu_ecs_set(ecs, e, COMP_PLAYER);
    e = nu_ecs_add(ecs);
    nu_ecs_set(ecs, e, COMP_TRANSFORM);

    nu_seria_dump_layout(PLAYER_LAYOUT);

    NU_INFO("saved:");
    nu_ecs_dump(ecs);

    nu_seria_t ser = nu_seria_new();
    nu_seria_open_file(ser, NU_SERIA_WRITE, NU_STR("dump.bin"), NU_MEM_4M);
    nu_ecs_save(ecs, ser);
    nu_size_t n = nu_seria_close(ser);

    nu_seria_open_file(ser, NU_SERIA_READ, NU_STR("dump.bin"), NU_MEM_4M);
    ecs = nu_ecs_load(ser);
    nu_seria_close(ser);

    NU_INFO("loaded:");
    nu_ecs_dump(ecs);
}

void
nu_app (void)
{
    nu_app_init_callback(init);
}

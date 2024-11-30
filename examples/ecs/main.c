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

static nu_ecs_t g_ecs;

static nu_seria_struct_t g_struct_subtype;
static nu_seria_struct_t g_struct_transform;
static nu_seria_struct_t g_struct_player;

static nu_ecs_id_t g_comp_transform;
static nu_ecs_id_t g_comp_player;
static nu_scope_t  g_scope;

void
init (void)
{
    NU_SERIA_STRUCT(g_struct_subtype,
                    subtype_t,
                    NU_SERIA_FIELD_PRIMITIVE(hello, NU_SERIA_U32, 1),
                    NU_SERIA_FIELD_PRIMITIVE(vector, NU_SERIA_V3, 1),
                    NU_SERIA_FIELD_PRIMITIVE(quat, NU_SERIA_Q4, 1));
    NU_SERIA_STRUCT(g_struct_transform,
                    transform_t,
                    NU_SERIA_FIELD_PRIMITIVE(position, NU_SERIA_V3, 1),
                    NU_SERIA_FIELD_PRIMITIVE(rotation, NU_SERIA_Q4, 1),
                    NU_SERIA_FIELD_PRIMITIVE(scale, NU_SERIA_V3, 1));
    NU_SERIA_STRUCT(g_struct_player,
                    player_t,
                    NU_SERIA_FIELD_PRIMITIVE(stat, NU_SERIA_U32, 1),
                    NU_SERIA_FIELD_PRIMITIVE(v, NU_SERIA_V3, 1));

    NU_ECS_COMPONENT(g_comp_transform, transform, g_struct_transform);
    NU_ECS_COMPONENT(g_comp_player, player, g_struct_player);
    g_scope = nu_scope_new(NU_STR("main"), NU_MEM_16M);
    nu_scope_set_active(g_scope);

    g_ecs = nu_ecs_new(512);

    nu_ecs_id_t  e     = nu_ecs_add(g_ecs);
    transform_t *p     = nu_ecs_set(g_ecs, e, g_comp_transform);
    p->scale           = NU_V3_ONES;
    p->position        = NU_V3_ONES;
    p->rotation        = nu_q4_identity();
    nu_ecs_id_t ep     = e;
    player_t   *player = nu_ecs_set(g_ecs, e, g_comp_player);
    player->stat       = 0xFFFFFFFF;
    e                  = nu_ecs_add(g_ecs);
    nu_ecs_set(g_ecs, e, g_comp_transform);

    nu_seria_dump_struct_type(g_struct_player);

    NU_INFO("saved:");
    nu_ecs_dump(g_ecs);

    nu_scope_push();
    nu_seria_t ser
        = nu_seria_new_file(NU_STR("dump.bin"), NU_SERIA_WRITE, NU_MEM_4M);
    nu_ecs_save(g_ecs, ser);
    nu_scope_pop();

    ser    = nu_seria_new_file(NU_STR("dump.bin"), NU_SERIA_READ, NU_MEM_4M);
    g_ecs  = nu_ecs_load(ser);
    player = nu_ecs_get(g_ecs, ep, g_comp_player);

    NU_INFO("loaded:");
    nu_ecs_dump(g_ecs);
}

void
nu_app (void)
{
    nu_app_init_callback(init);
    nu_app_log_level(NU_LOG_DEBUG);
}

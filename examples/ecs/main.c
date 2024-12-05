#define NU_IMPLEMENTATION
#include <nucleus/nucleus.h>

typedef struct
{
    nu_v3_t position;
    nu_q4_t rotation;
    nu_v3_t scale;
} transform_t;

typedef struct
{
    nu_u32_t    stat;
    nu_v3_t     v;
    transform_t transform;
} player_t;

static nu_ecs_id_t g_transform;
static nu_ecs_id_t g_transform_position;
static nu_ecs_id_t g_transform_rotation;
static nu_ecs_id_t g_transform_scale;
static nu_ecs_id_t g_player;
static nu_ecs_id_t g_player_stat;
static nu_ecs_id_t g_player_transform;
static nu_ecs_t    g_ecs;
static nu_scope_t  g_scope;

void
init (void)
{
    g_scope = nu_scope_new(NU_STR("main"), NU_MEM_16M);
    nu_scope_set_active(g_scope);

    g_transform = NU_ECS_COMPONENT(transform, transform_t);
    g_transform_position
        = NU_ECS_APRIMITIVE(g_transform, transform_t, position, NU_ECS_V3, 1);
    g_transform_rotation
        = NU_ECS_APRIMITIVE(g_transform, transform_t, rotation, NU_ECS_Q4, 1);
    g_transform_scale
        = NU_ECS_APRIMITIVE(g_transform, transform_t, scale, NU_ECS_V3, 1);

    g_player      = NU_ECS_COMPONENT(player, player_t);
    g_player_stat = NU_ECS_APRIMITIVE(g_player, player_t, stat, NU_ECS_U32, 1);
    g_player_transform
        = NU_ECS_ACOMPONENT(g_player, player_t, transform, g_transform, 1);

    g_ecs = nu_ecs_new(512);

    nu_ecs_id_t  e          = nu_ecs_add(g_ecs);
    transform_t *p          = nu_ecs_set(g_ecs, e, g_transform);
    p->scale                = NU_V3_ONES;
    p->position             = NU_V3_ONES;
    p->rotation             = nu_q4_identity();
    nu_ecs_id_t ep          = e;
    player_t   *player      = nu_ecs_set(g_ecs, e, g_player);
    player->transform.scale = NU_V3_ONES;
    player->stat            = 0xFFFFFFFF;
    e                       = nu_ecs_add(g_ecs);
    nu_ecs_set(g_ecs, e, g_transform);

    NU_INFO("saved:");
    nu_ecs_dump(g_ecs);

    nu_scope_push();
    nu_seria_t ser
        = nu_seria_new_file(NU_STR("dump.bin"), NU_SERIA_WRITE, NU_MEM_4M);
    nu_ecs_save(g_ecs, ser);
    nu_scope_pop();

    // ser    = nu_seria_new_file(NU_STR("dump.bin"), NU_SERIA_READ, NU_MEM_4M);
    // g_ecs  = nu_ecs_load(ser);
    // player = nu_ecs_get(g_ecs, ep, g_player);
    //
    // NU_INFO("loaded:");
    // nu_ecs_dump(g_ecs);
}

void
nu_app (void)
{
    nu_app_init_callback(init);
    nu_app_log_level(NU_LOG_DEBUG);
}

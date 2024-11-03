#include <build/examples/ecs/config.h>
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
    nu_v3_t   position;
    nu_q4_t   rotation;
    nu_v3_t   scale;
    subtype_t subtype;
} transform_t;

typedef struct
{
    nu_u32_t stat;
    nu_v3_t  v;
} player_t;

void
init (void)
{
    NU_SERIA_STRUCT(
        "subtype", subtype_t, NU_SERIA_FIELD("hello", NU_SERIA_U32, 1, hello);
        NU_SERIA_FIELD("vector", NU_SERIA_V3, 1, vector);
        NU_SERIA_FIELD("quat", NU_SERIA_Q4, 1, quat););
    NU_SERIA_STRUCT(
        "transform",
        transform_t,
        NU_SERIA_FIELD("position", NU_SERIA_V3, 1, position);
        NU_SERIA_FIELD("rotation", NU_SERIA_Q4, 1, rotation);
        NU_SERIA_FIELD("scale", NU_SERIA_V3, 1, scale);
        NU_SERIA_FIELD(
            "subtype", nu_seria_type(NU_STR("subtype")), 1, subtype););
    NU_SERIA_STRUCT(
        "player", player_t, NU_SERIA_FIELD("stat", NU_SERIA_U32, 1, stat);
        NU_SERIA_FIELD("v", NU_SERIA_V3, 1, v));

    ecs = nu_ecs_create();
    COMP_TRANSFORM
        = nu_ecs_register_seria(ecs, nu_seria_type(NU_STR("transform")));
    COMP_PLAYER = nu_ecs_register_seria(ecs, nu_seria_type(NU_STR("player")));

    nu_seria_dump_types();

    NU_VEC(transform_t) transforms;
    NU_VEC_INIT(10, &transforms);

    for (nu_size_t i = 0; i < 14; ++i)
    {
        transform_t *transform    = NU_VEC_PUSH(&transforms);
        transform->position       = nu_v3(1, 2, 3);
        transform->scale          = NU_V3_ONES;
        transform->rotation       = nu_q4_identity();
        transform->subtype.quat   = nu_q4_identity();
        transform->subtype.hello  = 0xFFFFFFFF;
        transform->subtype.vector = NU_V3_ONES;
    }

    const nu_size_t bytes_size = 1 << 14;
    nu_byte_t      *bytes      = nu_alloc(bytes_size);
    nu_seria_t      ser        = nu_seria_create();
    nu_seria_type_t type       = nu_seria_type(NU_STR("transform"));

    nu_seria_open_bytes(ser, NU_SERIA_WRITE, NU_SERIA_NBIN, bytes, bytes_size);
    NU_VEC_WRITE(&transforms, ser, type);
    nu_size_t n = nu_seria_close(ser);

    nu__seria_write_bytes(NU_STR("dump.bin"), bytes, n);

    nu_seria_open_bytes(ser, NU_SERIA_READ, NU_SERIA_NBIN, bytes, n);
    NU_VEC_READ(&transforms, ser, type, NU_NULL);
    NU_ASSERT(transforms.size == 14);
    nu_seria_close(ser);

    nu_seria_dump_values(type, 1, transforms.data);
}

void
nu_app (void)
{
    nu_app_init_callback(init);
}

#ifndef NU_ECS_H
#define NU_ECS_H

#include <nucleus/ecs/api.h>

#define NU__ECS_ENTITY_PER_MASK    32
#define NU__ECS_COMPONENT_NAME_LEN 64

typedef nu_u32_t nu__ecs_mask_t;
typedef NU_VEC(nu__ecs_mask_t) nu__ecs_bitset_t;

typedef struct
{
    nu_byte_t         name[NU__ECS_COMPONENT_NAME_LEN];
    nu_seria_layout_t layout;
} nu__ecs_component_t;

typedef struct
{
    nu_size_t        capa;
    void            *data;
    nu_size_t        component_size;
    nu__ecs_bitset_t bitset;
} nu__ecs_component_pool_t;

typedef struct
{
    NU_VEC(nu_ecs_id_t)
    includes;
    NU_VEC(nu_ecs_id_t) excludes;
    nu__ecs_mask_t mask;
    nu_size_t      mask_index;
    nu_size_t      mask_offset;
} nu__ecs_iter_t;

typedef struct
{
    NU_VEC(nu__ecs_iter_t) iters;
    nu__ecs_bitset_t bitset;
    NU_VEC(nu__ecs_component_pool_t) pools;
} nu__ecs_instance_t;

typedef struct
{
    NU_POOL(nu__ecs_instance_t) instances;
    NU_VEC(nu__ecs_component_t) components;
#ifdef NU_BUILD_RESOURCE
    nu_resource_t res_ecs;
#endif
} nu__ecs_t;

static void nu__ecs_init(void);
static void nu__ecs_free(void);

static nu_size_t nu__ecs_bitset_count(const nu__ecs_bitset_t *bitset);
static nu_bool_t nu__ecs_bitset_isset(const nu__ecs_bitset_t *bitset,
                                      nu_size_t               index);
#ifdef NU_BUILD_RESOURCE
static void nu__ecs_res_register(void);
#endif

#endif

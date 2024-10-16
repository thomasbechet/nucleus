#ifndef NU_ECS_H
#define NU_ECS_H

#include <nucleus/ecs/api.h>

typedef nu_u64_t nu__ecs_bitset_t;

typedef struct
{
    nu_size_t size;
    nu_size_t capa;
    void     *data;
} nu__ecs_comp_t;

typedef struct
{
    nu_bool_t        used;
    nu__ecs_bitset_t include;
    nu__ecs_bitset_t exclude;
    NU_VEC(nu_ecs_id_t) entities;
    nu_ecs_system_callback_t callback;
} nu__ecs_sys_t;

typedef struct
{
    NU_VEC(nu__ecs_comp_t) components;
    NU_VEC(nu__ecs_sys_t) systems;
    NU_VEC(nu_size_t) bitsets;
    NU_VEC(nu_size_t) free_entities;
} nu__ecs_instance_t;

typedef struct
{
    NU_POOL(nu__ecs_instance_t) instances;
} nu__ecs_t;

static void nu__ecs_init(void);
static void nu__ecs_free(void);

#endif

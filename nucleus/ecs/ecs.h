#ifndef NU_ECS_H
#define NU_ECS_H

#include <nucleus/ecs/api.h>

#define NU__ECS_ENTITY_PER_MASK 32
typedef nu_u32_t nu__ecs_mask_t;
typedef NU_VEC(nu__ecs_mask_t) nu__ecs_bitset_t;

typedef struct
{
    nu_size_t        size;
    nu_size_t        capa;
    void            *data;
    nu__ecs_bitset_t bitset;
} nu__ecs_comp_t;

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
    NU_VEC(nu__ecs_comp_t) components;
    NU_VEC(nu__ecs_iter_t) iters;
    nu__ecs_bitset_t bitset;
    NU_VEC(nu_size_t) free_entities;
    nu_size_t next;
} nu__ecs_instance_t;

typedef struct
{
    NU_POOL(nu__ecs_instance_t) instances;
} nu__ecs_t;

static void nu__ecs_init(void);
static void nu__ecs_free(void);

#endif

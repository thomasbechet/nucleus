#ifndef NU_ECS_IMPL_H
#define NU_ECS_IMPL_H

#include <nucleus/internal.h>

static void
nu__ecs_init (void)
{
    NU_POOL_INIT(1, &_ctx.ecs.instances);
}
static void
nu__ecs_free (void)
{
    NU_POOL_FREE(&_ctx.ecs.instances);
}

nu_ecs_t
nu_ecs_create (void)
{
    nu_size_t           index;
    nu__ecs_instance_t *ins = NU_POOL_ADD(&_ctx.ecs.instances, &index);
    NU_VEC_INIT(64, &ins->components);
    NU_VEC_INIT(64, &ins->systems);
    NU_VEC_INIT(64, &ins->bitsets);
    NU_VEC_INIT(64, &ins->free_entities);
    return NU_HANDLE_MAKE(nu_ecs_t, index);
}
void
nu_ecs_delete (nu_ecs_t ecs)
{
    nu__ecs_instance_t *ins = _ctx.ecs.instances.data + NU_HANDLE_INDEX(ecs);
    NU_VEC_FREE(&ins->components);
    NU_VEC_FREE(&ins->systems);
    NU_VEC_FREE(&ins->bitsets);
    NU_VEC_FREE(&ins->free_entities);
    NU_POOL_REMOVE(&_ctx.ecs.instances, NU_HANDLE_INDEX(ecs));
}

nu_ecs_id_t
nu_ecs_add (nu_ecs_t ecs)
{
    nu__ecs_instance_t *ins = _ctx.ecs.instances.data + NU_HANDLE_INDEX(ecs);

    nu_size_t index;
    if (ins->free_entities.size)
    {
        // reuse removed entity
        index                    = *NU_VEC_POP(&ins->free_entities);
        ins->bitsets.data[index] = 0;
    }
    else
    {
        // allocate new entity
        index                       = ins->bitsets.size;
        *NU_VEC_PUSH(&ins->bitsets) = 0;
    }

    // insert entity in pools
    for (nu_size_t i = 0; i < ins->systems.size; ++i)
    {
        nu__ecs_sys_t *sys = ins->systems.data + i;
        if (sys->used)
        {
        }
    }

    return index + 1;
}
void
nu_ecs_remove (nu_ecs_t ecs, nu_ecs_id_t e)
{
    nu__ecs_instance_t *ins = _ctx.ecs.instances.data + NU_HANDLE_INDEX(ecs);

    NU_ASSERT(e);
    nu_size_t index                   = e - 1;
    *NU_VEC_PUSH(&ins->free_entities) = index;

    // TODO: remove from systems
}

nu_ecs_id_t
nu_ecs_reg_component (nu_ecs_t ecs, nu_size_t size)
{
    nu__ecs_instance_t *ins = _ctx.ecs.instances.data + NU_HANDLE_INDEX(ecs);
    return nu_ecs_reg_component_with(ecs, size, ins->components.size);
}
nu_ecs_id_t
nu_ecs_reg_component_with (nu_ecs_t ecs, nu_ecs_id_t id, nu_size_t size)
{
    nu__ecs_instance_t *ins = _ctx.ecs.instances.data + NU_HANDLE_INDEX(ecs);

    NU_ASSERT(id < 64);
    if (id >= ins->components.size)
    {
        NU_VEC_RESIZE(&ins->components, id + 1);
    }

    nu__ecs_comp_t *comp = ins->components.data + id;
    comp->size           = size;
    comp->capa           = 10;
    comp->data           = nu_alloc(size * comp->capa);

    return id;
}
static void
nu__bitset_set (nu_size_t *bitset, nu_size_t n)
{
    *bitset = *bitset | (1 << n);
}
static void
nu__bitset_unset (nu_size_t *bitset, nu_size_t n)
{
    *bitset = *bitset & ~(1 << n);
}
static nu_bool_t
nu__bitset_isset (nu_size_t bitset, nu_size_t n)
{
    return (bitset >> n) & 1;
}
void *
nu_ecs_set (nu_ecs_t ecs, nu_ecs_id_t e, nu_ecs_id_t c)
{
    NU_ASSERT(e);
    nu__ecs_instance_t *ins   = _ctx.ecs.instances.data + NU_HANDLE_INDEX(ecs);
    nu_size_t           index = e - 1;
    nu__ecs_comp_t     *comp  = ins->components.data + c;

    // realloc components if needed
    if (comp->capa <= index)
    {
        nu_size_t old_capa = comp->capa;
        comp->capa *= 2;
        comp->data = nu_realloc(
            comp->data, comp->size * old_capa, comp->size * comp->capa);
    }

    // update bitset
    nu__bitset_set(&ins->bitsets.data[index], c);

    return nu_ecs_get(ecs, e, c);
}
void
nu_ecs_unset (nu_ecs_t ecs, nu_ecs_id_t e, nu_ecs_id_t c)
{
    NU_ASSERT(e);
    nu__ecs_instance_t *ins   = _ctx.ecs.instances.data + NU_HANDLE_INDEX(ecs);
    nu_size_t           index = e - 1;
    nu__bitset_unset(&ins->bitsets.data[index], c);
}
void *
nu_ecs_get (nu_ecs_t ecs, nu_ecs_id_t e, nu_ecs_id_t c)
{
    NU_ASSERT(e);
    if (!nu_ecs_has(ecs, e, c))
    {
        return NU_NULL;
    }
    nu__ecs_instance_t *ins   = _ctx.ecs.instances.data + NU_HANDLE_INDEX(ecs);
    nu_size_t           index = e - 1;
    nu__ecs_comp_t     *comp  = ins->components.data + c;
    return (void *)((nu_size_t)comp->data + comp->size * index);
}
nu_bool_t
nu_ecs_has (nu_ecs_t ecs, nu_ecs_id_t e, nu_ecs_id_t c)
{
    NU_ASSERT(e);
    nu__ecs_instance_t *ins   = _ctx.ecs.instances.data + NU_HANDLE_INDEX(ecs);
    nu_size_t           index = e - 1;
    return nu__bitset_isset(ins->bitsets.data[index], c);
}

nu_ecs_id_t
nu_ecs_reg_system (nu_ecs_t ecs, nu_ecs_system_callback_t callback)
{
    nu__ecs_instance_t *ins = _ctx.ecs.instances.data + NU_HANDLE_INDEX(ecs);
    return nu_ecs_reg_system_with(ecs, ins->systems.size, callback);
}
nu_ecs_id_t
nu_ecs_reg_system_with (nu_ecs_t                 ecs,
                        nu_ecs_id_t              id,
                        nu_ecs_system_callback_t callback)
{
    nu__ecs_instance_t *ins = _ctx.ecs.instances.data + NU_HANDLE_INDEX(ecs);

    NU_ASSERT(id < 64);
    if (id >= ins->systems.size)
    {
        NU_VEC_RESIZE(&ins->systems, id + 1);
    }

    nu__ecs_sys_t *sys = ins->systems.data + id;
    sys->used          = NU_TRUE;
    sys->include       = 0;
    sys->exclude       = 0;
    NU_VEC_INIT(10, &sys->entities);
    sys->callback = callback;

    return id;
}
void
nu_ecs_requires (nu_ecs_t ecs, nu_ecs_id_t s, nu_ecs_id_t c)
{
    nu__ecs_instance_t *ins = _ctx.ecs.instances.data + NU_HANDLE_INDEX(ecs);
    nu__ecs_sys_t      *sys = ins->systems.data + s;
    nu__bitset_set(&sys->include, c);
}
void
nu_ecs_excludes (nu_ecs_t ecs, nu_ecs_id_t s, nu_ecs_id_t c)
{
    nu__ecs_instance_t *ins = _ctx.ecs.instances.data + NU_HANDLE_INDEX(ecs);
    nu__ecs_sys_t      *sys = ins->systems.data + s;
    nu__bitset_set(&sys->exclude, c);
}

#endif

#ifndef NU_ECS_IMPL_H
#define NU_ECS_IMPL_H

#include <nucleus/internal.h>
#include <nucleus/ecs/seria_impl.h>

static void
nu__ecs_mask_set (nu__ecs_mask_t *bitset, nu_size_t n)
{
    *bitset = *bitset | (1 << n);
}
static void
nu__ecs_mask_unset (nu__ecs_mask_t *bitset, nu_size_t n)
{
    *bitset = *bitset & ~(1 << n);
}
static nu_bool_t
nu__ecs_mask_isset (nu__ecs_mask_t bitset, nu_size_t n)
{
    return (bitset >> n) & 1;
}
static nu_size_t
nu__ecs_mask_count (nu__ecs_mask_t bitset)
{
    nu_size_t c;
    for (c = 0; bitset; bitset >>= 1)
    {
        c += bitset & 1;
    }
    return c;
}

static nu_bool_t
nu__ecs_bitset_isset (const nu__ecs_bitset_t *bitset, nu_size_t index)
{
    nu_size_t mask   = index / NU__ECS_ENTITY_PER_MASK;
    nu_size_t offset = index % NU__ECS_ENTITY_PER_MASK;
    return (bitset->size > mask
            && nu__ecs_mask_isset(bitset->data[mask], offset));
}
static void
nu__ecs_bitset_set (nu__ecs_bitset_t *bitset, nu_size_t index)
{
    nu_size_t mask   = index / NU__ECS_ENTITY_PER_MASK;
    nu_size_t offset = index % NU__ECS_ENTITY_PER_MASK;
    if (bitset->size <= mask)
    {
        nu_size_t old_size = bitset->size;
        NU_VEC_RESIZE(bitset, mask + 1);
        // initialize masks
        for (nu_size_t i = old_size; i < bitset->size; ++i)
        {
            bitset->data[i] = 0;
        }
    }
    nu__ecs_mask_set(&bitset->data[mask], offset);
}
static void
nu__ecs_bitset_unset (nu__ecs_bitset_t *bitset, nu_size_t index)
{
    nu_size_t mask   = index / NU__ECS_ENTITY_PER_MASK;
    nu_size_t offset = index % NU__ECS_ENTITY_PER_MASK;
    if (bitset->size > mask)
    {
        nu__ecs_mask_unset(&bitset->data[mask], offset);
    }
}
static nu__ecs_mask_t
nu__ecs_bitset_mask (const nu__ecs_bitset_t *bitset, nu_size_t mask_index)
{
    if (mask_index >= bitset->size)
    {
        return 0;
    }
    return bitset->data[mask_index];
}
static nu_size_t
nu__ecs_bitset_count (const nu__ecs_bitset_t *bitset)
{
    nu_size_t n = 0;
    for (nu_size_t i = 0; i < bitset->size; ++i)
    {
        n += nu__ecs_mask_count(bitset->data[i]);
    }
    return n;
}

static void
nu__ecs_init (void)
{
    NU_POOL_INIT(1, &_ctx.ecs.instances);
#ifdef NU_BUILD_ECS_SERIA
    NU_SERIA_STRUCT(NU_ECS_COMPONENT_TYPE_NAME,
                    nu__ecs_comp_dto_t,
                    NU_SERIA_FIELD("name", NU_SERIA_BYTE, 64, name);
                    NU_SERIA_FIELD("entities", NU_SERIA_BUF, 1, entities);
                    NU_SERIA_FIELD("data", NU_SERIA_BUF, 1, data));
    _ctx.ecs.component_type = nu_seria_type(NU_STR(NU_ECS_COMPONENT_TYPE_NAME));
#endif
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
    NU_VEC_INIT(64, &ins->iters);
    NU_VEC_INIT(64, &ins->bitset);
    NU_VEC_INIT(64, &ins->free_entities);
    ins->next = 0;
    return NU_HANDLE_MAKE(nu_ecs_t, index);
}
void
nu_ecs_delete (nu_ecs_t ecs)
{
    nu__ecs_instance_t *ins = _ctx.ecs.instances.data + NU_HANDLE_INDEX(ecs);
    for (nu_size_t i = 0; i < ins->components.size; ++i)
    {
        nu__ecs_comp_t *comp = ins->components.data + i;
        if (comp->data)
        {
            nu_free(comp->data, comp->size * comp->capa);
            NU_VEC_FREE(&comp->bitset);
        }
    }
    NU_VEC_FREE(&ins->components);
    for (nu_size_t i = 0; i < ins->iters.size; ++i)
    {
        nu__ecs_iter_t *it = ins->iters.data + i;
        NU_VEC_FREE(&it->includes);
        NU_VEC_FREE(&it->excludes);
    }
    NU_VEC_FREE(&ins->iters);
    NU_VEC_FREE(&ins->bitset);
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
        index = *NU_VEC_POP(&ins->free_entities);
    }
    else
    {
        index = ins->next++;
    }

    nu__ecs_bitset_set(&ins->bitset, index);

    return index + 1;
}
void
nu_ecs_add_at (nu_ecs_t ecs, nu_ecs_id_t id)
{
    NU_ASSERT(id);
    nu__ecs_instance_t *ins = _ctx.ecs.instances.data + NU_HANDLE_INDEX(ecs);
    if (nu_ecs_valid(ecs, id))
    {
        return;
    }
    nu__ecs_bitset_set(&ins->bitset, id - 1);
}
void
nu_ecs_remove (nu_ecs_t ecs, nu_ecs_id_t e)
{
    NU_ASSERT(e);
    nu__ecs_instance_t *ins   = _ctx.ecs.instances.data + NU_HANDLE_INDEX(ecs);
    nu_size_t           index = e - 1;

    // remove from components
    for (nu_size_t i = 0; i < ins->components.size; ++i)
    {
        nu_ecs_unset(ecs, e, i);
    }

    // mask entity as invalid
    nu__ecs_bitset_unset(&ins->bitset, index);

    *NU_VEC_PUSH(&ins->free_entities) = index;
}
nu_bool_t
nu_ecs_valid (nu_ecs_t ecs, nu_ecs_id_t e)
{
    NU_ASSERT(e);
    nu__ecs_instance_t *ins   = _ctx.ecs.instances.data + NU_HANDLE_INDEX(ecs);
    nu_size_t           index = e - 1;
    return nu__ecs_bitset_isset(&ins->bitset, index);
}
nu_size_t
nu_ecs_count (nu_ecs_t ecs)
{
    nu__ecs_instance_t *ins = _ctx.ecs.instances.data + NU_HANDLE_INDEX(ecs);
    return nu_ecs_capacity(ecs) - ins->free_entities.size;
}
nu_size_t
nu_ecs_capacity (nu_ecs_t ecs)
{
    nu__ecs_instance_t *ins = _ctx.ecs.instances.data + NU_HANDLE_INDEX(ecs);
    return ins->next;
}

nu_ecs_id_t
nu_ecs_register (nu_ecs_t ecs, nu_size_t size)
{
    nu__ecs_instance_t *ins = _ctx.ecs.instances.data + NU_HANDLE_INDEX(ecs);

    nu__ecs_comp_t *comp = NU_VEC_PUSH(&ins->components);
    comp->size           = size;
    comp->capa           = 10;
    comp->data           = nu_alloc(size * comp->capa);
#ifdef NU_BUILD_ECS_SERIA
    comp->type = NU_NULL;
#endif
    nu_size_t init_mask_count = (comp->capa / NU__ECS_ENTITY_PER_MASK) + 1;
    NU_VEC_INIT(init_mask_count, &comp->bitset);

    return ins->components.size - 1;
}
void *
nu_ecs_set (nu_ecs_t ecs, nu_ecs_id_t e, nu_ecs_id_t c)
{
    NU_ASSERT(e);
    nu__ecs_instance_t *ins   = _ctx.ecs.instances.data + NU_HANDLE_INDEX(ecs);
    nu_size_t           index = e - 1;
    nu__ecs_comp_t     *comp  = ins->components.data + c;

    // check existing component
    if (!nu__ecs_bitset_isset(&comp->bitset, index))
    {
        // update bitset
        nu__ecs_bitset_set(&comp->bitset, index);

        // realloc components if needed
        if (comp->capa <= index)
        {
            nu_size_t old_capa = comp->capa;
            comp->capa *= 2;
            comp->data = nu_realloc(
                comp->data, comp->size * old_capa, comp->size * comp->capa);
        }

        // expect zero memory by default
        nu_memset((void *)((nu_size_t)comp->data + comp->size * index),
                  0,
                  comp->size);
    }

    return nu_ecs_get(ecs, e, c);
}
void
nu_ecs_unset (nu_ecs_t ecs, nu_ecs_id_t e, nu_ecs_id_t c)
{
    NU_ASSERT(e);
    nu__ecs_instance_t *ins   = _ctx.ecs.instances.data + NU_HANDLE_INDEX(ecs);
    nu_size_t           index = e - 1;
    nu__ecs_comp_t     *comp  = ins->components.data + c;
    nu__ecs_bitset_unset(&comp->bitset, index);
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
    nu__ecs_comp_t     *comp  = ins->components.data + c;
    return nu__ecs_bitset_isset(&comp->bitset, index);
}

nu_ecs_id_t
nu_ecs_iter (nu_ecs_t ecs)
{
    nu__ecs_instance_t *ins   = _ctx.ecs.instances.data + NU_HANDLE_INDEX(ecs);
    nu_size_t           index = ins->iters.size;
    nu__ecs_iter_t     *it    = NU_VEC_PUSH(&ins->iters);
    NU_VEC_INIT(4, &it->includes);
    NU_VEC_INIT(4, &it->excludes);
    return index;
}
void
nu_ecs_includes (nu_ecs_t ecs, nu_ecs_id_t iter, nu_ecs_id_t c)
{
    nu__ecs_instance_t *ins = _ctx.ecs.instances.data + NU_HANDLE_INDEX(ecs);
    nu__ecs_iter_t     *it  = ins->iters.data + iter;
    *NU_VEC_PUSH(&it->includes) = c;
}
void
nu_ecs_excludes (nu_ecs_t ecs, nu_ecs_id_t iter, nu_ecs_id_t c)
{
    nu__ecs_instance_t *ins = _ctx.ecs.instances.data + NU_HANDLE_INDEX(ecs);
    nu__ecs_iter_t     *it  = ins->iters.data + iter;
    *NU_VEC_PUSH(&it->excludes) = c;
}
static nu_ecs_id_t
nu__ecs_iter_next (const nu__ecs_instance_t *ins, nu__ecs_iter_t *it)
{
    // find next non empty mask
    while (!it->mask)
    {
        ++it->mask_index;
        const nu_size_t max_mask_count
            = (ins->next / NU__ECS_ENTITY_PER_MASK) + 1;
        if (it->mask_index >= max_mask_count)
        {
            // reach last mask
            return NU_NULL;
        }

        // initialize mask with existing entities
        it->mask = nu__ecs_bitset_mask(&ins->bitset, it->mask_index);
        // filter on required components
        for (nu_size_t i = 0; i < it->includes.size; ++i)
        {
            nu__ecs_comp_t *comp = ins->components.data + it->includes.data[i];
            it->mask &= nu__ecs_bitset_mask(&comp->bitset, it->mask_index);
        }
        // must be processed after includes
        for (nu_size_t i = 0; i < it->excludes.size; ++i)
        {
            nu__ecs_comp_t *comp = ins->components.data + it->excludes.data[i];
            it->mask &= ~nu__ecs_bitset_mask(&comp->bitset, it->mask_index);
        }

        it->mask_offset = 0;
    }

    // consume mask (at this step, the mask has at least one bit set)
    while (!(it->mask & 1))
    {
        it->mask >>= 1;
        ++it->mask_offset;
    }
    it->mask >>= 1;
    nu_size_t offset = it->mask_offset++;
    return (it->mask_index * NU__ECS_ENTITY_PER_MASK + offset) + 1;
}
nu_ecs_id_t
nu_ecs_begin (nu_ecs_t ecs, nu_ecs_id_t iter)
{
    nu__ecs_instance_t *ins = _ctx.ecs.instances.data + NU_HANDLE_INDEX(ecs);
    nu__ecs_iter_t     *it  = ins->iters.data + iter;
    it->mask                = 0;
    it->mask_offset         = 0;
    it->mask_index          = (nu_size_t)-1; // trick for first iteration
    return nu__ecs_iter_next(ins, it);
}
nu_ecs_id_t
nu_ecs_next (nu_ecs_t ecs, nu_ecs_id_t iter)
{
    nu__ecs_instance_t *ins = _ctx.ecs.instances.data + NU_HANDLE_INDEX(ecs);
    nu__ecs_iter_t     *it  = ins->iters.data + iter;
    return nu__ecs_iter_next(ins, it);
}

#endif

#ifndef NU_ECS_IMPL_H
#define NU_ECS_IMPL_H

#include <nucleus/internal.h>

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
        if (mask >= bitset->capacity)
        {
            NU_ERROR("max ecs entity count reached");
            return;
        }
        bitset->size = mask + 1;
        // masks are already initialized to 0
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
static nu_size_t
nu__ecs_bitset_find_unset (const nu__ecs_bitset_t *bitset)
{
    for (nu_size_t i = 0; i < bitset->size; ++i)
    {
        nu__ecs_mask_t mask = bitset->data[i];
        if (bitset->data[i] != 0xFFFFFFFF)
        {
            nu_size_t index = 0;
            while (mask & 0x1)
            {
                mask >>= 1;
                ++index;
            }
            return i * NU__ECS_ENTITY_PER_MASK + index;
        }
    }
    return bitset->size * NU__ECS_ENTITY_PER_MASK;
}
static nu_size_t
nu__ecs_bitset_capacity (const nu__ecs_bitset_t *bitset)
{
    return bitset->capacity * NU__ECS_ENTITY_PER_MASK;
}

static void
nu__ecs_handler (nu_object_hook_t hook, void *data)
{
    nu__ecs_instance_t *ins = data;
    switch (hook)
    {
        case NU_OBJECT_CLEANUP:
            break;
        case NU_OBJECT_SAVE:
            break;
        case NU_OBJECT_LOAD:
            break;
    }
}

static void
nu__ecs_init (void)
{
    NU_FIXEDVEC_ALLOC(&_ctx.ecs.components, NU_ECS_COMPONENT_MAX);
    NU_FIXEDVEC_ALLOC(&_ctx.ecs.iters, NU_ECS_ITER_MAX);
    _ctx.ecs.obj_ecs = nu_object_register(
        NU_STR("ecs"), sizeof(nu__ecs_instance_t), nu__ecs_handler);
}

nu_ecs_id_t
nu_ecs_register (nu_str_t name, nu_seria_layout_t layout)
{
    NU_ASSERT(layout);
    nu__ecs_component_t *comp = NU_FIXEDVEC_PUSH(&_ctx.ecs.components);
    if (!comp)
    {
        NU_ERROR("max ecs component count reached");
        return NU_NULL;
    }
    comp->layout = layout;
    nu_str_to_cstr(name, comp->name, NU__ECS_COMPONENT_NAME_LEN);

    return _ctx.ecs.components.size - 1;
}
nu_ecs_id_t
nu_ecs_find_component (nu_str_t name)
{
    for (nu_size_t i = 0; i < _ctx.ecs.components.size; ++i)
    {
        const nu__ecs_component_t *comp = _ctx.ecs.components.data + i;
        if (nu_str_eq(name, nu_str_from_cstr((nu_byte_t *)comp->name)))
        {
            return i;
        }
    }
    NU_UNREACHABLE();
    return 0;
}

nu_ecs_t
nu_ecs_new (nu_size_t capacity)
{
    nu__ecs_instance_t *ins = nu_object_new(_ctx.ecs.obj_ecs);
    ins->scope              = nu_scope_active();
    NU_FIXEDVEC_ALLOC(&ins->pools, _ctx.ecs.components.size);
    NU_FIXEDVEC_ALLOC(&ins->bitset, (capacity / NU__ECS_ENTITY_PER_MASK) + 1);
    return (nu_ecs_t)ins;
}
nu_ecs_id_t
nu_ecs_add (nu_ecs_t ecs)
{
    nu__ecs_instance_t *ins = (nu__ecs_instance_t *)ecs;

    nu_size_t index = nu__ecs_bitset_find_unset(&ins->bitset);
    nu__ecs_bitset_set(&ins->bitset, index);

    return index + 1;
}
void
nu_ecs_add_at (nu_ecs_t ecs, nu_ecs_id_t id)
{
    NU_ASSERT(id);
    nu__ecs_instance_t *ins = (nu__ecs_instance_t *)ecs;
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
    nu__ecs_instance_t *ins   = (nu__ecs_instance_t *)ecs;
    nu_size_t           index = e - 1;

    // remove from components
    for (nu_size_t i = 0; i < ins->pools.size; ++i)
    {
        nu_ecs_unset(ecs, e, i);
    }

    // mask entity as invalid
    nu__ecs_bitset_unset(&ins->bitset, index);
}
nu_bool_t
nu_ecs_valid (nu_ecs_t ecs, nu_ecs_id_t e)
{
    NU_ASSERT(e);
    nu__ecs_instance_t *ins   = (nu__ecs_instance_t *)ecs;
    nu_size_t           index = e - 1;
    return nu__ecs_bitset_isset(&ins->bitset, index);
}
nu_size_t
nu_ecs_count (nu_ecs_t ecs)
{
    nu__ecs_instance_t *ins = (nu__ecs_instance_t *)ecs;
    return nu__ecs_bitset_count(&ins->bitset);
}
nu_size_t
nu_ecs_capacity (nu_ecs_t ecs)
{
    nu__ecs_instance_t *ins = (nu__ecs_instance_t *)ecs;
    return ins->bitset.capacity * NU__ECS_ENTITY_PER_MASK;
}
void
nu_ecs_clear (nu_ecs_t ecs)
{
    nu__ecs_instance_t *ins = (nu__ecs_instance_t *)ecs;
    for (nu_size_t i = 0; i < ins->pools.size; ++i)
    {
        nu__ecs_component_pool_t *pool = ins->pools.data + i;
        NU_FIXEDVEC_CLEAR(&pool->bitset);
    }
    NU_FIXEDVEC_CLEAR(&ins->bitset);
}

void *
nu_ecs_set (nu_ecs_t ecs, nu_ecs_id_t e, nu_ecs_id_t c)
{
    NU_ASSERT(e);
    nu__ecs_instance_t *ins   = (nu__ecs_instance_t *)ecs;
    nu_size_t           index = e - 1;

    const nu__ecs_component_t *component = _ctx.ecs.components.data + c;

    // initialize pool if component missing
    if (c >= ins->pools.size)
    {
        if (c >= ins->pools.capacity)
        {
            NU_ERROR("invalid ecs component id");
            return NU_NULL;
        }
        nu_size_t prev_size = ins->pools.size;
        ins->pools.size     = c + 1;
        for (nu_size_t i = prev_size; i < ins->pools.size; ++i)
        {
            nu_size_t component_size
                = nu_seria_size(_ctx.ecs.components.data[i].layout);
            nu__ecs_component_pool_t *pool = ins->pools.data + i;
            pool->component_size           = component_size;
            NU_FIXEDVEC_ALLOC(&pool->chunks, ins->bitset.capacity);
            NU_FIXEDVEC_ALLOC(&pool->bitset, ins->bitset.capacity);
            for (nu_size_t i = 0; i < ins->bitset.capacity; ++i)
            {
                pool->chunks.data[i] = NU_NULL;
                pool->bitset.data[i] = 0;
            }
        }
    }

    nu__ecs_component_pool_t *pool = ins->pools.data + c;

    // check existing component
    if (!nu__ecs_bitset_isset(&pool->bitset, index))
    {
        // update bitset
        nu__ecs_bitset_set(&pool->bitset, index);

        // check if chunk exists
        nu_size_t mask = index / NU__ECS_ENTITY_PER_MASK;
        if (!pool->chunks.data[mask])
        {
            // allocate new chunk
            pool->chunks.data[mask] = nu_scope_alloc(pool->component_size
                                                     * NU__ECS_ENTITY_PER_MASK);
            // expect zero memory by default
            nu_memset(pool->chunks.data[mask],
                      0,
                      pool->component_size * NU__ECS_ENTITY_PER_MASK);
        }
    }

    return nu_ecs_get(ecs, e, c);
}
void
nu_ecs_unset (nu_ecs_t ecs, nu_ecs_id_t e, nu_ecs_id_t c)
{
    NU_ASSERT(e);
    nu__ecs_instance_t       *ins   = (nu__ecs_instance_t *)ecs;
    nu_size_t                 index = e - 1;
    nu__ecs_component_pool_t *pool  = ins->pools.data + c;
    nu__ecs_bitset_unset(&pool->bitset, index);
}
void *
nu_ecs_get (nu_ecs_t ecs, nu_ecs_id_t e, nu_ecs_id_t c)
{
    NU_ASSERT(e);
    if (!nu_ecs_has(ecs, e, c))
    {
        return NU_NULL;
    }
    nu__ecs_instance_t       *ins    = (nu__ecs_instance_t *)ecs;
    nu_size_t                 index  = e - 1;
    nu_size_t                 mask   = index / NU__ECS_ENTITY_PER_MASK;
    nu_size_t                 offset = index % NU__ECS_ENTITY_PER_MASK;
    nu__ecs_component_pool_t *pool   = ins->pools.data + c;
    return (void *)((nu_size_t)pool->chunks.data[mask]
                    + pool->component_size * offset);
}
nu_bool_t
nu_ecs_has (nu_ecs_t ecs, nu_ecs_id_t e, nu_ecs_id_t c)
{
    NU_ASSERT(e);
    nu__ecs_instance_t       *ins   = (nu__ecs_instance_t *)ecs;
    nu_size_t                 index = e - 1;
    nu__ecs_component_pool_t *pool  = ins->pools.data + c;
    return nu__ecs_bitset_isset(&pool->bitset, index);
}

nu_ecs_id_t
nu_ecs_iter (nu_size_t include_count, nu_size_t exclude_count)
{
    nu__ecs_iter_t *it = NU_FIXEDVEC_PUSH(&_ctx.ecs.iters);
    if (!it)
    {
        NU_ERROR("max ecs iter count reached");
        return NU_NULL;
    }
    NU_FIXEDVEC_ALLOC(&it->includes, include_count);
    NU_FIXEDVEC_ALLOC(&it->excludes, exclude_count);
    return _ctx.ecs.iters.size - 1;
}
void
nu_ecs_includes (nu_ecs_id_t iter, nu_ecs_id_t c)
{
    nu__ecs_iter_t *it   = _ctx.ecs.iters.data + iter;
    nu_ecs_id_t    *incl = NU_FIXEDVEC_PUSH(&it->includes);
    NU_ASSERT(incl);
    *incl = c;
}
void
nu_ecs_excludes (nu_ecs_id_t iter, nu_ecs_id_t c)
{
    nu__ecs_iter_t *it   = _ctx.ecs.iters.data + iter;
    nu_ecs_id_t    *excl = NU_FIXEDVEC_PUSH(&it->excludes);
    NU_ASSERT(excl);
    *excl = c;
}
static nu_ecs_id_t
nu__ecs_iter_next (const nu__ecs_instance_t *ins, nu__ecs_iter_t *it)
{
    // find next non empty mask
    while (!it->mask)
    {
        ++it->mask_index;
        const nu_size_t max_mask_count = ins->bitset.size;
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
            nu__ecs_component_pool_t *pool
                = ins->pools.data + it->includes.data[i];
            it->mask &= nu__ecs_bitset_mask(&pool->bitset, it->mask_index);
        }
        // must be processed after includes
        for (nu_size_t i = 0; i < it->excludes.size; ++i)
        {
            nu__ecs_component_pool_t *pool
                = ins->pools.data + it->excludes.data[i];
            it->mask &= ~nu__ecs_bitset_mask(&pool->bitset, it->mask_index);
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
    nu__ecs_instance_t *ins = (nu__ecs_instance_t *)ecs;
    nu__ecs_iter_t     *it  = _ctx.ecs.iters.data + iter;
    it->mask                = 0;
    it->mask_offset         = 0;
    it->mask_index          = (nu_size_t)-1; // trick for first iteration
    return nu__ecs_iter_next(ins, it);
}
nu_ecs_id_t
nu_ecs_next (nu_ecs_t ecs, nu_ecs_id_t iter)
{
    nu__ecs_instance_t *ins = (nu__ecs_instance_t *)ecs;
    nu__ecs_iter_t     *it  = _ctx.ecs.iters.data + iter;
    return nu__ecs_iter_next(ins, it);
}

void
nu_ecs_save (nu_ecs_t ecs, nu_seria_t seria)
{
    nu__ecs_instance_t *ins = (nu__ecs_instance_t *)ecs;

    // write ecs header
    nu_seria_write_u32(seria, nu_ecs_capacity(ecs));
    nu_seria_write_u32(seria, ins->pools.size);

    // iterate pools
    for (nu_size_t p = 0; p < ins->pools.size; ++p)
    {
        const nu__ecs_component_pool_t *pool = ins->pools.data + p;
        nu_u32_t component_count = nu__ecs_bitset_count(&pool->bitset);
        if (!component_count)
        {
            continue;
        }
        const nu__ecs_component_t *comp = _ctx.ecs.components.data + p;
        nu_str_t component_name = nu_str_from_cstr((nu_byte_t *)comp->name);

        // write component name
        nu_seria_write(
            seria, NU_SERIA_STR, component_name.size, component_name.data);

        // write component count
        nu_seria_write(seria, NU_SERIA_U32, 1, &component_count);

        // write component data
        nu_size_t entity_count = nu_ecs_count(ecs);
        for (nu_size_t i = 0; i < entity_count; ++i)
        {
            if (nu__ecs_bitset_isset(&pool->bitset, i))
            {
                // write entity
                nu_ecs_id_t id = i + 1;
                nu_seria_write(seria, NU_SERIA_U32, 1, &id);

                // write data
                void *data = nu_ecs_get(ecs, id, p);
                nu_seria_write(seria, comp->layout, 1, data);
            }
        }
    }
}
nu_ecs_t
nu_ecs_load (nu_seria_t seria)
{
    // read ecs header
    nu_size_t capacity   = nu_seria_read_u32(seria);
    nu_u32_t  pool_count = nu_seria_read_u32(seria);

    nu_ecs_t            ecs = nu_ecs_new(capacity);
    nu__ecs_instance_t *ins = (nu__ecs_instance_t *)ecs;

    for (nu_size_t p = 0; p < pool_count; ++p)
    {
        // read component name
        nu_byte_t component_name[NU__ECS_COMPONENT_NAME_LEN];
        nu_seria_read(
            seria, NU_SERIA_STR, NU__ECS_COMPONENT_NAME_LEN, component_name);

        // find component
        nu_ecs_id_t component
            = nu_ecs_find_component(nu_str_from_cstr(component_name));

        // read component count
        nu_u32_t component_count = nu_seria_read_u32(seria);
        NU_ASSERT(component_count > 0);

        // find component layout
        nu_seria_layout_t layout = _ctx.ecs.components.data[component].layout;
        NU_ASSERT(layout);

        // read component data
        for (nu_size_t i = 0; i < component_count; ++i)
        {
            // read entity
            nu_u32_t id;
            nu_seria_read(seria, NU_SERIA_U32, 1, &id);
            NU_ASSERT(id);
            nu_ecs_id_t e = id;

            // create entity if needed
            if (!nu_ecs_valid(ecs, e))
            {
                nu_ecs_add_at(ecs, e);
            }

            // set component
            void *data = nu_ecs_set(ecs, e, component);

            // read component
            nu_seria_read(seria, layout, 1, data);
        }
    }

    return ecs;
}
void
nu_ecs_dump (nu_ecs_t ecs)
{
    const nu__ecs_instance_t *ins = (nu__ecs_instance_t *)ecs;

    for (nu_size_t i = 0; i < nu_ecs_capacity(ecs); ++i)
    {
        nu_ecs_id_t e = i + 1;
        if (nu_ecs_valid(ecs, e))
        {
            NU_INFO("  #%d", e);
            for (nu_size_t c = 0; c < ins->pools.size; ++c)
            {
                if (nu_ecs_has(ecs, e, c))
                {
                    const nu__ecs_component_t *comp
                        = _ctx.ecs.components.data + c;
                    NU_INFO(
                        "  " NU_STR_FMT,
                        NU_STR_ARGS(nu_str_from_cstr((nu_byte_t *)comp->name)));
                    void *data = nu_ecs_get(ecs, e, c);
                    nu_seria_dump_values(comp->layout, 1, data);
                }
            }
        }
    }
}

#endif

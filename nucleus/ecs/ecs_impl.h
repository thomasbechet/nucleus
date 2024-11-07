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

static void
nu__ecs_init (void)
{
    NU_POOL_INIT(1, &_ctx.ecs.instances);
    NU_VEC_INIT(1, &_ctx.ecs.components);
}
static void
nu__ecs_free (void)
{
    NU_VEC_FREE(&_ctx.ecs.components);
    NU_POOL_FREE(&_ctx.ecs.instances);
}

nu_ecs_t
nu_ecs_create (void)
{
    nu_size_t           index;
    nu__ecs_instance_t *ins = NU_POOL_ADD(&_ctx.ecs.instances, &index);
    NU_VEC_INIT(64, &ins->pools);
    NU_VEC_INIT(64, &ins->iters);
    NU_VEC_INIT(64, &ins->bitset);
    return NU_HANDLE_MAKE(nu_ecs_t, index);
}
void
nu_ecs_delete (nu_ecs_t ecs)
{
    nu__ecs_instance_t *ins = _ctx.ecs.instances.data + NU_HANDLE_INDEX(ecs);
    for (nu_size_t i = 0; i < ins->pools.size; ++i)
    {
        nu__ecs_component_pool_t *pool = ins->pools.data + i;
        if (pool->data)
        {
            nu_free(pool->data, pool->component_size * pool->capa);
            NU_VEC_FREE(&pool->bitset);
        }
    }
    NU_VEC_FREE(&ins->pools);
    for (nu_size_t i = 0; i < ins->iters.size; ++i)
    {
        nu__ecs_iter_t *it = ins->iters.data + i;
        NU_VEC_FREE(&it->includes);
        NU_VEC_FREE(&it->excludes);
    }
    NU_VEC_FREE(&ins->iters);
    NU_VEC_FREE(&ins->bitset);
    NU_POOL_REMOVE(&_ctx.ecs.instances, NU_HANDLE_INDEX(ecs));
}

nu_ecs_id_t
nu_ecs_register (nu_str_t name, nu_seria_layout_t layout)
{
    NU_ASSERT(layout);
    nu__ecs_component_t *comp = NU_VEC_PUSH(&_ctx.ecs.components);
    comp->layout              = layout;
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
    NU_ASSERT(NU_FALSE);
    return 0;
}

nu_ecs_id_t
nu_ecs_add (nu_ecs_t ecs)
{
    nu__ecs_instance_t *ins = _ctx.ecs.instances.data + NU_HANDLE_INDEX(ecs);

    nu_size_t index = nu__ecs_bitset_find_unset(&ins->bitset);
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
    nu__ecs_instance_t *ins   = _ctx.ecs.instances.data + NU_HANDLE_INDEX(ecs);
    nu_size_t           index = e - 1;
    return nu__ecs_bitset_isset(&ins->bitset, index);
}
nu_size_t
nu_ecs_count (nu_ecs_t ecs)
{
    nu__ecs_instance_t *ins = _ctx.ecs.instances.data + NU_HANDLE_INDEX(ecs);
    return nu__ecs_bitset_count(&ins->bitset);
}
nu_size_t
nu_ecs_capacity (nu_ecs_t ecs)
{
    nu__ecs_instance_t *ins = _ctx.ecs.instances.data + NU_HANDLE_INDEX(ecs);
    return ins->bitset.size * NU__ECS_ENTITY_PER_MASK;
}
void
nu_ecs_clear (nu_ecs_t ecs)
{
    nu__ecs_instance_t *ins = _ctx.ecs.instances.data + NU_HANDLE_INDEX(ecs);
    for (nu_size_t i = 0; i < ins->pools.size; ++i)
    {
        nu__ecs_component_pool_t *pool = ins->pools.data + i;
        NU_VEC_CLEAR(&pool->bitset);
    }
    NU_VEC_CLEAR(&ins->bitset);
}

void *
nu_ecs_set (nu_ecs_t ecs, nu_ecs_id_t e, nu_ecs_id_t c)
{
    NU_ASSERT(e);
    nu__ecs_instance_t *ins   = _ctx.ecs.instances.data + NU_HANDLE_INDEX(ecs);
    nu_size_t           index = e - 1;

    const nu__ecs_component_t *component = _ctx.ecs.components.data + c;

    // initialize pool if component missing
    if (c >= ins->pools.size)
    {
        nu_size_t prev_size = ins->pools.size;
        NU_VEC_RESIZE(&ins->pools, c + 1);
        for (nu_size_t i = prev_size; i < ins->pools.size; ++i)
        {
            nu_size_t component_size
                = nu_seria_size(_ctx.ecs.components.data[i].layout);
            nu__ecs_component_pool_t *pool = ins->pools.data + i;
            pool->capa                     = 10;
            pool->data           = nu_alloc(component_size * pool->capa);
            pool->component_size = component_size;
            nu_size_t init_mask_count
                = (pool->capa / NU__ECS_ENTITY_PER_MASK) + 1;
            NU_VEC_INIT(init_mask_count, &pool->bitset);
        }
    }

    nu__ecs_component_pool_t *pool = ins->pools.data + c;

    // check existing component
    if (!nu__ecs_bitset_isset(&pool->bitset, index))
    {
        // update bitset
        nu__ecs_bitset_set(&pool->bitset, index);

        // realloc components if needed
        if (pool->capa <= index)
        {
            nu_size_t old_capa = pool->capa;
            pool->capa *= 2;
            pool->data = nu_realloc(pool->data,
                                    pool->component_size * old_capa,
                                    pool->component_size * pool->capa);
        }

        // expect zero memory by default
        nu_memset(
            (void *)((nu_size_t)pool->data + pool->component_size * index),
            0,
            pool->component_size);
    }

    return nu_ecs_get(ecs, e, c);
}
void
nu_ecs_unset (nu_ecs_t ecs, nu_ecs_id_t e, nu_ecs_id_t c)
{
    NU_ASSERT(e);
    nu__ecs_instance_t *ins   = _ctx.ecs.instances.data + NU_HANDLE_INDEX(ecs);
    nu_size_t           index = e - 1;
    nu__ecs_component_pool_t *pool = ins->pools.data + c;
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
    nu__ecs_instance_t *ins   = _ctx.ecs.instances.data + NU_HANDLE_INDEX(ecs);
    nu_size_t           index = e - 1;
    nu__ecs_component_pool_t *pool = ins->pools.data + c;
    return (void *)((nu_size_t)pool->data + pool->component_size * index);
}
nu_bool_t
nu_ecs_has (nu_ecs_t ecs, nu_ecs_id_t e, nu_ecs_id_t c)
{
    NU_ASSERT(e);
    nu__ecs_instance_t *ins   = _ctx.ecs.instances.data + NU_HANDLE_INDEX(ecs);
    nu_size_t           index = e - 1;
    nu__ecs_component_pool_t *pool = ins->pools.data + c;
    return nu__ecs_bitset_isset(&pool->bitset, index);
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

void
nu_ecs_save (nu_ecs_t ecs, nu_seria_t seria)
{
    const nu__ecs_instance_t *ins
        = _ctx.ecs.instances.data + NU_HANDLE_INDEX(ecs);

    // write ecs header
    nu_u32_t pool_count = ins->pools.size;
    nu_seria_write(seria, NU_SERIA_U32, 1, &pool_count);

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
    nu_ecs_t            ecs = nu_ecs_create();
    nu__ecs_instance_t *ins = _ctx.ecs.instances.data + NU_HANDLE_INDEX(ecs);

    // read ecs header
    nu_u32_t pool_count;
    nu_seria_read(seria, NU_SERIA_U32, 1, &pool_count);

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
        nu_u32_t component_count;
        nu_seria_read(seria, NU_SERIA_U32, 1, &component_count);
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
    const nu__ecs_instance_t *ins
        = _ctx.ecs.instances.data + NU_HANDLE_INDEX(ecs);

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

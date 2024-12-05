#ifndef NU_ECS_IMPL_H
#define NU_ECS_IMPL_H

#include <nucleus/internal.h>

#define NU__ECS_ATTR_MAKE(comp, offset) (comp << 16 | offset)
#define NU__ECS_ATTR_OFFSET(id)         (id & 0xFFFF)
#define NU__ECS_ATTR_COMPID(id)         (id >> 16)

static nu_size_t
nu__ecs_primitive_size (nu_ecs_primitive_t primitive)
{
    switch (primitive)
    {
        case NU_ECS_BYTE:
            return 1;
        case NU_ECS_U32:
            return 4;
        case NU_ECS_F32:
            return 4;
        case NU_ECS_V3:
            return 4 * 3;
        case NU_ECS_Q4:
            return 4 * 4;
        case NU_ECS_M4:
            return 4 * 16;
    }
    return 0;
}

static nu_size_t
nu__ecs_attr_size (nu_ecs_attribute_t attr)
{
    switch (attr.kind)
    {
        case NU_ECS_ATTR_PRIMITIVE:
            return nu__ecs_primitive_size(attr.p);
        case NU_ECS_ATTR_COMPONENT:
            return nu_ecs_component_size(attr.c);
        case NU_ECS_ATTR_OBJECT:
            return sizeof(nu_object_t);
        case NU_ECS_ATTR_STRING:
            return attr.s;
    }
    return 0;
}

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
nu__ecs_init (void)
{
    NU_VEC_ALLOC(&_ctx.ecs.components, NU_ECS_COMPONENT_MAX);
    NU_VEC_ALLOC(&_ctx.ecs.attributes, NU_ECS_ATTRIBUTE_MAX);
    NU_VEC_ALLOC(&_ctx.ecs.iters, NU_ECS_ITER_MAX);
    _ctx.ecs.obj_ecs = nu_object_type_new(
        NU_STR("ecs"), sizeof(nu__ecs_instance_t), NU_NULL);
}

nu_ecs_id_t
nu_ecs_register_component (nu_str_t name, nu_size_t size)
{
    nu__ecs_component_t *comp = NU_VEC_PUSH(&_ctx.ecs.components);
    if (!comp)
    {
        NU_ERROR("max ecs component count reached");
        return NU_NULL;
    }
    nu_str_to_cstr(name, comp->name, NU__ECS_COMPONENT_NAME_LEN);
    comp->size = size;

    return NU_ID_MAKE(_ctx.ecs.components.size - 1);
}
nu_ecs_id_t
nu_ecs_register_attribute (nu_ecs_id_t        comp,
                           nu_str_t           name,
                           nu_ecs_attribute_t attribute,
                           nu_size_t          offset,
                           nu_size_t          count)
{
    nu__ecs_component_t *c = _ctx.ecs.components.data + NU_ID_INDEX(comp);
    nu__ecs_attribute_t *a = NU_VEC_PUSH(&_ctx.ecs.attributes);
    if (!a)
    {
        NU_ERROR("max ecs attribute count reached");
        return NU_NULL;
    }
    nu_str_to_cstr(name, a->name, NU__ECS_COMPONENT_NAME_LEN);
    a->component = comp;
    a->count     = count;
    a->info      = attribute;

    // compute new attribute offset
    nu_size_t asize = nu__ecs_attr_size(attribute);
    NU_ASSERT(asize);
    a->offset = offset;
    NU_ASSERT(a->offset <= 0xFFFF);

    return NU__ECS_ATTR_MAKE(comp, a->offset);
}
nu_ecs_id_t
nu_ecs_find_component (nu_str_t name)
{
    for (nu_size_t i = 0; i < _ctx.ecs.components.size; ++i)
    {
        const nu__ecs_component_t *comp = _ctx.ecs.components.data + i;
        if (nu_str_eq(name, nu_str_from_cstr((nu_byte_t *)comp->name)))
        {
            return NU_ID_MAKE(i);
        }
    }
    return NU_NULL;
}
nu_size_t
nu_ecs_component_size (nu_ecs_id_t c)
{
    NU_ASSERT(c);
    const nu__ecs_component_t *comp = _ctx.ecs.components.data + NU_ID_INDEX(c);
    return comp->size;
}

nu_ecs_t
nu_ecs_new (nu_size_t capacity)
{
    nu__ecs_instance_t *ins = nu_object_new(_ctx.ecs.obj_ecs);
    ins->scope              = nu_scope_active();
    NU_VEC_ALLOC(&ins->pools, _ctx.ecs.components.size);
    NU_VEC_ALLOC(&ins->bitset, (capacity / NU__ECS_ENTITY_PER_MASK) + 1);
    return (nu_ecs_t)ins;
}
nu_ecs_id_t
nu_ecs_add (nu_ecs_t ecs)
{
    nu__ecs_instance_t *ins = (nu__ecs_instance_t *)ecs;

    nu_size_t index = nu__ecs_bitset_find_unset(&ins->bitset);
    nu__ecs_bitset_set(&ins->bitset, index);

    return NU_ID_MAKE(index);
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
    nu__ecs_bitset_set(&ins->bitset, NU_ID_INDEX(id));
}
void
nu_ecs_remove (nu_ecs_t ecs, nu_ecs_id_t e)
{
    NU_ASSERT(e);
    nu__ecs_instance_t *ins   = (nu__ecs_instance_t *)ecs;
    nu_size_t           index = NU_ID_INDEX(e);

    // remove from components
    for (nu_size_t i = 0; i < ins->pools.size; ++i)
    {
        nu_ecs_unset(ecs, e, NU_ID_MAKE(i));
    }

    // mask entity as invalid
    nu__ecs_bitset_unset(&ins->bitset, index);
}
nu_bool_t
nu_ecs_valid (nu_ecs_t ecs, nu_ecs_id_t e)
{
    NU_ASSERT(e);
    nu__ecs_instance_t *ins   = (nu__ecs_instance_t *)ecs;
    nu_size_t           index = NU_ID_INDEX(e);
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
        NU_VEC_CLEAR(&pool->bitset);
    }
    NU_VEC_CLEAR(&ins->bitset);
}

void *
nu_ecs_set (nu_ecs_t ecs, nu_ecs_id_t e, nu_ecs_id_t c)
{
    NU_ASSERT(e);
    nu__ecs_instance_t *ins   = (nu__ecs_instance_t *)ecs;
    nu_size_t           index = NU_ID_INDEX(e);

    nu_size_t                  c_index   = NU_ID_INDEX(c);
    const nu__ecs_component_t *component = _ctx.ecs.components.data + c_index;

    // initialize pool if component missing
    if (c_index >= ins->pools.size)
    {
        if (c_index >= ins->pools.capacity)
        {
            NU_PANIC("invalid ecs component id");
        }
        nu_size_t prev_size = ins->pools.size;
        ins->pools.size     = c_index + 1;
        for (nu_size_t i = prev_size; i < ins->pools.size; ++i)
        {
            nu__ecs_component_pool_t *pool = ins->pools.data + i;
            pool->comp_size                = _ctx.ecs.components.data[i].size;
            NU_VEC_ALLOC(&pool->chunks, ins->bitset.capacity);
            NU_VEC_ALLOC(&pool->bitset, ins->bitset.capacity);
            for (nu_size_t i = 0; i < ins->bitset.capacity; ++i)
            {
                pool->chunks.data[i] = NU_NULL;
                pool->bitset.data[i] = 0;
            }
        }
    }

    nu__ecs_component_pool_t *pool = ins->pools.data + c_index;

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
            pool->chunks.data[mask]
                = nu_malloc(pool->comp_size * NU__ECS_ENTITY_PER_MASK);
            // expect zero memory by default
            nu_memset(pool->chunks.data[mask],
                      0,
                      pool->comp_size * NU__ECS_ENTITY_PER_MASK);
        }
    }

    return nu_ecs_get(ecs, e, c);
}
void
nu_ecs_unset (nu_ecs_t ecs, nu_ecs_id_t e, nu_ecs_id_t c)
{
    NU_ASSERT(e);
    nu__ecs_instance_t       *ins   = (nu__ecs_instance_t *)ecs;
    nu_size_t                 index = NU_ID_INDEX(e);
    nu__ecs_component_pool_t *pool  = ins->pools.data + NU_ID_INDEX(c);
    nu__ecs_bitset_unset(&pool->bitset, index);
}
nu_bool_t
nu_ecs_has (nu_ecs_t ecs, nu_ecs_id_t e, nu_ecs_id_t c)
{
    NU_ASSERT(e);
    NU_ASSERT(c);
    nu__ecs_instance_t       *ins   = (nu__ecs_instance_t *)ecs;
    nu_size_t                 index = NU_ID_INDEX(e);
    nu__ecs_component_pool_t *pool  = ins->pools.data + NU_ID_INDEX(c);
    return nu__ecs_bitset_isset(&pool->bitset, index);
}
void *
nu_ecs_get (nu_ecs_t ecs, nu_ecs_id_t e, nu_ecs_id_t c)
{
    NU_ASSERT(e);
    NU_ASSERT(c);
    if (!nu_ecs_has(ecs, e, c))
    {
        return NU_NULL;
    }
    nu__ecs_instance_t       *ins    = (nu__ecs_instance_t *)ecs;
    nu_size_t                 index  = NU_ID_INDEX(e);
    nu_size_t                 mask   = index / NU__ECS_ENTITY_PER_MASK;
    nu_size_t                 offset = index % NU__ECS_ENTITY_PER_MASK;
    nu__ecs_component_pool_t *pool   = ins->pools.data + NU_ID_INDEX(c);
    return (void *)((nu_size_t)pool->chunks.data[mask]
                    + pool->comp_size * offset);
}
void *
nu_ecs_get_a (nu_ecs_t ecs, nu_ecs_id_t e, nu_ecs_id_t a)
{
    NU_ASSERT(e);
    NU_ASSERT(a);
    nu_byte_t *data = nu_ecs_get(ecs, e, NU__ECS_ATTR_COMPID(a));
    if (data)
    {
        return data + NU__ECS_ATTR_OFFSET(a);
    }
    return NU_NULL;
}

nu_ecs_id_t
nu_ecs_iter (nu_size_t include_count, nu_size_t exclude_count)
{
    nu__ecs_iter_t *it = NU_VEC_PUSH(&_ctx.ecs.iters);
    if (!it)
    {
        NU_ERROR("max ecs iter count reached");
        return NU_NULL;
    }
    NU_VEC_ALLOC(&it->includes, include_count);
    NU_VEC_ALLOC(&it->excludes, exclude_count);
    return NU_ID_MAKE(_ctx.ecs.iters.size - 1);
}
void
nu_ecs_includes (nu_ecs_id_t iter, nu_ecs_id_t c)
{
    NU_ASSERT(c);
    nu__ecs_iter_t *it   = _ctx.ecs.iters.data + NU_ID_INDEX(iter);
    nu_ecs_id_t    *incl = NU_VEC_PUSH(&it->includes);
    NU_ASSERT(incl);
    *incl = c;
}
void
nu_ecs_excludes (nu_ecs_id_t iter, nu_ecs_id_t c)
{
    NU_ASSERT(c);
    nu__ecs_iter_t *it   = _ctx.ecs.iters.data + NU_ID_INDEX(iter);
    nu_ecs_id_t    *excl = NU_VEC_PUSH(&it->excludes);
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
                = ins->pools.data + NU_ID_INDEX(it->includes.data[i]);
            it->mask &= nu__ecs_bitset_mask(&pool->bitset, it->mask_index);
        }
        // must be processed after includes
        for (nu_size_t i = 0; i < it->excludes.size; ++i)
        {
            nu__ecs_component_pool_t *pool
                = ins->pools.data + NU_ID_INDEX(it->excludes.data[i]);
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
    return NU_ID_MAKE(it->mask_index * NU__ECS_ENTITY_PER_MASK + offset);
}
nu_ecs_id_t
nu_ecs_begin (nu_ecs_t ecs, nu_ecs_id_t iter)
{
    nu__ecs_instance_t *ins = (nu__ecs_instance_t *)ecs;
    nu__ecs_iter_t     *it  = _ctx.ecs.iters.data + NU_ID_INDEX(iter);
    it->mask                = 0;
    it->mask_offset         = 0;
    it->mask_index          = (nu_size_t)-1; // trick for first iteration
    return nu__ecs_iter_next(ins, it);
}
nu_ecs_id_t
nu_ecs_next (nu_ecs_t ecs, nu_ecs_id_t iter)
{
    nu__ecs_instance_t *ins = (nu__ecs_instance_t *)ecs;
    nu__ecs_iter_t     *it  = _ctx.ecs.iters.data + NU_ID_INDEX(iter);
    return nu__ecs_iter_next(ins, it);
}

void
nu_ecs_save (nu_ecs_t ecs, nu_seria_t seria)
{
    // nu__ecs_instance_t *ins = (nu__ecs_instance_t *)ecs;
    //
    // // write ecs header
    // nu_seria_write_1u32(seria, NU_STR("capacity"), nu_ecs_capacity(ecs));
    // nu_seria_write_1u32(seria, NU_STR("count"), ins->pools.size);
    //
    // // iterate pools
    // for (nu_size_t c = 0; c < ins->pools.size; ++c)
    // {
    //     nu_ecs_id_t                     cid  = NU_ID_MAKE(c);
    //     const nu__ecs_component_pool_t *pool = ins->pools.data + c;
    //     nu_u32_t component_count = nu__ecs_bitset_count(&pool->bitset);
    //     if (!component_count)
    //     {
    //         continue;
    //     }
    //     const nu__ecs_component_t *comp = _ctx.ecs.components.data + c;
    //     nu_str_t component_name = nu_str_from_cstr((nu_byte_t *)comp->name);
    //
    //     // write component name
    //     nu_seria_write_1str(seria, NU_STR("name"), component_name);
    //
    //     // write component count
    //     nu_seria_write_1u32(seria, NU_STR("count"), component_count);
    //
    //     // write component data
    //     nu_size_t entity_count = nu_ecs_count(ecs);
    //     for (nu_size_t i = 0; i < entity_count; ++i)
    //     {
    //         if (nu__ecs_bitset_isset(&pool->bitset, i))
    //         {
    //             // write entity
    //             nu_ecs_id_t id = NU_ID_MAKE(i);
    //             nu_seria_write_1u32(seria, NU_STR("id"), id);
    //
    //             // write data
    //             void *data = nu_ecs_get(ecs, id, cid);
    //             nu_seria_write_struct(
    //                 seria, NU_STR("data"), comp->layout, 1, data);
    //         }
    //     }
    // }
}
nu_ecs_t
nu_ecs_load (nu_seria_t seria)
{
    // // read ecs header
    // nu_size_t capacity   = nu_seria_read_1u32(seria, NU_STR("capacity"));
    // nu_u32_t  pool_count = nu_seria_read_1u32(seria, NU_STR("count"));
    //
    // nu_ecs_t            ecs = nu_ecs_new(capacity);
    // nu__ecs_instance_t *ins = (nu__ecs_instance_t *)ecs;
    //
    // for (nu_size_t p = 0; p < pool_count; ++p)
    // {
    //     // read component name
    //     nu_byte_t component_name_buf[NU__ECS_COMPONENT_NAME_LEN];
    //     nu_str_t  component_name = nu_seria_read_1str(seria,
    //                                                  NU_STR("name"),
    //                                                  NU__ECS_COMPONENT_NAME_LEN,
    //                                                  component_name_buf);
    //
    //     // find component
    //     nu_ecs_id_t cid = nu_ecs_find_component(component_name);
    //     NU_ASSERT(cid);
    //
    //     // read component count
    //     nu_u32_t component_count = nu_seria_read_1u32(seria,
    //     NU_STR("count")); NU_ASSERT(component_count > 0);
    //
    //     // find component layout
    //     nu_seria_struct_t layout
    //         = _ctx.ecs.components.data[NU_ID_INDEX(cid)].layout;
    //     NU_ASSERT(layout);
    //
    //     // read component data
    //     for (nu_size_t i = 0; i < component_count; ++i)
    //     {
    //         // read entity
    //         nu_ecs_id_t e = nu_seria_read_1u32(seria, NU_STR("id"));
    //         NU_ASSERT(e);
    //
    //         // create entity if needed
    //         if (!nu_ecs_valid(ecs, e))
    //         {
    //             nu_ecs_add_at(ecs, e);
    //         }
    //
    //         // set component
    //         void *data = nu_ecs_set(ecs, e, cid);
    //
    //         // read component
    //         nu_seria_read_struct(seria, NU_STR("data"), layout, 1, data);
    //     }
    // }
    //
    // return ecs;
    return NU_NULL;
}
static void
nu__ecs_print_with_depth (nu_size_t depth, nu_str_t format, ...)
{
    va_list args;
    va_start(args, format);
    NU_STR_BUF(str, 256);
    nu_str_vfmt(str, format, args);
    NU_INFO("%*s%s", depth * 2, "", str);
    va_end(args);
}
static void nu__ecs_dump_attribute(nu_size_t                  depth,
                                   const nu__ecs_attribute_t *a,
                                   const nu_byte_t           *data);
static void
nu__ecs_dump_component (nu_size_t        depth,
                        nu_ecs_id_t      c,
                        nu_size_t        count,
                        const nu_byte_t *data)
{
    const nu__ecs_component_t *comp = _ctx.ecs.components.data + NU_ID_INDEX(c);
    nu__ecs_print_with_depth(
        depth,
        NU_STR(NU_STR_FMT " {"),
        NU_STR_ARGS(nu_str_from_cstr((nu_byte_t *)comp->name)));
    for (nu_size_t i = 0; i < count; ++i)
    {
        for (nu_size_t j = 0; j < _ctx.ecs.attributes.size; ++j)
        {
            const nu__ecs_attribute_t *a = _ctx.ecs.attributes.data + j;
            if (a->component == c)
            {
                const nu_byte_t *p = data + a->offset;
                nu__ecs_dump_attribute(depth + 1, a, data);
            }
        }
    }
    nu__ecs_print_with_depth(depth, NU_STR("}"));
}
static void
nu__ecs_dump_object (nu_size_t        depth,
                     nu_object_type_t type,
                     nu_size_t        count,
                     const nu_byte_t *data)
{
    for (nu_size_t i = 0; i < count; ++i)
    {
        nu_byte_t   *p   = (nu_byte_t *)data + sizeof(nu_object_t) * i;
        nu_object_t *obj = (nu_object_t *)p;
        if (*obj)
        {
            nu_uid_t uid = nu_object_get_uid(obj);
            NU_ASSERT(uid);
            nu__ecs_print_with_depth(depth, NU_STR("%llu"), uid);
        }
        else
        {
            nu__ecs_print_with_depth(depth, NU_STR("null"));
        }
    }
}
static void
nu__ecs_dump_primitive (nu_size_t          depth,
                        nu_ecs_primitive_t primitive,
                        nu_size_t          count,
                        const nu_byte_t   *data)
{
    for (nu_size_t i = 0; i < count; ++i)
    {
        const nu_byte_t *p = data + nu__ecs_primitive_size(primitive) * i;
        switch (primitive)
        {
            case NU_ECS_BYTE:
                nu__ecs_print_with_depth(depth, NU_STR("%d"), *(nu_u8_t *)p);
                break;
            case NU_ECS_U32:
                nu__ecs_print_with_depth(depth, NU_STR("%d"), *(nu_u32_t *)p);
                break;
            case NU_ECS_F32:
                nu__ecs_print_with_depth(depth, NU_STR("%lf"), *(nu_f32_t *)p);
                break;
            case NU_ECS_V3: {
                nu_v3_t *v = (nu_v3_t *)p;
                nu__ecs_print_with_depth(
                    depth, NU_STR("[" NU_V3_FMT "]"), NU_V3_ARGS(v));
            }
            break;
            case NU_ECS_Q4: {
                nu_q4_t *v = (nu_q4_t *)p;
                nu__ecs_print_with_depth(
                    depth, NU_STR("[" NU_Q4_FMT "]"), NU_Q4_ARGS(v));
            }
            break;
            case NU_ECS_M4: {
                nu_m4_t *m = (nu_m4_t *)p;
                nu__ecs_print_with_depth(
                    depth, NU_STR("[" NU_M4_FMT "]"), NU_M4_ARGS(m));
            }
            break;
        }
    }
}
static void
nu__ecs_dump_attribute (nu_size_t                  depth,
                        const nu__ecs_attribute_t *a,
                        const nu_byte_t           *data)
{
    nu__ecs_print_with_depth(
        depth,
        NU_STR(NU_STR_FMT ":"),
        NU_STR_ARGS(nu_str_from_cstr((nu_byte_t *)a->name)));
    const nu_byte_t *p = data + a->offset;
    switch (a->info.kind)
    {
        case NU_ECS_ATTR_PRIMITIVE:
            nu__ecs_dump_primitive(depth + 1, a->info.p, a->count, p);
            break;
        case NU_ECS_ATTR_COMPONENT:
            nu__ecs_dump_component(depth + 1, a->info.c, a->count, p);
            break;
        case NU_ECS_ATTR_OBJECT:
            nu__ecs_dump_object(depth + 1, a->info.o, a->count, p);
            break;
        case NU_ECS_ATTR_STRING:
            break;
    }
}
void
nu_ecs_dump_component (nu_ecs_t ecs, nu_ecs_id_t e, nu_ecs_id_t c)
{
    const nu__ecs_component_t *comp = _ctx.ecs.components.data + c;
    const void                *data = nu_ecs_get(ecs, e, c);
    nu__ecs_dump_component(0, c, 1, data);
}
void
nu_ecs_dump_entity (nu_ecs_t ecs, nu_ecs_id_t e)
{
    const nu__ecs_instance_t *ins = (nu__ecs_instance_t *)ecs;
    NU_INFO("entity #%d {", e);
    for (nu_size_t c = 0; c < ins->pools.size; ++c)
    {
        nu_ecs_id_t cid = NU_ID_MAKE(c);
        if (nu_ecs_has(ecs, e, cid))
        {
            const void *data = nu_ecs_get(ecs, e, cid);
            nu__ecs_dump_component(1, cid, 1, data);
        }
    }
    NU_INFO("}");
}
void
nu_ecs_dump (nu_ecs_t ecs)
{
    const nu__ecs_instance_t *ins = (nu__ecs_instance_t *)ecs;
    for (nu_size_t i = 0; i < nu_ecs_capacity(ecs); ++i)
    {
        nu_ecs_id_t e = NU_ID_MAKE(i);
        if (nu_ecs_valid(ecs, e))
        {
            nu_ecs_dump_entity(ecs, e);
        }
    }
}

#endif

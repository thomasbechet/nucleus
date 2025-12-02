#ifndef NU_SLOTMAP_H
#define NU_SLOTMAP_H

#include <nucleus/vm/allocator.h>
#include <nucleus/vm/types.h>

typedef struct nu__slotmap *nu__slotmap_t;
typedef nu_u16_t            nu__slot_t;

#define NU_SLOT_NULL NU_NULL
#define NU_SLOT_MAX  NU_U16_MAX - 1

NU_API nu_u16_t nu_slot_index(nu__slot_t index);

NU_API void       nu__slotmap_init(nu__allocator_t  *alloc,
                                   nu_memory_usage_t usage,
                                   nu_size_t         size,
                                   nu_u16_t          capacity,
                                   nu__slotmap_t    *slotmap);
NU_API void      *nu__slotmap_get(nu__slotmap_t slotmap, nu__slot_t slot);
NU_API nu__slot_t nu__slotmap_add(nu__slotmap_t slotmap);
NU_API void       nu__slotmap_remove(nu__slotmap_t slotmap, nu__slot_t slot);

typedef struct nu__slotlist_item *nu__slotlist_t;

NU_API void       nu__slotlist_init(nu__allocator_t  *alloc,
                                    nu_memory_usage_t usage,
                                    nu_u16_t          capacity,
                                    nu__slotlist_t   *slotlist);
NU_API void       nu__slotlist_add_first(nu__slotlist_t slotlist,
                                         nu__slot_t    *first,
                                         nu__slot_t     slot);
NU_API void       nu__slotlist_remove(nu__slotlist_t slotlist,
                                      nu__slot_t    *first,
                                      nu__slot_t     slot);
NU_API nu__slot_t nu__slotlist_next(nu__slotlist_t slotlist, nu__slot_t slot);

#ifdef NU_IMPL

struct nu__slotmap
{
    nu_u16_t   capacity;
    nu__slot_t free;
    nu_size_t  obj_size;
};

static void *
nu__slotmap_data (nu__slotmap_t slotmap)
{
    return (void *)((nu_size_t)slotmap + sizeof(struct nu__slotmap));
}

nu_u16_t
nu_slot_index (nu__slot_t index)
{
    NU_ASSERT(index);
    return index - 1;
}

void
nu__slotmap_init (nu__allocator_t  *alloc,
                  nu_memory_usage_t usage,
                  nu_size_t         obj_size,
                  nu_u16_t          capacity,
                  nu__slotmap_t    *slotmap)
{
    struct nu__slotmap *sm;
    nu_size_t           i;
    void               *data;

    NU_ASSERT(obj_size >= sizeof(nu__slot_t));
    NU_ASSERT(capacity > 0);

    sm = nu__alloc(
        alloc, sizeof(struct nu__slotmap) + obj_size * capacity, usage);
    sm->capacity = capacity;
    sm->obj_size = obj_size;
    sm->free     = 1;

    data = nu__slotmap_data(sm);
    for (i = 0; i < capacity; i++)
    {
        nu__slot_t *ptr = (nu__slot_t *)((nu_size_t)data + obj_size * i);
        *ptr            = i + 2;
    }

    *slotmap = sm;
}
void *
nu__slotmap_get (nu__slotmap_t slotmap, nu__slot_t slot)
{
    void *data;
    if (!slot)
    {
        return NU_NULL;
    }
    data = nu__slotmap_data(slotmap);
    return (void *)((nu_size_t)data + slotmap->obj_size * (slot - 1));
}
nu__slot_t
nu__slotmap_add (nu__slotmap_t slotmap)
{
    nu__slot_t free = slotmap->free;
    void      *data = nu__slotmap_data(slotmap);

    NU_ASSERT(free <= slotmap->capacity);
    slotmap->free
        = *(nu__slot_t *)((nu_size_t)data + slotmap->obj_size * (free - 1));

    return free;
}
void
nu__slotmap_remove (nu__slotmap_t slotmap, nu__slot_t slot)
{
    void *data;
    if (!slot)
    {
        return;
    }
    data = nu__slotmap_data(slotmap);
    *(nu__slot_t *)((nu_size_t)data + slotmap->obj_size * (slot - 1))
        = slotmap->free;
    slotmap->free = slot;
}

struct nu__slotlist_item
{
    nu__slot_t prev;
    nu__slot_t next;
};

void
nu__slotlist_init (nu__allocator_t  *alloc,
                   nu_memory_usage_t usage,
                   nu_u16_t          capacity,
                   nu__slotlist_t   *slotlist)
{
    nu__slotlist_t sl;
    nu_size_t      i;

    sl = nu__alloc(alloc, sizeof(struct nu__slotlist_item) * capacity, usage);
    for (i = 0; i < capacity; ++i)
    {
        sl[i].prev = NU_SLOT_NULL;
        sl[i].next = NU_SLOT_NULL;
    }

    *slotlist = sl;
}
void
nu__slotlist_add_first (nu__slotlist_t slotlist,
                        nu__slot_t    *first,
                        nu__slot_t     slot)
{
    struct nu__slotlist_item *item;
    if (!slot)
    {
        return;
    }
    item = &slotlist[slot - 1];
    if (*first)
    {
        struct nu__slotlist_item *first_item = &slotlist[*first - 1];
        first_item->prev                     = slot;
        item->next                           = *first;
    }
    else
    {
        item->next = NU_SLOT_NULL;
    }
    item->prev = NU_SLOT_NULL;
    *first     = slot;
}
void
nu__slotlist_remove (nu__slotlist_t slotlist,
                     nu__slot_t    *first,
                     nu__slot_t     slot)
{
    struct nu__slotlist_item *item;
    if (!slot)
    {
        return;
    }
    item = &slotlist[slot - 1];
    if (item->prev)
    {
        slotlist[item->prev - 1].next = item->next;
    }
    if (item->next)
    {
        slotlist[item->next - 1].prev = item->prev;
    }
    if (*first == slot)
    {
        *first = item->next;
    }
}
nu__slot_t
nu__slotlist_next (nu__slotlist_t slotlist, nu__slot_t slot)
{
    if (!slot)
    {
        return NU_SLOT_NULL;
    }
    return slotlist[slot - 1].next;
}

#endif

#endif

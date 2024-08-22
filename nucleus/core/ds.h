#ifndef NU_DS_H
#define NU_DS_H

#include <nucleus/core/memory.h>

//////////////////////////////////////////////////////////////////////////
//////                           Vector                             //////
//////////////////////////////////////////////////////////////////////////

#define nu_vec(type)        \
    struct                  \
    {                       \
        type     *data;     \
        nu_size_t size;     \
        nu_size_t capacity; \
    }

#define nu_vec_init(v, alloc, cap)                                   \
    do                                                               \
    {                                                                \
        (v)->data     = nu_alloc(alloc, sizeof(*(v)->data) * (cap)); \
        (v)->capacity = (cap);                                       \
        (v)->size     = 0;                                           \
    } while (0)
#define nu_vec_free(v, alloc) \
    nu_free(alloc, (v)->data, sizeof(*(v)->data) * (v)->capacity)
#define nu_vec_clear(v) (v)->size = 0
#define nu_vec_push(v, alloc)                                                       \
    do                                                                              \
    {                                                                               \
        if ((v)->size >= (v)->capacity)                                             \
        {                                                                           \
            nu_size_t new_capacity = (v)->capacity * 2;                             \
            (v)->data              = nu_realloc(alloc,                              \
                                   (v)->data,                          \
                                   sizeof(*(v)->data) * (v)->capacity, \
                                   sizeof(*(v)->data) * new_capacity); \
            (v)->capacity          = new_capacity;                                  \
        }                                                                           \
        ++(v)->size;                                                                \
    } while (0)
#define nu_vec_pop(v)    \
    do                   \
    {                    \
        if ((v)->size)   \
        {                \
            --(v)->size; \
        }                \
    } while (0)
#define nu_vec_last(v) (v)->size ? (v)->data + ((v)->size - 1) : NU_NULL

typedef nu_vec(nu_u32_t) nu_u32_vec_t;

//////////////////////////////////////////////////////////////////////////
//////                           Slotmap                            //////
//////////////////////////////////////////////////////////////////////////

typedef nu_u32_t nu_slot_t;

#define nu_slotmap(type)     \
    struct                   \
    {                        \
        struct               \
        {                    \
            type      value; \
            nu_slot_t next;  \
        }        *data;      \
        nu_slot_t free;      \
        nu_size_t capacity;  \
    }

#define nu_slotmap_init(s, alloc, cap)                               \
    do                                                               \
    {                                                                \
        (s)->data     = nu_alloc(alloc, sizeof(*(s)->data) * (cap)); \
        (s)->capacity = cap;                                         \
        (s)->free     = 1;                                           \
        for (nu_size_t i = 1; i < (cap); ++i)                        \
        {                                                            \
            (s)->data[i - 1].next = i;                               \
        }                                                            \
        (s)->data[(s)->capacity - 1].next = 0;                       \
    } while (0)

#define nu_slotmap_free(s, alloc) \
    nu_free(alloc, (s)->data, sizeof(*(s)->data) * (s)->capacity)

#define nu_slotmap_get(s, slot) (slot ? &(s)->data[slot - 1].value : NU_NULL)

#define nu_slotmap_add(s, alloc, pslot)                                             \
    do                                                                              \
    {                                                                               \
        if ((s)->free)                                                              \
        {                                                                           \
            *(pslot)  = (s)->free;                                                  \
            (s)->free = (s)->data[(s)->free].next;                                  \
        }                                                                           \
        else                                                                        \
        {                                                                           \
            nu_size_t new_capacity = (s)->capacity * 2;                             \
            (s)->data              = nu_realloc(alloc,                              \
                                   (s)->data,                          \
                                   sizeof(*(s)->data) * (s)->capacity, \
                                   sizeof(*(s)->data) * new_capacity); \
            for (nu_size_t i = (s)->capacity; i < (new_capacity); ++i)              \
            {                                                                       \
                (s)->data[i - 1].next = i;                                          \
            }                                                                       \
            (s)->data[new_capacity - 1].next = 0;                                   \
            *(pslot)                         = (s)->capacity;                       \
            (s)->free                        = (s)->capacity + 1;                   \
            (s)->capacity                    = new_capacity;                        \
        }                                                                           \
    } while (0)

#define nu_slotmap_remove(s, slot)              \
    do                                          \
    {                                           \
        if ((slot))                             \
        {                                       \
            (s)->data[(slot)].next = (s)->free; \
            (s)->free              = slot;      \
        }                                       \
    } while (0)

typedef nu_slotmap(nu_u32_t) nu_u32_slotmap_t;

#endif

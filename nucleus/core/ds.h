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

#define nu_vec_clear(v) ((v)->size = 0)

NU_API void     *nu__vec_push(nu_allocator_t *alloc,
                              nu_size_t       tsize,
                              void           *data,
                              nu_size_t      *size,
                              nu_size_t      *capacity);
NU_API nu_bool_t nu__vec_pop(nu_size_t *size);

#define nu_vec_push(v, alloc)                                                \
    (((v)->data = nu__vec_push(                                              \
          alloc, sizeof(*(v)->data), (v)->data, &(v)->size, &(v)->capacity)) \
         ? (v)->data + ((v)->size - 1)                                       \
         : NU_NULL)

#define nu_vec_pop(v) \
    (nu__vec_pop(&(v)->size) ? (v)->data + (v)->size : NU_NULL)
#define nu_vec_last(v) ((v)->size ? (v)->data + ((v)->size - 1) : NU_NULL)

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
        (s)->data[(s)->capacity - 1].next = NU_NULL;                 \
    } while (0)

#define nu_slotmap_free(s, alloc) \
    nu_free(alloc, (s)->data, sizeof(*(s)->data) * (s)->capacity)

#define nu_slotmap_get(s, slot) \
    ((slot) ? &(s)->data[(slot) - 1].value : NU_NULL)

#define nu_slotmap_add(s, alloc, pslot)                                             \
    do                                                                              \
    {                                                                               \
        if ((s)->free)                                                              \
        {                                                                           \
            *(pslot)  = (s)->free;                                                  \
            (s)->free = (s)->data[(s)->free - 1].next;                              \
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
            (s)->data[new_capacity - 1].next = NU_NULL;                             \
            *(pslot)                         = (s)->capacity;                       \
            (s)->free                        = (s)->capacity + 1;                   \
            (s)->capacity                    = new_capacity;                        \
        }                                                                           \
    } while (0)

#define nu_slotmap_remove(s, slot)                  \
    do                                              \
    {                                               \
        if ((slot))                                 \
        {                                           \
            (s)->data[(slot) - 1].next = (s)->free; \
            (s)->free                  = slot;      \
        }                                           \
    } while (0)

#define nu_slotmap_is_used(s, i) ((s)->data[i].next == 0xFFFFFFFF)

typedef nu_slotmap(nu_u32_t) nu_u32_slotmap_t;

#endif

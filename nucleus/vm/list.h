#ifndef NU_LIST_H
#define NU_LIST_H

#include <nucleus/vm/allocator.h>

typedef struct
{
    nu_size_t               size;
    struct nu__list_header *first;
    struct nu__list_header *head;
    struct nu__list_header *free;
} nu__list_t;

void  nu__list_init(nu__list_t *list, nu_size_t obj_size);
void *nu__list_append(nu__list_t       *list,
                      nu__allocator_t  *alloc,
                      nu_memory_usage_t usage);
void  nu__list_remove(nu__list_t *list, void *obj);
void *nu__list_first(nu__list_t *list);
void *nu__list_next(void *obj);

void *nu__list_alloc_object(nu__allocator_t  *alloc,
                            nu_memory_usage_t usage,
                            nu_size_t         size);

#ifdef NU_IMPL

struct nu__list_header
{
    struct nu__list_header *prev; /* only valid in object list */
    struct nu__list_header *next;
};

static void *
nu__object_from_header (struct nu__list_header *header)
{
    return (void *)((nu_size_t)header + sizeof(struct nu__list_header));
}

static struct nu__list_header *
nu__header_from_object (void *obj)
{
    return (struct nu__list_header *)((nu_size_t)obj
                                      - sizeof(struct nu__list_header));
}

void
nu__list_init (nu__list_t *list, nu_size_t obj_size)
{
    list->size  = obj_size + sizeof(struct nu__list_header);
    list->first = NU_NULL;
    list->head  = NU_NULL;
    list->free  = NU_NULL;
}

void *
nu__list_append (nu__list_t       *list,
                 nu__allocator_t  *alloc,
                 nu_memory_usage_t usage)
{
    struct nu__list_header *header;

    /* create object */
    if (list->free)
    {
        header = list->free;
        /* remove from free list */
        list->free = header->next;
    }
    else
    {
        /* allocate new object */
        void *object = nu__alloc(alloc, list->size, usage);
        NU_CHECK(object, return NU_NULL);
        header = nu__header_from_object(object);
    }

    /* add to object list */
    if (list->head)
    {
        list->head->next = header;
        header->prev     = list->head;
    }
    else
    {
        header->prev = NU_NULL;
        list->first  = header;
    }

    header->next = NU_NULL;
    list->head   = header;

    return nu__object_from_header(header);
}

void
nu__list_remove (nu__list_t *list, void *obj)
{
    struct nu__list_header *header = nu__header_from_object(obj);

    NU_ASSERT(obj);

    /* remove from object list */
    if (header->prev)
    {
        header->prev->next = header->next;
    }
    if (header->next)
    {
        header->next->prev = header->prev;
    }
    if (list->head == header)
    {
        list->head = header->prev;
    }
    if (list->first == header)
    {
        list->first = header->next;
    }

    /* add to free list */
    header->next = list->free;
    header->prev = NU_NULL;
    list->free   = header;
}

void *
nu__list_first (nu__list_t *list)
{
    if (list->first)
    {
        return nu__object_from_header(list->first);
    }
    return NU_NULL;
}

void *
nu__list_next (void *obj)
{
    struct nu__list_header *header;

    NU_ASSERT(obj);

    header = nu__header_from_object(obj);
    if (header->next)
    {
        return nu__object_from_header(header->next);
    }
    return NU_NULL;
}
#endif

#endif

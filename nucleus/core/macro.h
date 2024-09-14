#ifndef NU_CORE_MACRO_H
#define NU_CORE_MACRO_H

#define nu_debug(...)   nu_log(NU_LOG_DEBUG, __SOURCE__, __VA_ARGS__)
#define nu_info(...)    nu_log(NU_LOG_INFO, __SOURCE__, __VA_ARGS__)
#define nu_warning(...) nu_log(NU_LOG_WARNING, __SOURCE__, __VA_ARGS__)
#define nu_error(...)   nu_log(NU_LOG_ERROR, __SOURCE__, __VA_ARGS__)

#define nu_uid(name) nu_hash(name)

#define NU_MATCH(a, b) (nu_strcmp(a, b) == 0)

#ifdef NU_CXX
#define NU_VOID_CAST(type, expr) (static_cast<decltype(type)>(expr))
#else
#define NU_VOID_CAST(type, expr) (expr)
#endif

#define nu_vec(type)               \
    struct                         \
    {                              \
        type           *data;      \
        nu_size_t       size;      \
        nu_size_t       capacity;  \
        nu_allocator_t *allocator; \
    }

#define nu_vec_init(cap, v) nu_vec_init_a(nu_allocator_core(), cap, v)

#define nu_vec_init_a(alloc, cap, v)                                     \
    do                                                                   \
    {                                                                    \
        (v)->data = NU_VOID_CAST(                                        \
            (v)->data, nu_alloc_a((alloc), sizeof(*(v)->data) * (cap))); \
        (v)->capacity  = (cap);                                          \
        (v)->size      = 0;                                              \
        (v)->allocator = (alloc);                                        \
    } while (0)

#define nu_vec_free(v) \
    nu_free_a((v)->allocator, (v)->data, sizeof(*(v)->data) * (v)->capacity)

#define nu_vec_clear(v) ((v)->size = 0)

#define nu_vec_push(v)                                          \
    (((v)->data = NU_VOID_CAST((v)->data,                       \
                               nu__vec_push((v)->allocator,     \
                                            sizeof(*(v)->data), \
                                            (v)->data,          \
                                            &(v)->size,         \
                                            &(v)->capacity)))   \
         ? (v)->data + ((v)->size - 1)                          \
         : NU_NULL)

#define nu_vec_pop(v) \
    (nu__vec_pop(&(v)->size) ? ((v)->data + (v)->size) : NU_NULL)

#define nu_vec_resize(v, new_size)                               \
    do                                                           \
    {                                                            \
        if ((v)->size != (new_size))                             \
        {                                                        \
            (v)->data = NU_VOID_CAST(                            \
                (v)->data,                                       \
                nu_realloc_a((v)->allocator,                     \
                             (v)->data,                          \
                             sizeof(*(v)->data) * (v)->capacity, \
                             sizeof(*(v)->data) * (new_size)));  \
            (v)->capacity = (new_size);                          \
            (v)->size     = (new_size);                          \
        }                                                        \
    } while (0)

#define nu_vec_last(v) ((v)->size ? (v)->data + ((v)->size - 1) : NU_NULL)

#define nu_pool(type)            \
    struct                       \
    {                            \
        type         *data;      \
        nu_size_t     capacity;  \
        nu_size_vec_t _freelist; \
    }

#define nu_pool_init(cap, p) nu_pool_init_a(nu_allocator_core(), cap, p)

#define nu_pool_init_a(alloc, cap, p)                                    \
    do                                                                   \
    {                                                                    \
        (p)->data = NU_VOID_CAST(                                        \
            (p)->data, nu_alloc_a((alloc), sizeof(*(p)->data) * (cap))); \
        nu_memset((p)->data, 0, sizeof(*(p)->data) * (cap));             \
        (p)->capacity = (cap);                                           \
        nu_vec_init_a(alloc, (cap), &(p)->_freelist);                    \
        for (nu_size_t i = 0; i < (cap); ++i)                            \
        {                                                                \
            *nu_vec_push(&(p)->_freelist) = (cap) - i - 1;               \
        }                                                                \
    } while (0)

#define nu_pool_free(p)                                \
    do                                                 \
    {                                                  \
        nu_free_a((p)->_freelist.allocator,            \
                  (p)->data,                           \
                  sizeof(*(p)->data) * (p)->capacity); \
        nu_vec_free(&(p)->_freelist);                  \
    } while (0)

#define nu_pool_add(p, pindex)                                  \
    (((p)->data = NU_VOID_CAST((p)->data,                       \
                               nu__pool_add(sizeof(*(p)->data), \
                                            (p)->data,          \
                                            &(p)->capacity,     \
                                            &(p)->_freelist,    \
                                            (pindex))))         \
         ? (p)->data + (*(pindex))                              \
         : NU_NULL)

#define nu_pool_remove(s, index) (*nu_vec_push_checked(&(s)->_freelist) = index)

#endif

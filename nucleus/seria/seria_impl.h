#ifndef NU_SERIA_IMPL_H
#define NU_SERIA_IMPL_H

#include <nucleus/internal.h>

// #include <nucleus/seria/reflect_impl.h>
// #include <nucleus/seria/dumper_impl.h>
// #ifdef NU_BUILD_JSMN
// #include <nucleus/seria/json_impl.h>
// #endif

#define NU__SERIA_PRIMITIVE(primitive)                                 \
    {                                                                  \
        nu__seria_layout_t *layout = NU_VEC_PUSH(&_ctx.seria.layouts); \
        layout->name               = #primitive;                       \
        layout->size               = sizeof(primitive);                \
        layout->is_primitive       = NU_TRUE;                          \
    }
static void
nu__seria_register_core_types (void)
{
    NU__SERIA_PRIMITIVE(nu_u32_t);
    NU__SERIA_PRIMITIVE(nu_u8_t);
    NU__SERIA_PRIMITIVE(nu_i8_t);
    NU__SERIA_PRIMITIVE(nu_u16_t);
    NU__SERIA_PRIMITIVE(nu_i16_t);
    NU__SERIA_PRIMITIVE(nu_u32_t);
    NU__SERIA_PRIMITIVE(nu_i32_t);
    NU__SERIA_PRIMITIVE(nu_u64_t);
    NU__SERIA_PRIMITIVE(nu_i64_t);
    NU__SERIA_PRIMITIVE(nu_f32_t);
    NU__SERIA_PRIMITIVE(nu_f64_t);

    NU__SERIA_PRIMITIVE(nu_char_t);
    NU__SERIA_PRIMITIVE(nu_bool_t);
    NU__SERIA_PRIMITIVE(nu_int_t);
    NU__SERIA_PRIMITIVE(nu_size_t);
    NU__SERIA_PRIMITIVE(nu_intptr_t);
    NU__SERIA_PRIMITIVE(nu_byte_t);
    NU__SERIA_PRIMITIVE(nu_word_t);
    NU__SERIA_PRIMITIVE(nu_uid_t);
}

static void
nu__seria_init (void)
{
    NU_POOL_INIT(1, &_ctx.seria.instances);
    NU_VEC_INIT(10, &_ctx.seria.layouts);
    nu__seria_register_core_types();
}
static void
nu__seria_free (void)
{
    for (nu_size_t i = 0; i < _ctx.seria.layouts.size; ++i)
    {
        NU_VEC_FREE(&_ctx.seria.layouts.data[i].accessors);
    }
    NU_VEC_FREE(&_ctx.seria.layouts);
    NU_POOL_FREE(&_ctx.seria.instances);
}

nu_seria_t
nu_seria_create (void)
{
    nu_size_t             index;
    nu__seria_instance_t *s = NU_POOL_ADD(&_ctx.seria.instances, &index);
    s->is_open              = NU_FALSE;
    return NU_HANDLE_MAKE(nu_seria_t, index);
}
void
nu_seria_delete (nu_seria_t seria)
{
    nu__seria_instance_t *s
        = _ctx.seria.instances.data + NU_HANDLE_INDEX(seria);
    if (s->is_open)
    {
        nu_seria_close(seria);
    }
    NU_POOL_REMOVE(&_ctx.seria.instances, NU_HANDLE_INDEX(seria));
}

void
nu_seria_open_reflect (nu_seria_t seria)
{
    nu__seria_instance_t *s
        = _ctx.seria.instances.data + NU_HANDLE_INDEX(seria);
    s->is_open = NU_TRUE;
}
void
nu_seria_open_dumper (nu_seria_t seria)
{
}
#ifdef NU_BUILD_JSMN
void
nu_seria_open_json (nu_seria_t       seria,
                    const jsmntok_t *toks,
                    nu_size_t        toks_count)
{
}
#endif
void
nu_seria_close (nu_seria_t seria)
{
    nu__seria_instance_t *s
        = _ctx.seria.instances.data + NU_HANDLE_INDEX(seria);
}

nu_seria_layout_t
nu_seria_layout_create (const nu_char_t *name, nu_size_t size)
{
    nu__seria_layout_t *layout = NU_VEC_PUSH(&_ctx.seria.layouts);
    layout->name               = name;
    layout->size               = size;
    layout->is_primitive       = NU_FALSE;
    NU_VEC_INIT(5, &layout->accessors);
    return NU_HANDLE_MAKE(nu_seria_layout_t, _ctx.seria.layouts.size - 1);
}
void
nu_seria_layout_access (nu_seria_layout_t layout,
                        const nu_char_t  *name,
                        nu_size_t         offset,
                        nu_seria_layout_t access_layout,
                        nu_size_t         size)
{
    NU_ASSERT(layout && name && size && access_layout);
    nu__seria_layout_t   *p = _ctx.seria.layouts.data + NU_HANDLE_INDEX(layout);
    nu__seria_accessor_t *a = NU_VEC_PUSH(&p->accessors);
    a->name                 = name;
    a->size                 = size;
    a->layout               = access_layout;
    a->offset               = offset;
}
nu_seria_layout_t
nu_seria_layout (const nu_char_t *name)
{
    for (nu_size_t i = 0; i < _ctx.seria.layouts.size; ++i)
    {
        if (NU_MATCH(name, _ctx.seria.layouts.data[i].name))
        {
            return NU_HANDLE_MAKE(nu_seria_layout_t, i);
        }
    }
    return NU_NULL;
}

#endif

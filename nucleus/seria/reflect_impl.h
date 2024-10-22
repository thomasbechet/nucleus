#ifndef NU_SERIA_REFLECT_IMPL_H
#define NU_SERIA_REFLECT_IMPL_H

#include <nucleus/internal.h>

static void
nu__print_with_depth (nu_size_t depth, const nu_char_t *format, ...)
{
    va_list args;
    va_start(args, format);

    nu_char_t str[256];
    nu_vsnprintf(str, 256, format, args);
    NU_INFO("%*s%s", depth * 2, "", str);

    va_end(args);
}

static void
nu__seria_reflect_open (nu__seria_ctx_t *ctx)
{
    NU_VEC_INIT(10, &ctx->reflect.entries);
    ctx->reflect.last_type    = -1;
    ctx->reflect.working_type = -1;
    ctx->reflect.skip_depth   = -1;
}
static void
nu__seria_reflect_close (nu__seria_ctx_t *ctx)
{
    NU_VEC_FREE(&ctx->reflect.entries);
}

void
nu__seria_reflect_enter_type (nu__seria_ctx_t *ctx)
{
    // check existing type
    nu_int_t current_type = ctx->reflect.last_type;
    while (current_type != -1)
    {
        nu__seria_reflect_entry_t *entry
            = ctx->reflect.entries.data + current_type;
        if (NU_MATCH(entry->name, ctx->enter_type.name))
        {
            // type is already known, we can skip the type
            ctx->reflect.skip_depth = ctx->enter_type.depth;
            return;
        }
        current_type = entry->prev;
    }

    // we can register the type
    nu__seria_reflect_entry_t *entry = NU_VEC_PUSH(&ctx->reflect.entries);
    entry->name                      = ctx->enter_type.name;
    entry->prev                      = ctx->reflect.last_type;
    entry->type.last_field           = -1;
    entry->type.depth                = ctx->enter_type.depth;
    ctx->reflect.last_type           = ctx->reflect.entries.size - 1;

    // mark the type as working type
    ctx->reflect.working_type = ctx->reflect.last_type;
}
void
nu__seria_reflect_leave_type (nu__seria_ctx_t *ctx)
{
    // detect end of root object
    if (ctx->leave_type.depth == 0)
    {
        nu_int_t current_type = ctx->reflect.last_type;
        while (current_type != -1)
        {
            nu__seria_reflect_entry_t *entry
                = ctx->reflect.entries.data + current_type;

            nu__print_with_depth(0, "%s {", entry->name);
            nu_int_t current_field = entry->type.last_field;
            while (current_field != -1)
            {
                nu__seria_reflect_entry_t *field
                    = ctx->reflect.entries.data + current_field;

                // find field typename
                const nu_char_t *field_typename;
                if (field->field.is_primitive)
                {
                    field_typename = field->field.primitive;
                }
                else
                {
                    field_typename
                        = ctx->reflect.entries.data[field->field.type].name;
                }

                if (field->field.card == 0) // variable size
                {
                    nu__print_with_depth(
                        1, "%s <%s,*>", field->name, field_typename);
                }
                else
                {
                    nu__print_with_depth(1,
                                         "%s <%s,%d>",
                                         field->name,
                                         field_typename,
                                         field->field.card);
                }
                current_field = field->prev;
            }
            nu__print_with_depth(0, "}");

            current_type = entry->prev;
        }

        return;
    }

    // reset skipped depth
    ctx->reflect.skip_depth = (nu_size_t)-1;
}
static void
nu__seria_reflect_enter_field (nu__seria_ctx_t *ctx)
{
}
static void
nu__seria_reflect_leave_field (nu__seria_ctx_t *ctx)
{
    if (ctx->leave_field.depth < ctx->reflect.skip_depth)
    {
        // find working type
        nu__seria_reflect_entry_t *working_type
            = ctx->reflect.entries.data + ctx->reflect.working_type;

        // find previous working type
        nu_int_t current_type = ctx->reflect.working_type;
        while (current_type != -1)
        {
            nu__seria_reflect_entry_t *entry
                = ctx->reflect.entries.data + current_type;
            if (working_type->type.depth - 1 == ctx->leave_type.depth)
            {
                ctx->reflect.working_type = current_type;
                break;
            }
            current_type = entry->prev;
        }

        // insert field
        nu__seria_reflect_entry_t *entry = NU_VEC_PUSH(&ctx->reflect.entries);
        entry->name                      = ctx->leave_field.name;
        entry->prev                      = working_type->type.last_field;
        working_type->type.last_field    = ctx->reflect.entries.size - 1;
        entry->field.card                = ctx->leave_field.card;
        entry->field.is_primitive        = NU_FALSE;
        entry->field.type                = ctx->reflect.working_type;
    }
}
static void
nu__seria_reflect_enter_primitive (nu__seria_ctx_t *ctx)
{
    if (ctx->enter_primitive.depth < ctx->reflect.skip_depth)
    {
        // update last field of current type with core type
        nu__seria_reflect_entry_t *working_type
            = ctx->reflect.entries.data + ctx->reflect.working_type;
        nu__seria_reflect_entry_t *last_field
            = ctx->reflect.entries.data + working_type->type.last_field;
        last_field->name = nu__seria_names[ctx->enter_primitive.primitive];
    }
}

static void
nu__seria_reflect_hooks (nu__seria_hooks_t *hooks)
{
    hooks->open            = nu__seria_reflect_open;
    hooks->close           = nu__seria_reflect_close;
    hooks->enter_type      = nu__seria_reflect_enter_type;
    hooks->leave_type      = nu__seria_reflect_leave_type;
    hooks->enter_field     = nu__seria_reflect_enter_field;
    hooks->leave_field     = nu__seria_reflect_leave_field;
    hooks->enter_primitive = nu__seria_reflect_enter_primitive;
}

#endif

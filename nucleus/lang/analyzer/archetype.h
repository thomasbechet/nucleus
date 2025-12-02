#ifndef NULANG_ANALYZER_ARCHETYPE_H
#define NULANG_ANALYZER_ARCHETYPE_H

#include <nucleus/lang/ast.h>
#include <nucleus/vm.h>

typedef struct
{
    const struct nu__vm  *vm;
    nulang__error_data_t *error_data;
} nulang__archetype_resolver_t;

static nulang__error_t
nulang__find_archetype (nulang__archetype_resolver_t *resolver,
                        nulang__string_t              name,
                        nulang__span_t                span,
                        nu_archetype_t               *archetype)
{
    *archetype
        = nu__archetype_find(&resolver->vm->tables, nu_uidn(name.p, name.n));
    if (*archetype == NU_ARCHETYPE_NULL)
    {
        resolver->error_data->span = span;
        return NULANG_ERROR_ARCHETYPE_NOT_FOUND;
    }
    return NULANG_ERROR_NONE;
}
static nulang__error_t
nulang__resolve_vartype (nulang__archetype_resolver_t *resolver,
                         nulang__span_t                span,
                         nulang__vartype_t            *vartype)
{
    if (vartype->primitive == NU_PRIMITIVE_ENTITY)
    {
        return nulang__find_archetype(
            resolver, vartype->archetype_name, span, &vartype->archetype);
    }
    return NULANG_ERROR_NONE;
}

static nulang__error_t
nulang__resolve_archetype (nulang__ast_t    *ast,
                           nulang__node_id_t id,
                           nulang__node_t   *node,
                           void             *data)
{
    nulang__error_t               error;
    nulang__archetype_resolver_t *resolver = data;
    switch (node->type)
    {
        case AST_VARDECL: {
            error = nulang__resolve_vartype(
                resolver, node->span, &node->value.vardecl.type);
            NULANG_ERROR_CHECK(error);
        }
        break;
        case AST_ARGDECL: {
            error = nulang__resolve_vartype(
                resolver, node->span, &node->value.argdecl.type);
            NULANG_ERROR_CHECK(error);
        }
        break;
        case AST_FUNDECL: {
            error = nulang__resolve_vartype(
                resolver, node->span, &node->value.fundecl.return_type);
            NULANG_ERROR_CHECK(error);
        }
        break;
        case AST_INSERT: {
            error = nulang__find_archetype(resolver,
                                           node->value.insert.archetype_name,
                                           node->span,
                                           &node->value.insert.archetype);
            NULANG_ERROR_CHECK(error);
        }
        break;
        case AST_SINGLETON: {
            error = nulang__find_archetype(resolver,
                                           node->value.singleton.archetype_name,
                                           node->span,
                                           &node->value.singleton.archetype);
            NULANG_ERROR_CHECK(error);
        }
        break;
        default:
            break;
    }

    error
        = nulang__ast_foreach_childs(ast, id, nulang__resolve_archetype, data);
    NULANG_ERROR_CHECK(error);

    return NULANG_ERROR_NONE;
}

static nulang__error_t
nulang__resolve_archetypes (nulang__ast_t        *ast,
                            const struct nu__vm  *vm,
                            nulang__error_data_t *error_data)
{
    nulang__error_t              error;
    nulang__archetype_resolver_t resolver;

    resolver.vm         = vm;
    resolver.error_data = error_data;

    error = nulang__resolve_archetype(ast, ast->root, ast->nodes, &resolver);
    NULANG_ERROR_CHECK(error);
    return NULANG_ERROR_NONE;
}

#endif

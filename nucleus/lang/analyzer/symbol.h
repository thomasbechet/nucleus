#ifndef NULANG_ANALYZER_SYMBOL_H
#define NULANG_ANALYZER_SYMBOL_H

#include <nucleus/lang/ast.h>
#include <nucleus/vm.h>

typedef struct
{
    nulang__ast_t        *ast;
    nulang__string_t      names[1024];
    nulang__node_id_t     ids[1024];
    nu_size_t             size;
    const struct nu__vm  *vm;
    nulang__error_data_t *error_data;
} nulang__symbol_resolver_t;

static nulang__error_t
nulang__find_symbol (nulang__symbol_resolver_t *resolver,
                     nulang__node_id_t          symref,
                     nulang__string_t           name,
                     nulang__node_id_t         *id)
{
    nu_size_t n = resolver->size;
    while (n--)
    {
        if (NULANG_SOURCE_STRING_EQUALS(resolver->names[n], name))
        {
            *id = resolver->ids[n];
            return NULANG_ERROR_NONE;
        }
    }
    resolver->error_data->span = resolver->ast->nodes[symref].span;
    return NULANG_ERROR_UNRESOLVED_SYMBOL;
}
static nulang__error_t
nulang__define_symbol (nulang__symbol_resolver_t *resolver,
                       nulang__string_t           name,
                       nulang__node_id_t          id)
{
    nulang__node_id_t dummy;
    if (nulang__find_symbol(resolver, id, name, &dummy)
        != NULANG_ERROR_UNRESOLVED_SYMBOL)
    {
        resolver->error_data->span = resolver->ast->nodes[id].span;
        return NULANG_ERROR_SYMBOL_ALREADY_DEFINED;
    }
    resolver->names[resolver->size] = name;
    resolver->ids[resolver->size]   = id;
    resolver->size++;
    return NULANG_ERROR_NONE;
}
static nulang__error_t
nulang__resolve_symbols_scoped (nulang__ast_t    *ast,
                                nulang__node_id_t id,
                                nulang__node_t   *node,
                                void             *data)
{
    nulang__error_t            error;
    nulang__symbol_resolver_t *resolver = data;
    nu_size_t                  size     = resolver->size;
    switch (node->type)
    {
        case AST_VARDECL: {
            error
                = nulang__define_symbol(resolver, node->value.vardecl.name, id);
            NULANG_ERROR_CHECK(error);
        }
        break;
        case AST_ARGDECL: {
            error
                = nulang__define_symbol(resolver, node->value.argdecl.name, id);
            NULANG_ERROR_CHECK(error);
        }
        break;
        case AST_SYMREF: {
            nulang__node_id_t ref;
            error = nulang__find_symbol(
                resolver, id, ast->nodes[id].value.symref.name, &ref);
            NULANG_ERROR_CHECK(error);
            ast->nodes[id].value.symref.node = ref;
        }
        break;
        default:
            break;
    }

    error = nulang__ast_foreach_childs(
        ast, id, nulang__resolve_symbols_scoped, data);
    NULANG_ERROR_CHECK(error);

    switch (node->type)
    {
        case AST_COMPOUND:
        case AST_FUNDECL:
            resolver->size = size;
        default:
            break;
    }

    return NULANG_ERROR_NONE;
}
static nulang__error_t
nulang__resolve_symbols (nulang__ast_t        *ast,
                         const struct nu__vm  *vm,
                         nulang__error_data_t *error_data)
{
    nulang__error_t           error;
    nulang__symbol_resolver_t resolver;
    nulang__node_id_t         child;
    nulang__node_t           *pchild;

    resolver.ast        = ast;
    resolver.vm         = vm;
    resolver.error_data = error_data;
    resolver.size       = 0;

    /* resolve imports */
    /* resolve function declarations */
    child = NULANG_NODE_NULL;
    while ((pchild = nulang__iter_childs(ast, ast->root, &child)))
    {
        if (pchild->type == AST_FUNDECL)
        {
            error = nulang__define_symbol(
                &resolver, pchild->value.fundecl.name, child);
            NULANG_ERROR_CHECK(error);
        }
    }

    /* resolve variables with scope */
    error
        = nulang__resolve_symbols_scoped(ast, ast->root, ast->nodes, &resolver);
    NULANG_ERROR_CHECK(error);

    return NULANG_ERROR_NONE;
}

#endif

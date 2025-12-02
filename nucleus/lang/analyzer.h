#ifndef NULANG_ANALYZER_H
#define NULANG_ANALYZER_H

#include <nucleus/lang/analyzer/archetype.h>
#include <nucleus/lang/analyzer/symbol.h>
#include <nucleus/lang/ast.h>
#include <nucleus/lang/builtin.h>
#include <nucleus/lang/error.h>
#include <nucleus/lang/report.h>
#include <nucleus/lang/token.h>
#include <nucleus/vm.h>
#include <nucleus/vm/string.h>
#include <nucleus/vm/table.h>
#include <nucleus/vm/types.h>

#ifdef NU_IMPL

typedef struct
{
    nulang__vartype_t type;
    nulang__value_t   value;
} nulang__variable_t;

typedef enum
{
    NULANG_MEMBER_PRIMITIVE_FUNCTION,
    NULANG_MEMBER_MODULE_FUNCTION,
    NULANG_MEMBER_FIELD_LOOKUP
} nulang__member_type_t;

typedef struct
{
    nulang__member_type_t    type;
    nulang__string_t         ident;
    nu_archetype_t           archetype;
    nu_field_t               field;
    const nulang__builtin_t *builtin;
} nulang__member_t;

/**
 * TODOLIST:
 * - resolve symbols (+ duplicated symbols)
 * - check types
 * - check return statements
 */
static nulang__error_t
nulang__analyze (nulang__ast_t        *ast,
                 const nu_vm_t         vm,
                 nulang__error_data_t *error_data)
{
    nulang__error_t error;

    /* resolve symbols */
    error = nulang__resolve_symbols(ast, vm, error_data);
    NULANG_ERROR_CHECK(error);

    /* resolve archetypes */
    error = nulang__resolve_archetypes(ast, vm, error_data);
    NULANG_ERROR_CHECK(error);

    /* check types */
    /* error = nulang__check_types(ast, vm, error_data);
    NULANG_ERROR_CHECK(error); */

    return NULANG_ERROR_NONE;
}

#endif

#endif

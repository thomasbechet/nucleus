#ifndef NULANG_COMPILER_H
#define NULANG_COMPILER_H

#include <nucleus/lang/error.h>
#include <nucleus/lang/lexer.h>
#include <nucleus/lang/parser.h>
#include <nucleus/lang/analyzer.h>

typedef struct
{
    nu_vm_t   vm;
    nu_size_t symbol_capacity;
    nu_size_t archetype_capacity;
    nu_size_t block_capacity;
    nu_size_t node_capacity;
} nulang_compiler_info_t;

typedef struct
{
    struct nu__vm       *vm;
    nulang__ast_t        ast;
    const nu_char_t     *source;
    nulang__error_t      error;
    nulang__error_data_t error_data;
} nulang_compiler_t;

NU_API void            nulang_compiler_info_default(nu_vm_t                 vm,
                                                    nulang_compiler_info_t *info);
NU_API nulang_status_t nulang_compiler_init(const nulang_compiler_info_t *info,
                                            nulang_compiler_t *compiler);
NU_API nulang_status_t nulang_compiler_free(nulang_compiler_t *compiler);
NU_API nulang_status_t nulang_compile(nulang_compiler_t *compiler,
                                      const nu_char_t   *source);
NU_API nulang_status_t nulang_evaluate(nulang_compiler_t *compiler);

#ifdef NU_IMPL

#ifdef NU_STDLIB

#include <stdlib.h>

static void *
nulang__default_allocator (nu_size_t s, void *userdata)
{
    (void)userdata;
    return malloc(s);
}

#endif

void
nulang_compiler_info_default (nu_vm_t vm, nulang_compiler_info_t *info)
{
    info->vm                 = vm;
    info->symbol_capacity    = 256;
    info->archetype_capacity = 256;
    info->block_capacity     = 256;
    info->node_capacity      = 256;
}

nulang_status_t
nulang_compiler_init (const nulang_compiler_info_t *info,
                      nulang_compiler_t            *compiler)
{
    compiler->vm = info->vm;

    compiler->error = nulang__ast_init(
        info->node_capacity, &compiler->vm->allocator, &compiler->ast);
    if (compiler->error != NULANG_ERROR_NONE)
    {
        return NULANG_FAILURE;
    }

    return NULANG_SUCCESS;
}
nulang_status_t
nulang_compiler_free (nulang_compiler_t *compiler)
{
    (void)compiler;
    /* TODO: free resources */
    return NULANG_SUCCESS;
}
static void
nulang__compiler_prepare (nulang_compiler_t *compiler, const nu_char_t *source)
{
    nulang__ast_clear(&compiler->ast);
    compiler->source = source;
}
#include <nucleus/lang/print.h>
nulang_status_t
nulang_compile (nulang_compiler_t *compiler, const nu_char_t *source)
{
    nulang__lexer_t lexer;

    /* prepare compiler */
    nulang__compiler_prepare(compiler, source);
    nulang__lexer_init(source, &compiler->error_data, &lexer);

    /* parsing */
    compiler->error = nulang__parse(
        compiler->vm, &lexer, &compiler->ast, &compiler->error_data);
    if (compiler->error != NULANG_ERROR_NONE)
    {
        return NULANG_FAILURE;
    }

    nulang__print_node(compiler->vm, &compiler->ast, 0, compiler->ast.root);

    /* resolve symbols */
    compiler->error
        = nulang__analyze(&compiler->ast, compiler->vm, &compiler->error_data);
    if (compiler->error != NULANG_ERROR_NONE)
    {
        return NULANG_FAILURE;
    }

    return NULANG_SUCCESS;
}

#endif

#endif

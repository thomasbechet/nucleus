#ifndef NULANG_AST_H
#define NULANG_AST_H

#include <nucleus/lang/builtin.h>
#include <nucleus/lang/error.h>
#include <nucleus/lang/lexer.h>
#include <nucleus/vm/table.h>
#include <nucleus/vm/types.h>

#define NULANG_FOREACH_AST(AST) \
    AST(MODULE)                 \
    AST(IMPORT)                 \
    AST(FUNDECL)                \
    AST(COMPOUND)               \
    AST(ARGDECL)                \
    AST(LITERAL)                \
    AST(SYMREF)                 \
    AST(MEMBER)                 \
    AST(BUILTIN)                \
    AST(BREAK)                  \
    AST(CONTINUE)               \
    AST(RETURN)                 \
    AST(IF)                     \
    AST(FOR)                    \
    AST(WHILE)                  \
    AST(LOOP)                   \
    AST(VARDECL)                \
    AST(CALL)                   \
    AST(ASSIGN)                 \
    AST(BINOP)                  \
    AST(UNOP)                   \
    AST(INSERT)                 \
    AST(DELETE)                 \
    AST(SINGLETON)
#define NULANG_GENERATE_AST(AST)      AST_##AST,
#define NULANG_GENERATE_AST_NAME(AST) #AST,
typedef enum
{
    NULANG_FOREACH_AST(NULANG_GENERATE_AST)
} nulang__node_type_t;
static const nu_char_t *NULANG_AST_NAMES[]
    = { NULANG_FOREACH_AST(NULANG_GENERATE_AST_NAME) };

#define NULANG_FOREACH_BINOP(BINOP) \
    BINOP(ADD)                      \
    BINOP(SUB)                      \
    BINOP(MUL)                      \
    BINOP(DIV)                      \
    BINOP(EQUAL)                    \
    BINOP(NEQUAL)                   \
    BINOP(LEQUAL)                   \
    BINOP(GEQUAL)                   \
    BINOP(LESS)                     \
    BINOP(GREATER)                  \
    BINOP(AND)                      \
    BINOP(OR)
#define NULANG_GENERATE_BINOP(BINOP)      BINOP_##BINOP,
#define NULANG_GENERATE_BINOP_NAME(BINOP) #BINOP,
typedef enum
{
    NULANG_FOREACH_BINOP(NULANG_GENERATE_BINOP)
} nulang__binop_t;
static const nu_char_t *NULANG_BINOP_NAMES[]
    = { NULANG_FOREACH_BINOP(NULANG_GENERATE_BINOP_NAME) };

#define NULANG_FOREACH_UNOP(UNOP) \
    UNOP(NOT)                     \
    UNOP(NEG)
#define NULANG_GENERATE_UNOP(UNOP)      UNOP_##UNOP,
#define NULANG_GENERATE_UNOP_NAME(UNOP) #UNOP,
typedef enum
{
    NULANG_FOREACH_UNOP(NULANG_GENERATE_UNOP)
} nulang__unop_t;
static const nu_char_t *NULANG_UNOP_NAMES[]
    = { NULANG_FOREACH_UNOP(NULANG_GENERATE_UNOP_NAME) };

typedef nu_u32_t nulang__node_id_t;
#define NULANG_NODE_NULL 0xffffffff

typedef struct
{
    nu_primitive_t   primitive;
    nu_archetype_t   archetype;
    nulang__string_t archetype_name;
} nulang__vartype_t;

typedef union
{
    nu_bool_t   b;
    nu_int_t    i;
    nu_fix_t    f;
    nu_ivec2_t  ivec2;
    nu_ivec3_t  ivec3;
    nu_ivec4_t  ivec4;
    nu_vec2_t   vec2;
    nu_vec3_t   vec3;
    nu_vec4_t   vec4;
    nu_mat3_t   mat3;
    nu_mat4_t   mat4;
    nu_quat_t   quat;
    nu_entity_t entity;
    nu_handle_t handle;
} nulang__value_t;

#ifdef NU_IMPL

typedef struct
{
    nulang__string_t  name;
    nulang__vartype_t return_type;
    nu_bool_t         exported;
} nulang__node_fundecl_t;

typedef struct
{
    nulang__string_t  name;
    nulang__vartype_t type;
} nulang__node_argdecl_t;

typedef struct
{
    nulang__string_t  name;
    nulang__vartype_t type;
} nulang__node_vardecl_t;

typedef struct
{
    nu_archetype_t   archetype;
    nulang__string_t archetype_name;
} nulang__node_insert_t;

typedef struct
{
    nu_archetype_t   archetype;
    nulang__string_t archetype_name;
} nulang__node_singleton_t;

typedef struct
{
    nulang__node_id_t node;
    nulang__string_t  name;
} nulang__node_symref_t;

typedef union
{
    nulang__node_fundecl_t   fundecl;
    nulang__node_argdecl_t   argdecl;
    nulang__node_vardecl_t   vardecl;
    nulang__lit_t            literal;
    nulang__node_symref_t    symref;
    nulang__binop_t          binop;
    nulang__unop_t           unop;
    nulang__node_insert_t    insert;
    nulang__node_singleton_t singleton;
    nulang__builtin_t        builtin;
    nu_primitive_t           constructor;
    nulang__string_t         member;
} nulang__node_value_t;

typedef struct
{
    nulang__node_type_t  type;
    nulang__node_value_t value;
    nulang__span_t       span;
    nulang__node_id_t    parent;
    nulang__node_id_t    first_child;
    nulang__node_id_t    last_child;
    nulang__node_id_t    next_sibling;
} nulang__node_t;

typedef struct
{
    nulang__node_id_t root;
    nulang__node_t   *nodes;
    nu_u32_t          node_capacity;
    nu_u32_t          node_count;
} nulang__ast_t;

static nulang__error_t
nulang__ast_init (nu_u32_t         node_capacity,
                  nu__allocator_t *alloc,
                  nulang__ast_t   *ast)
{
    NU_ASSERT(node_capacity);
    ast->nodes = nu__alloc(alloc,
                           sizeof(nulang__node_t) * node_capacity,
                           NU_MEMORY_USAGE_COMPILER);
    if (!ast->nodes)
    {
        return NULANG_ERROR_OUT_OF_MEMORY;
    }
    ast->node_capacity         = node_capacity;
    ast->node_count            = 1;
    ast->nodes[0].type         = AST_MODULE;
    ast->nodes[0].parent       = NULANG_NODE_NULL;
    ast->nodes[0].first_child  = NULANG_NODE_NULL;
    ast->nodes[0].last_child   = NULANG_NODE_NULL;
    ast->nodes[0].next_sibling = NULANG_NODE_NULL;
    ast->root                  = 0;

    return NULANG_ERROR_NONE;
}
static void
nulang__ast_clear (nulang__ast_t *ast)
{
    ast->node_count            = 1; /* keep root node */
    ast->nodes[0].parent       = NULANG_NODE_NULL;
    ast->nodes[0].first_child  = NULANG_NODE_NULL;
    ast->nodes[0].last_child   = NULANG_NODE_NULL;
    ast->nodes[0].next_sibling = NULANG_NODE_NULL;
}
static nulang__error_t
nulang__add_node (nulang__ast_t *ast, nulang__node_id_t *id)
{
    nulang__node_t *node;
    if (ast->node_count >= ast->node_capacity)
    {
        return NULANG_ERROR_OUT_OF_NODE;
    }
    node               = &ast->nodes[ast->node_count];
    *id                = ast->node_count++;
    node->parent       = NULANG_NODE_NULL;
    node->first_child  = NULANG_NODE_NULL;
    node->last_child   = NULANG_NODE_NULL;
    node->next_sibling = NULANG_NODE_NULL;
    return NULANG_ERROR_NONE;
}

static void
nulang__append_child (nulang__ast_t    *ast,
                      nulang__node_id_t parent,
                      nulang__node_id_t child)
{
    nulang__node_id_t last_child;
    NU_ASSERT(child != 0);
    NU_ASSERT(parent != child);
    NU_ASSERT(parent != NULANG_NODE_NULL && child != NULANG_NODE_NULL);
    last_child = ast->nodes[parent].last_child;
    NU_ASSERT(last_child != child);
    NU_ASSERT(last_child != parent);
    if (last_child != NULANG_NODE_NULL)
    {
        ast->nodes[last_child].next_sibling = child;
        ast->nodes[parent].last_child       = child;
    }
    else
    {
        ast->nodes[parent].first_child = child;
        ast->nodes[parent].last_child  = child;
    }
    ast->nodes[child].parent = parent;
}

static nu_bool_t
nulang__is_mod_stmt (nulang__node_type_t t)
{
    nu_size_t                        i;
    static const nulang__node_type_t statements[] = { AST_FUNDECL, AST_IMPORT };
    for (i = 0; i < NU_ARRAY_SIZE(statements); ++i)
    {
        if (t == statements[i])
        {
            return NU_TRUE;
        }
    }
    return NU_FALSE;
}
static nu_bool_t
nulang__is_stmt (nulang__node_type_t t)
{
    nu_size_t                        i;
    static const nulang__node_type_t statements[]
        = { AST_RETURN, AST_IF,      AST_FOR,   AST_WHILE,
            AST_LOOP,   AST_VARDECL, AST_ASSIGN };
    for (i = 0; i < NU_ARRAY_SIZE(statements); ++i)
    {
        if (t == statements[i])
        {
            return NU_TRUE;
        }
    }
    return NU_FALSE;
}
static nu_bool_t
nulang__is_expr (nulang__node_type_t t)
{
    nu_size_t                        i;
    static const nulang__node_type_t expressions[]
        = { AST_LITERAL, AST_SYMREF,    AST_MEMBER, AST_BUILTIN, AST_CALL,
            AST_INSERT,  AST_SINGLETON, AST_BINOP,  AST_UNOP };
    for (i = 0; i < NU_ARRAY_SIZE(expressions); ++i)
    {
        if (t == expressions[i])
        {
            return NU_TRUE;
        }
    }
    return NU_FALSE;
}
static nu_bool_t
nulang__is_loop (nulang__node_type_t t)
{
    nu_size_t                        i;
    static const nulang__node_type_t loops[] = { AST_FOR, AST_WHILE, AST_LOOP };
    for (i = 0; i < NU_ARRAY_SIZE(loops); ++i)
    {
        if (t == loops[i])
        {
            return NU_TRUE;
        }
    }
    return NU_FALSE;
}
static nu_bool_t
nulang__is_binop (nulang__token_type_t t)
{
    nu_size_t                         i;
    static const nulang__token_type_t binops[]
        = { TOKEN_ADD,     TOKEN_MUL,    TOKEN_DIV,    TOKEN_EQUAL,
            TOKEN_NEQUAL,  TOKEN_LEQUAL, TOKEN_GEQUAL, TOKEN_LESS,
            TOKEN_GREATER, TOKEN_AND,    TOKEN_OR };
    for (i = 0; i < NU_ARRAY_SIZE(binops); ++i)
    {
        if (t == binops[i])
        {
            return NU_TRUE;
        }
    }
    return NU_FALSE;
}
static nulang__binop_t
nulang__binop_from_token (nulang__token_type_t t)
{
    switch (t)
    {
        case TOKEN_ADD:
            return BINOP_ADD;
        case TOKEN_SUB:
            return BINOP_SUB;
        case TOKEN_MUL:
            return BINOP_MUL;
        case TOKEN_DIV:
            return BINOP_DIV;
        case TOKEN_EQUAL:
            return BINOP_EQUAL;
        case TOKEN_NEQUAL:
            return BINOP_NEQUAL;
        case TOKEN_LEQUAL:
            return BINOP_LEQUAL;
        case TOKEN_GEQUAL:
            return BINOP_GEQUAL;
        case TOKEN_LESS:
            return BINOP_LESS;
        case TOKEN_GREATER:
            return BINOP_GREATER;
        case TOKEN_AND:
            return BINOP_AND;
        case TOKEN_OR:
            return BINOP_OR;
        default:
            return BINOP_ADD;
    }
}
static nu_bool_t
nulang__is_unop (nulang__token_type_t t)
{
    nu_size_t                         i;
    static const nulang__token_type_t unops[] = { TOKEN_SUB, TOKEN_NOT };
    for (i = 0; i < NU_ARRAY_SIZE(unops); ++i)
    {
        if (t == unops[i])
        {
            return NU_TRUE;
        }
    }
    return NU_FALSE;
}
static nulang__unop_t
nulang__unop_from_token (nulang__token_type_t t)
{
    switch (t)
    {
        case TOKEN_SUB:
            return UNOP_NEG;
        case TOKEN_NOT:
            return UNOP_NOT;
        default:
            return UNOP_NEG;
    }
}
static nu_u16_t
nulang__binop_precedence (nulang__token_type_t t)
{
    switch (t)
    {
        case TOKEN_ADD:
        case TOKEN_SUB:
            return 1;
        case TOKEN_MUL:
        case TOKEN_DIV:
            return 2;
        case TOKEN_EQUAL:
        case TOKEN_NEQUAL:
        case TOKEN_LEQUAL:
        case TOKEN_GEQUAL:
        case TOKEN_LESS:
        case TOKEN_GREATER:
        case TOKEN_AND:
        case TOKEN_OR:
            return 3;
        default:
            NU_ASSERT(NU_FALSE);
            return 0;
    }
}
static nu_bool_t
nulang__binop_is_left_associative (nulang__token_type_t t)
{
    switch (t)
    {
        case TOKEN_ADD:
        case TOKEN_SUB:
        case TOKEN_MUL:
        case TOKEN_DIV:
            return NU_TRUE;
        default:
            return NU_FALSE;
    }
}

static nulang__node_t *
nulang__first_child (nulang__ast_t     *ast,
                     nulang__node_id_t  parent,
                     nulang__node_id_t *child)
{
    nulang__node_id_t n;
    NU_ASSERT(parent != NULANG_NODE_NULL);
    n = ast->nodes[parent].first_child;
    if (child)
    {
        *child = n;
    }
    if (n != NULANG_NODE_NULL)
    {
        return &ast->nodes[n];
    }
    else
    {
        return NU_NULL;
    }
}
static nulang__node_t *
nulang__sibling (nulang__ast_t     *ast,
                 nulang__node_id_t  node,
                 nulang__node_id_t *sibling)
{
    nulang__node_id_t n;
    NU_ASSERT(node != NULANG_NODE_NULL);
    n = ast->nodes[node].next_sibling;
    if (sibling)
    {
        *sibling = n;
    }
    if (n != NULANG_NODE_NULL)
    {
        return &ast->nodes[n];
    }
    else
    {
        return NU_NULL;
    }
}

static nu_bool_t
nulang__vartype_compatible (nulang__vartype_t var, nulang__vartype_t expr)
{
    if (var.primitive == expr.primitive)
    {
        if (var.primitive == NU_PRIMITIVE_ENTITY)
        {
            if (var.archetype == expr.archetype
                || expr.archetype == NU_ARCHETYPE_NULL)
            {
                return NU_TRUE;
            }
        }
        else
        {
            return NU_TRUE;
        }
    }
    return NU_FALSE;
}

static nulang__node_t *
nulang__iter_childs (nulang__ast_t     *ast,
                     nulang__node_id_t  parent,
                     nulang__node_id_t *child)
{
    NU_ASSERT(child);
    if (*child == NULANG_NODE_NULL)
    {
        return nulang__first_child(ast, parent, child);
    }
    else
    {
        return nulang__sibling(ast, *child, child);
    }
}

typedef nulang__error_t (*nulang__foreach_callback_t)(nulang__ast_t    *ast,
                                                      nulang__node_id_t id,
                                                      nulang__node_t   *node,
                                                      void             *data);

static nulang__error_t
nulang__ast_foreach_childs (nulang__ast_t             *ast,
                            nulang__node_id_t          parent,
                            nulang__foreach_callback_t callback,
                            void                      *data)
{
    nulang__error_t   error;
    nulang__node_id_t child;
    child = NULANG_NODE_NULL;
    while (nulang__iter_childs(ast, parent, &child))
    {
        nulang__node_t *node = &ast->nodes[child];
        error                = callback(ast, child, node, data);
        NULANG_ERROR_CHECK(error);
    }
    return NULANG_ERROR_NONE;
}

#endif

#endif

#ifndef NU_INTERPRETER_H
#define NU_INTERPRETER_H

#include <nucleus/vm/types.h>
#include <nucleus/vm/math.h>

#define NU_FOREACH_OP(OP) \
    OP(ADD)               \
    OP(SUB)               \
    OP(MULI)              \
    OP(MULF)              \
    OP(DIVI)              \
    OP(DIVF)
#define NU_GENERATE_OP_ENUM(OP) NU_OP_##OP,
#define NU_GENERATE_OP_NAME(OP) #OP,
typedef enum
{
    NU_FOREACH_OP(NU_GENERATE_OP_ENUM) NU_OP_UNKNOWN
} nu_opcode_t;
const nu_char_t *NU_OP_NAMES[]
    = { NU_FOREACH_OP(NU_GENERATE_OP_NAME) "UNKNOWN" };

#ifdef NU_IMPL

#define NU_STACK_SIZE 256

typedef nu_u32_t nu__word_t;
typedef nu_u16_t nu__instruction_t;

#define NU_GET_OPCODE(i) (i & 0xff)

typedef struct
{
    nu_u32_t           sp; /* stack pointer */
    nu_u32_t           lp; /* locals pointer */
    nu_u32_t           pc; /* program counter */
    nu__word_t         stack[NU_STACK_SIZE];
    nu__word_t         local[NU_STACK_SIZE];
    nu__instruction_t *instructions;
    nu__word_t        *constants;
} nu__interpreter_t;

static void
nu__interpreter_init (nu__interpreter_t *inte)
{
    nu_size_t i;
    inte->sp = 0;
    inte->lp = 0;
    inte->pc = 0;
    for (i = 0; i < NU_STACK_SIZE; ++i)
    {
        inte->stack[i] = 0;
        inte->local[i] = 0;
    }
    inte->instructions = NU_NULL;
    inte->constants    = NU_NULL;
}

static void
nu__interpreter_exec (nu__interpreter_t *inte)
{
    nu_i32_t          a, b;
    nu__instruction_t instruction = inte->instructions[inte->pc++];
    switch (NU_GET_OPCODE(instruction))
    {
        case NU_OP_ADD:
            a                       = inte->stack[--inte->sp];
            b                       = inte->stack[--inte->sp];
            inte->stack[inte->sp++] = a + b;
            break;
        case NU_OP_SUB:
            a                       = inte->stack[--inte->sp];
            b                       = inte->stack[--inte->sp];
            inte->stack[inte->sp++] = a - b;
            break;
        case NU_OP_MULI:
            a                       = inte->stack[--inte->sp];
            b                       = inte->stack[--inte->sp];
            inte->stack[inte->sp++] = a * b;
            break;
        case NU_OP_MULF:
            a                       = inte->stack[--inte->sp];
            b                       = inte->stack[--inte->sp];
            inte->stack[inte->sp++] = nu_fmul(a, b);
            break;
        case NU_OP_DIVI:
            a                       = inte->stack[--inte->sp];
            b                       = inte->stack[--inte->sp];
            inte->stack[inte->sp++] = a / b;
            break;
        case NU_OP_DIVF:
            a                       = inte->stack[--inte->sp];
            b                       = inte->stack[--inte->sp];
            inte->stack[inte->sp++] = nu_fdiv(a, b);
            break;
        default:
            break;
    }
}

#endif

#endif

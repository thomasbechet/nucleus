#ifndef NULANG_REPORT_H
#define NULANG_REPORT_H

#include <nucleus/lang/ast.h>

typedef struct
{
    nulang__span_t       span;
    nulang__token_type_t token_expect;
    nulang__token_type_t token_got;
    nulang__vartype_t    vartype_expect;
    nulang__vartype_t    vartype_got;
} nulang__error_data_t;

#endif

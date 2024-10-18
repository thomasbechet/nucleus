#ifndef NU_SERIA_IMPL_H
#define NU_SERIA_IMPL_H

#include <nucleus/internal.h>

void
nu_seria_bin_read (const nu_seria_t *seria,
                   void             *data,
                   const nu_byte_t  *bytes,
                   nu_size_t         size)
{
    nu_byte_t *p = (nu_byte_t *)data;
    for (nu_size_t i = 0; i < seria->field_count; ++i)
    {
        const nu_seria_field_t *field = seria->fields + i;
        switch (field->type)
        {
            case NU_SERIA_BOOL:
            case NU_SERIA_U32:
            case NU_SERIA_F32:
            case NU_SERIA_V3U:
            case NU_SERIA_V3F:
            case NU_SERIA_Q4:
            case NU_SERIA_STR:
            case NU_SERIA_OBJ:
                break;
        }
    }
}

#endif

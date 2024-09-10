#ifndef NU_CONTEXT_H
#define NU_CONTEXT_H

#include <nucleus/core/config.h>

NU_API nu_error_t nu_init(void);
NU_API nu_error_t nu_terminate(void);

NU_API void nu__panic(const nu_char_t *source, const nu_char_t *format, ...);
NU_API void nu__vpanic(const nu_char_t *source,
                       const nu_char_t *format,
                       va_list          args);

#define nu_panic(...)

#endif

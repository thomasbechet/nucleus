#ifndef NU_LOGGER_H
#define NU_LOGGER_H

#include <nucleus/core/config.h>
#include <nucleus/core/memory.h>

typedef enum
{
    NU_LOG_DEBUG   = 5,
    NU_LOG_INFO    = 4,
    NU_LOG_WARNING = 3,
    NU_LOG_ERROR   = 2,
} nu_log_level_t;

NU_API void nu_log(nu_log_level_t   level,
                   const nu_char_t *source,
                   const nu_char_t *format,
                   ...);
NU_API void nu_vlog(nu_log_level_t   level,
                    const nu_char_t *source,
                    const nu_char_t *format,
                    va_list          args);

#define nu_debug(...)   nu_log(NU_LOG_DEBUG, __SOURCE__, __VA_ARGS__)
#define nu_info(...)    nu_log(NU_LOG_INFO, __SOURCE__, __VA_ARGS__)
#define nu_warning(...) nu_log(NU_LOG_WARNING, __SOURCE__, __VA_ARGS__)
#define nu_error(...)   nu_log(NU_LOG_ERROR, __SOURCE__, __VA_ARGS__)

#endif

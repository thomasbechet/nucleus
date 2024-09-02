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
    NU_LOG_FATAL   = 1
} nu_log_level_t;

NU_API void nu_log(nu_log_level_t   level,
                   const nu_char_t *filename,
                   nu_size_t        fileline,
                   const nu_char_t *format,
                   ...);
NU_API void nu_vlog(nu_log_level_t   level,
                    const nu_char_t *filename,
                    nu_size_t        fileline,
                    const nu_char_t *format,
                    va_list          args);

#define __FILENAME__ \
    (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

#define NU_DEBUG(...) nu_log(NU_LOG_DEBUG, __FILENAME__, __LINE__, __VA_ARGS__)
#define NU_INFO(...)  nu_log(NU_LOG_INFO, __FILENAME__, __LINE__, __VA_ARGS__)
#define NU_WARNING(...) \
    nu_log(NU_LOG_WARNING, __FILENAME__, __LINE__, __VA_ARGS__)
#define NU_ERROR(...) nu_log(NU_LOG_ERROR, __FILENAME__, __LINE__, __VA_ARGS__)
#define NU_FATAL(...) nu_log(NU_LOG_FATAL, __FILENAME__, __LINE__, __VA_ARGS__)

#endif

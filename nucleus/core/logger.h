#ifndef NU_LOGGER_H
#define NU_LOGGER_H

#include <nucleus/core/config.h>

typedef struct
{
    nu_u32_t todo;
} nu__logger_t;

typedef enum
{
    NU_LOG_DEBUG   = 5,
    NU_LOG_INFO    = 4,
    NU_LOG_WARNING = 3,
    NU_LOG_ERROR   = 2,
    NU_LOG_FATAL   = 1
} nu_log_level_t;

typedef struct
{
    nu_log_level_t level;
} nu_logger_info_t;

typedef struct
{
    nu_log_level_t level;
} nu_logger_t;

NU_API nu_logger_info_t nu_logger_info_default(void);

NU_API nu_error_t nu_logger_init(const nu_logger_info_t *info,
                                 nu_logger_t            *logger);
NU_API nu_error_t nu_logger_free(nu_logger_t *logger);

NU_API void nu_log(nu_logger_t     *logger,
                   nu_log_level_t   level,
                   const nu_char_t *filename,
                   nu_size_t        fileline,
                   const nu_char_t *format,
                   ...);
NU_API void nu_vlog(nu_logger_t     *logger,
                    nu_log_level_t   level,
                    const nu_char_t *filename,
                    nu_size_t        fileline,
                    const nu_char_t *format,
                    va_list          args);

#define __FILENAME__ \
    (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

#define NU_DEBUG(logger, ...) \
    nu_log(logger, NU_LOG_DEBUG, __FILENAME__, __LINE__, __VA_ARGS__)
#define NU_INFO(logger, ...) \
    nu_log(logger, NU_LOG_INFO, __FILENAME__, __LINE__, __VA_ARGS__)
#define NU_WARNING(logger, ...) \
    nu_log(logger, NU_LOG_WARNING, __FILENAME__, __LINE__, __VA_ARGS__)
#define NU_ERROR(logger, ...) \
    nu_log(logger, NU_LOG_ERROR, __FILENAME__, __LINE__, __VA_ARGS__)
#define NU_FATAL(logger, ...) \
    nu_log(logger, NU_LOG_FATAL, __FILENAME__, __LINE__, __VA_ARGS__)

#endif

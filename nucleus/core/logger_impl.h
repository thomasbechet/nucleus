#ifndef NU_LOGGER_IMPL_H
#define NU_LOGGER_IMPL_H

#include <nucleus/internal.h>

void
nu_log (nu_log_level_t   level,
        const nu_char_t *source,
        const nu_char_t *format,
        ...)
{
    va_list args;
    va_start(args, format);
    nu_vlog(level, source, format, args);
    va_end(args);
}
void
nu_vlog (nu_log_level_t   level,
         const nu_char_t *source,
         const nu_char_t *format,
         va_list          args)
{
    if (_ctx.core.logger.level < level)
    {
        return;
    }

    nu_time_t t    = nu_time();
    nu_u32_t  hour = nu_time_hours(&t);
    nu_u32_t  min  = nu_time_minutes(&t);
    nu_u32_t  sec  = nu_time_seconds(&t);

    fprintf(stdout, "\x1B[90m%02d:%02d:%02d ", hour, min, sec);
    switch (level)
    {
        case NU_LOG_DEBUG:
            fprintf(stdout, "\x1B[36mDEBUG\x1B[0m ");
            break;
        case NU_LOG_INFO:
            fprintf(stdout, "\x1B[32mINFO \x1B[0m ");
            break;
        case NU_LOG_WARNING:
            fprintf(stdout, "\033[0;33mWARN \x1B[0m ");
            break;
        case NU_LOG_ERROR:
            fprintf(stdout, "\x1B[31mERROR\x1B[0m ");
            break;
    }
    fprintf(stdout, "\x1B[90m%s\x1B[0m", source);
    vfprintf(stdout, format, args);
    fprintf(stdout, "\n");
}

#endif

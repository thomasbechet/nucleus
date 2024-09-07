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

    // if (s_state.log_file)
    // {
    //     nu_file_write_printf(
    //         s_state.log_file, "[%s][%s] ", data->name, severity_name);
    //     nu_file_write_vprintf(s_state.log_file, format, args);
    //     nu_file_write_printf(s_state.log_file, "\n");
    // }

    nu_time_t t    = nu_time();
    nu_u32_t  hour = nu_time_hours(&t);
    nu_u32_t  min  = nu_time_minutes(&t);
    nu_u32_t  sec  = nu_time_seconds(&t);

#if defined(NU_PLATFORM_WINDOWS)
    switch (severity)
    {
        case nu_info:
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 3);
            break;
        case nu_warning:
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 6);
            break;
        case nu_error:
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);
            break;
        case nu_fatal:
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);
            break;
    }
    fprintf(stdout, "[%s][%s] ", data->name, severity_name);
    vfprintf(stdout, format, args);
    fprintf(stdout, "\n");
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
#elif defined(NU_PLATFORM_UNIX)
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
        case NU_LOG_FATAL:
            fprintf(stdout, "\x1B[31mFATAL\x1B[0m ");
            break;
    }
    fprintf(stdout, "\x1B[90m%s:\x1B[0m ", source);
    vfprintf(stdout, format, args);
    fprintf(stdout, "\n");
#endif
}

#endif

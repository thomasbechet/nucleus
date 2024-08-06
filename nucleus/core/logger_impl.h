#ifndef NU_LOGGER_IMPL_H
#define NU_LOGGER_IMPL_H

#include <nucleus/core/logger.h>
#include <nucleus/core/time.h>

nu_logger_info_t
nu_logger_info_default (void)
{
    nu_logger_info_t info;
    info.todo = 0;
    return info;
}

nu_error_t
nu_logger_init (const nu_logger_info_t *info, nu_logger_t *logger)
{
    NU_UNUSED(info);
    logger->level = NU_LOG_DEBUG;
    return NU_ERROR_NONE;
}
nu_error_t
nu_logger_terminate (nu_logger_t *logger)
{
    return NU_ERROR_NONE;
}

void
nu_log (nu_logger_t     *logger,
        nu_log_level_t   level,
        const nu_char_t *filename,
        nu_size_t        fileline,
        const nu_char_t *format,
        ...)
{
    va_list args;
    va_start(args, format);
    nu_vlog(logger, level, filename, fileline, format, args);
    va_end(args);
}
void
nu_vlog (nu_logger_t     *logger,
         nu_log_level_t   level,
         const nu_char_t *filename,
         nu_size_t        fileline,
         const nu_char_t *format,
         va_list          args)
{
    if (logger->level < level)
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
        case NU_INFO:
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 3);
            break;
        case NU_WARNING:
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 6);
            break;
        case NU_ERROR:
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);
            break;
        case NU_FATAL:
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
    fprintf(stdout, "\x1B[90m%s:%ld:\x1B[0m ", filename, fileline);
    vfprintf(stdout, format, args);
    fprintf(stdout, "\n");
#endif
}

#endif

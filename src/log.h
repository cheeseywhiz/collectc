#ifndef LOG_H
#define LOG_H

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
char* strerror(int errnum);

enum log_levels {
    LOG_NONE = 1 << 6,
    LOG_CRITICAL = 1 << 5,
    LOG_EXCEPTION = 1 << 4,
    LOG_ERROR = 1 << 3,
    LOG_WARNING = 1 << 2,
    LOG_INFO = 1 << 1,
    LOG_DEBUG = 1 << 0,
    LOG_ALL = 0,
};

void log_init_prog_name(char *argv[]);
void log_backtrace(void);

#define LOG_NAME_MAX 255
#define LOG_NAME_SIZE LOG_NAME_MAX + 3
extern int log_level;
extern int log_strict_exceptions;
extern int log_surpress_exceptions;
extern char prog_name[LOG_NAME_SIZE];

#ifdef _COLLECT_DEBUG
# define LOG_LEVEL_RESET() log_level = LOG_DEBUG
# define LOG_FILE_LINE() STDERR("%s (%d): ", __FILE__, __LINE__)
#else
# define LOG_LEVEL_RESET() log_level = 0
# define LOG_FILE_LINE()
#endif

#define LOG_LEVEL_ADD(option) log_level |= option
#define LOG_LEVEL_REMOVE(option) log_level &= ~option
#define __SET_LOWER(new_level, option) if (new_level <= option) LOG_LEVEL_ADD(option)

#define SET_LOG_LEVEL(level) \
    LOG_LEVEL_RESET(); \
    __SET_LOWER(level, LOG_CRITICAL); \
    __SET_LOWER(level, LOG_EXCEPTION); \
    __SET_LOWER(level, LOG_ERROR); \
    __SET_LOWER(level, LOG_WARNING); \
    __SET_LOWER(level, LOG_INFO); \
    __SET_LOWER(level, LOG_DEBUG)

#define STDERR(args...) fprintf(stderr, args)
#define PRINT_LEVEL(level) \
    STDERR("%s", prog_name); \
    if (0) { \
    } else if (level & LOG_CRITICAL) { \
        STDERR("critical: "); \
    } else if (level & LOG_EXCEPTION) { \
        STDERR("exception: "); \
    } else if (level & LOG_ERROR) { \
        STDERR("error: "); \
    } else if (level & LOG_WARNING) { \
        STDERR("warning: "); \
    } else if (level & LOG_INFO) { \
        STDERR("info: "); \
    } else if (level & LOG_DEBUG) { \
        STDERR("debug: "); \
        LOG_FILE_LINE(); \
    }

#define PRINT(level, args...) \
    PRINT_LEVEL(level); \
    STDERR(args); \
    STDERR("\n")

#define LOG_LEVEL(level, args...) if (log_level & level) { PRINT(level, args); }
#define LOG_BACKTRACE(level, args...) if (log_level & level) { log_backtrace(); PRINT(level, args); }
#define CRITICAL(args...) LOG_LEVEL(LOG_CRITICAL, args)
#define EXCEPTION(args...) \
    if (!log_surpress_exceptions) { \
        LOG_BACKTRACE(LOG_EXCEPTION, args); \
        if (log_strict_exceptions) { \
            exit(EXIT_FAILURE); \
        } \
    }
#define ERROR(args...) LOG_LEVEL(LOG_ERROR, args)
#define WARNING(args...) LOG_LEVEL(LOG_WARNING, args)
#define INFO(args...) LOG_LEVEL(LOG_INFO, args)
#define DEBUG(args...) LOG_LEVEL(LOG_DEBUG, args)
#define LOG(args...) INFO(args)

#define LOG_ERRNO() EXCEPTION("errno: %s", strerror(errno))

#endif

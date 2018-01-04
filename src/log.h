#ifndef LOG_H
#define LOG_H

#include <stdio.h>
#include <errno.h>
char* strerror(int errnum);

enum log_levels {
    LOG_NONE = 1 << 5,
    LOG_CRITICAL = 1 << 4,
    LOG_ERROR = 1 << 3,
    LOG_WARNING = 1 << 2,
    LOG_INFO = 1 << 1,
    LOG_DEBUG = 1 << 0,
    LOG_ALL = 0,
};

extern int log_level;

#ifdef _COLLECT_DEBUG
# define LOG_LEVEL_RESET() log_level = LOG_DEBUG
#else
# define LOG_LEVEL_RESET() log_level = 0
#endif

#define ADD_LEVEL(option) log_level |= option
#define REMOVE_LEVEL(option) log_level &= option
#define __SET_LOWER(new_level, option) if (new_level <= option) ADD_LEVEL(option)

#define SET_LOG_LEVEL(level) \
    LOG_LEVEL_RESET(); \
    __SET_LOWER(level, LOG_CRITICAL); \
    __SET_LOWER(level, LOG_ERROR); \
    __SET_LOWER(level, LOG_WARNING); \
    __SET_LOWER(level, LOG_INFO); \
    __SET_LOWER(level, LOG_DEBUG)

#define PRINT_LEVEL(level) \
    fprintf(stderr, "collectc: "); \
    if (0) { \
    } else if (level & LOG_CRITICAL) { \
        fprintf(stderr, "critical: "); \
    } else if (level & LOG_ERROR) { \
        fprintf(stderr, "error: "); \
    } else if (level & LOG_WARNING) { \
        fprintf(stderr, "warning: "); \
    } else if (level & LOG_INFO) { \
        fprintf(stderr, "info: "); \
    } else if (level & LOG_DEBUG) { \
        fprintf(stderr, "debug: "); \
    }

#define PRINT(level, args...) \
    PRINT_LEVEL(level); \
    fprintf(stderr, args); \
    fprintf(stderr, "\n")

#define LOG_LEVEL(level, args...) if (log_level & level) { PRINT(level, args); }
#define CRITICAL(args...) LOG_LEVEL(LOG_CRITICAL, args)
#define ERROR(args...) LOG_LEVEL(LOG_ERROR, args)
#define WARNING(args...) LOG_LEVEL(LOG_WARNING, args)
#define INFO(args...) LOG_LEVEL(LOG_INFO, args)
#define DEBUG(args...) LOG_LEVEL(LOG_DEBUG, args)
#define LOG(args...) INFO(args)

#define LOG_ERRNO() ERROR("%s", strerror(errno))

#endif

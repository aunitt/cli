
#if !defined(__DEBUG_H__)

#define __DEBUG_H__

typedef enum {
    SEVERITY_DEBUG,
    SEVERITY_INFO,
    SEVERITY_WARN,
    SEVERITY_ERROR,
    SEVERITY_FATAL,
}   Severity;

void log_print(Severity s, const char *fmt, ...) __attribute__((format(printf,2, 3)));
void log_die();

#define ALOG_LEVEL(level, fmt, ...) \
    log_print(level, "%s +%d %s() : " fmt, __FILE__, __LINE__, __FUNCTION__, ## __VA_ARGS__ )

#define ALOG_DEBUG(fmt, ...)    ALOG_LEVEL(SEVERITY_DEBUG, fmt, ## __VA_ARGS__ )
#define ALOG_ERROR(fmt, ...)    ALOG_LEVEL(SEVERITY_ERROR, fmt, ## __VA_ARGS__ )

#define ASSERT(x)               if (!(x)) { ALOG_ERROR(""); log_die(); }

#endif // __DEBUG_H__

//  FIN


#if !defined(__DEBUG_H__)

#define __DEBUG_H__

#define UNUSED(x) ((x) = (x))

void log_open();
void log_close();
void log_print(const char *fmt, ...) __attribute__((format(printf,1,2)));
void log_die();

#if !defined(LOG_DEBUG)

#define LOG_LEVEL(level, fmt, ...) \
    log_print("%s %s +%d %s() : " fmt, level, __FILE__, __LINE__, __FUNCTION__, ## __VA_ARGS__ )

#define LOG_DEBUG(fmt, ...)    LOG_LEVEL("DEBUG", fmt, ## __VA_ARGS__ )
#define LOG_ERROR(fmt, ...)    LOG_LEVEL("ERROR", fmt, ## __VA_ARGS__ )

#define ASSERT(x)               if (!(x)) { LOG_ERROR(""); log_die(); }

#endif // LOG_DEBUG

#endif // __DEBUG_H__

//  FIN

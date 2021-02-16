
#if !defined(__DEBUG_H__)

#define __DEBUG_H__

#include <stdio.h>
#include <stdlib.h> // exit()

#define LOG_DEBUG(fmt, ...) fprintf(stderr, "DEBUG " fmt "\n", __VA_ARGS__ )
#define LOG_ERROR(fmt, ...) fprintf(stderr, "ERROR " fmt "\n", __VA_ARGS__ )

#define ASSERT(x) if (!(x)) { \
        LOG_ERROR("%d", 1); \
        exit(0); \
    }

#endif // __DEBUG_H__

//  FIN

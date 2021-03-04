
#include <pthread.h>

#include "debug.h"

#include "mutex.h"

class LinuxMutex : public Mutex
{
    pthread_mutex_t mutex;
public:
    ~LinuxMutex();

    LinuxMutex();

    virtual void lock();
    virtual void unlock();
};

Mutex* Mutex::create()
{
    return new LinuxMutex();
}

LinuxMutex::LinuxMutex()
{
    const int err = pthread_mutex_init(& mutex, 0);
    ASSERT(err == 0);
}

LinuxMutex::~LinuxMutex()
{
    const int err = pthread_mutex_destroy(& mutex);
    ASSERT(err == 0);
}

void LinuxMutex::lock()
{
    const int err = pthread_mutex_lock(& mutex);
    ASSERT(err == 0);
}

void LinuxMutex::unlock()
{
    const int err = pthread_mutex_unlock(& mutex);
    ASSERT(err == 0);
}

//  FIN

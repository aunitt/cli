
#if !defined(__MUTEX_H__)
#define __MUTEX_H__

#if defined(__cplusplus)

class Mutex 
{
public:
    virtual ~Mutex(){}

    virtual void lock() = 0;
    virtual void unlock() = 0;

    static Mutex *create();
    static Mutex *create_critical_section();
};

    /*
     *
     */

class Lock
{
    Mutex *mutex;
public:
    Lock(Mutex *m) : mutex(m)
    {
        if (mutex)
        {
            mutex->lock();
        }
    }

    ~Lock()
    {
        if (mutex)
        {
            mutex->unlock();
        }
    }
};

#else  // __cplusplus

struct Mutex;

struct Mutex *mutex_create();

#endif // __cplusplus

#endif  //  __MUTEX_H__

//  FIN

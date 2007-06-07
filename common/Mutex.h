#pragma once

// Include platform-dependent thread libraries 
// and define mutex variable type.

#ifdef IT_USE_PTHREADS
#include <pthread.h>
typedef pthread_mutex_t MutexType;
#endif

#ifdef IT_USE_WIN32_THREADS
#include <windows.h>
#include <winbase.h>
typedef CRITICAL_SECTION MutexType;
#endif

/**
 * \class Mutex
 * \brief Implements a mutex lock.
 * Basic platform-dependent mutex class.  Recommended use
 * is to lock a Mutex using the constructor of a
 * MutexLocker class created on the stack.
 * Implementation borrowed heavily from itk::SimpleFastMutexLock.
 */
class Mutex
{
public:
    Mutex();
    virtual ~Mutex();
    void Lock() const;
    void Unlock() const;
protected:
    mutable MutexType lock;
private:
    // Not implemented on purpose.  We don't want any copies
    // of mutex created willy-nilly.
    Mutex(const Mutex& other);
    void operator=(const Mutex& other);
};

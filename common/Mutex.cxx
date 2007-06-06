#include "Mutex.h"

Mutex::Mutex()
{
#ifdef IT_USE_PTHREADS
    pthread_mutex_init(&this->lock, NULL);
#endif
    
#ifdef IT_USE_WIN32_THREADS
    InitializeCriticalSection(&this->lock);
#endif
}

Mutex::~Mutex()
{
#ifdef IT_USE_PTHREADS
    pthread_mutex_destroy(&this->lock);
#endif
    
#ifdef IT_USE_WIN32_THREADS
    DeleteCriticalSection(&this->lock);
#endif
}

void Mutex::Lock() const
{
#ifdef IT_USE_PTHREADS
    pthread_mutex_lock(&this->lock);
#endif
    
#ifdef IT_USE_WIN32_THREADS
    EnterCriticalSection(&this->lock);
#endif
}

void Mutex::Unlock() const
{
#ifdef IT_USE_PTHREADS
    pthread_mutex_unlock(&this->lock);
#endif
    
#ifdef IT_USE_WIN32_THREADS
    LeaveCriticalSection(&this->lock);
#endif
}

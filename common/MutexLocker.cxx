#include "MutexLocker.h"

MutexLocker::MutexLocker(const Mutex& mutex)
{
    // Lock this locker's mutex
    this->mutex = mutex;
    this->mutex.Lock();
}

MutexLocker::~MutexLocker()
{
    // Unlock this locker's mutex
    this->mutex.Unlock();
}

#include "MutexLocker.h"

MutexLocker::MutexLocker(const Mutex& mutex)
{
    // Keep a reference to the supplied mutex and 
    // lock this locker's mutex
    this->mutex = const_cast<Mutex*>(&mutex);
    this->mutex->Lock();
}

MutexLocker::~MutexLocker()
{
    // Unlock this locker's mutex
    this->mutex->Unlock();
}

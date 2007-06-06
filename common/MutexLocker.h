#pragma once

#include "Mutex.h"

/**
 * \class MutexLocker
 * \brief Controls a Mutex through an unlock-on-delete mechanism.
 * A MutexLocker obtains a lock on a Mutex on instantiation.  It
 * releases this lock on deletion.  In this way, a MutexLocker
 * created on the stack is guaranteed to release the Mutex lock
 * when it goes out of scope.
 */
class MutexLocker
{
public:
    MutexLocker(const Mutex& mutex);
    virtual ~MutexLocker();
protected:
    Mutex mutex;
private:
};

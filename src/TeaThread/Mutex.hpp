
#ifndef TEATHREAD__MUTEX_HPP
#define TEATHREAD__MUTEX_HPP

#include <pthread.h>

namespace TeaThread {

class Mutex
{
private:
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
public:
    int lock()
    {
        return pthread_mutex_lock(&(this->mutex));
    }

    int unlock()
    {
        return pthread_mutex_unlock(&(this->mutex));
    }
};

} /* namespace TeaThread */

#endif



#include "thread.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <pthread.h>
#endif


namespace tools {


#ifdef _WIN32
#error "Not implemented yet."

Thread::Thread()
    : delegate_(NULL)
    , thread_(NULL)
    , Args()
{
}

Thread::~Thread()
{
}

void Thread::Start(void * arg)
{
}
#else

void * Thread::thread_proc(void * arg)
{
    Args * a = (Args*)arg;
    Thread * thread = a->pThis;
    void * ag = a->arg;
    thread->delegate_->Run(ag);
    return NULL;
}

Thread::Thread(Delegate* delegate)
    : delegate_(delegate)
    , thread_(NULL)
    , args_()
{
}

Thread::~Thread()
{
}

void Thread::Start(void * arg)
{
    args_.pThis = this;
    args_.arg = arg;
    pthread_create((pthread_t*)thread_, NULL, thread_proc, (void*)&args_);
}

#endif

}



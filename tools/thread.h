

#pragma once

#include <stdlib.h>

namespace tools {

class Thread {
public:
    class Delegate {
    public:
        Delegate() {}
        virtual ~Delegate() {}
        virtual void Run(void * arg) = 0;
    };

    class Args {
    public:
        Args() : pThis(NULL), arg(NULL) {}
        Thread*     pThis;
        void *      arg;
    };

    Thread(Delegate* delegate);
    virtual ~Thread();

    void Start(void * arg);

private:
    Delegate * delegate_;
    void *  thread_;
    Args    args_;

    static void * thread_proc(void * arg);
};

}



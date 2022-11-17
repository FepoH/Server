#pragma once

#include <pthread.h>
#include <semaphore.h>
#include "noncopyable.h"

namespace fepoh{

class Sem : Noncopyable{
    public:
        Sem(int value = 0,int pshared = 0);
        ~Sem();

        void post();
        void wait();
        //查看当前信号量
        int getValue();

    private:
        sem_t  m_sem;
};

}//namespace
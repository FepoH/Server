#pragma

#include <pthread.h>
#include <semaphore.h>
#include <boost/noncopyable.hpp>

namespace fepoh{

class Sem : public boost::noncopyable{
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
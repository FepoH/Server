#include "fiber.h"
#include "log/log.h"
#include "thread/thread.h"

#include <vector>

using namespace fepoh;

static Logger::ptr s_log_system = FEPOH_LOG_NAME("system");

void test(){
    //FEPOH_LOG_DEBUG(s_log_system) <<"test fiber,id = " << Fiber::GetThis()->getId();
}

void test_fiber(){
    Fiber::GetThis();

    std::vector<Fiber::ptr> fibers;
    for(int i=0;i<10;++i){
        fibers.push_back(Fiber::ptr(new Fiber(test)));
    }
    for(int i=0;i<10;++i){
        fibers[i]->swapIn();
    }
}

void test_thread(){
    std::vector<Thread::ptr> thrs;
    for(int i=0;i<3;++i){
        thrs.push_back(Thread::ptr(new Thread(test_fiber,"fepoh" + std::to_string(i))));
    }

}

int main(){
    FEPOH_LOG_DEBUG(s_log_system) << "main start";

    test_thread();
    FEPOH_LOG_DEBUG(s_log_system) <<"main end";
    return 0;
}
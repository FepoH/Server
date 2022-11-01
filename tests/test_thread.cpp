#include "thread/thread.h"
#include "log/log.h"

#include <iostream>
#include <unistd.h>
#include <vector>

using namespace fepoh;


static Logger::ptr s_log_system = FEPOH_LOG_NAME("root");

void test(){
    while(true)
        FEPOH_LOG_FATAL(s_log_system)<<"test 1";
}

void test_thread(){
    fepoh::Logger::ptr s_log_system = FEPOH_LOG_NAME("root");

    std::cout << "test_thread start" <<std::endl;

    std::vector<Thread::ptr> thrs;
    for(int i=0;i<7;++i){
        thrs.push_back(Thread::ptr(new Thread(test,"fepoh_"+std::to_string(i))));
    }
    // for(int i=0;i<10;++i){
    //     thrs[i]->join();
    // }

    std::cout << "test_thread end" <<std::endl;
    
}


int main(){
    test_thread();
    std::cout << "bbb" <<std::endl;
    return 0;
}
#include "schedule_manager.h"
#include "log/log.h"
using namespace fepoh;

static Logger::ptr s_log_system = FEPOH_LOG_NAME("system");

void test_func(){
    FEPOH_LOG_DEBUG(s_log_system) <<" test**********";
}

void test(){
    FEPOH_LOG_DEBUG(s_log_system) <<"main start";
    ScheduleManager::ptr sche(new ScheduleManager("",5,true));
    for(int i=0;i<1;++i){
        sche->schedule(test_func);
    }

    sche->start();
    sche->stop();
    
}

int main(){
    test();
    FEPOH_LOG_DEBUG(s_log_system) <<"main end";
}
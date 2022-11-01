#include "timer.h"
#include "log/log.h"

#include <unistd.h>
using namespace fepoh;

static Logger::ptr s_log_system = FEPOH_LOG_NAME("system");

void test_func1(){
    FEPOH_LOG_DEBUG(s_log_system) << "test_func start1";
    // sleep(1);
    // FEPOH_LOG_DEBUG(s_log_system) << "test_func end";
}
void test_func2(){
    FEPOH_LOG_DEBUG(s_log_system) << "test_func start2";
    // sleep(1);
    // FEPOH_LOG_DEBUG(s_log_system) << "test_func end";
}

void test_timer(){
    int count = 3;
    std::list<std::function<void()>> cbs;
    TimerManager::ptr timeMgr(new TimerManager());
    Timer::ptr time(new Timer(test_func2,3000,timeMgr.get(),true));
    Timer::ptr time1(new Timer(test_func1,2000,timeMgr.get(),true));
    timeMgr->addTimer(time);
    timeMgr->addTimer(time1);
    while(1){
        cbs = timeMgr->listAllExpired();
        for(auto item:cbs){
            item();
        }
        if(count == 5){
            time->setRecurring(false);
        }
        //FEPOH_LOG_DEBUG(s_log_system) << fepoh::GetCurTimeMs();
        // sleep(1);
        // ++count;
    }
}

int main(){
    test_timer();
    
}
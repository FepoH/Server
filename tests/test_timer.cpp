/*
 * @Author: fepo_h
 * @Date: 2022-11-20 15:21:33
 * @LastEditors: fepo_h
 * @LastEditTime: 2022-11-20 15:35:43
 * @FilePath: /fepoh/workspace/fepoh_server/tests/test_timer.cpp
 * @Description: 
 * 
 * Copyright (c) 2022 by FepoH Fepo_H@163.com, All Rights Reserved. 
 * @version: V1.0.0
 * @Mailbox: Fepo_H@163.com
 * @Descripttion: 
 */
#include "timer.h"
#include "log/log.h"
#include "io_manager.h"

#include <unistd.h>
using namespace fepoh;

static Logger::ptr s_log_system = FEPOH_LOG_NAME("system");

static int count = 0;
static std::shared_ptr<int> cond(new int(10));

void test_func1(){
    FEPOH_LOG_DEBUG(s_log_system) << "test_func start1";
    if(count >5){
        
        cond.reset();
    }
    ++count;
    // sleep(1);
    // FEPOH_LOG_DEBUG(s_log_system) << "test_func end";
}
void test_func2(){
    FEPOH_LOG_DEBUG(s_log_system) << "test_func start2";
}


void test(){
    IOManager::GetThis()->addTimer(1000,[](){
        FEPOH_LOG_DEBUG(s_log_system) << "timer";
    },true);
}

void test_cond_timer(){

}

int main(){
    IOManager iom("",3);
    std::weak_ptr<int> wp(cond);
    iom.addConditionTimer(1000,test_func1,wp,true);
    iom.start();
    
}
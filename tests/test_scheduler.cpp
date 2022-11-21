/*
 * @Author: fepo_h
 * @Date: 2022-11-20 14:15:26
 * @LastEditors: fepo_h
 * @LastEditTime: 2022-11-20 15:27:25
 * @FilePath: /fepoh/workspace/fepoh_server/tests/test_scheduler.cpp
 * @Description: 
 * 
 * Copyright (c) 2022 by FepoH Fepo_H@163.com, All Rights Reserved. 
 * @version: V1.0.0
 * @Mailbox: Fepo_H@163.com
 * @Descripttion: 
 */
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
    for(int i=0;i<100;++i){
        sche->schedule(test_func);
    }

    sche->start();
    sche->stop();
    
}

int main(){
    test();
    FEPOH_LOG_DEBUG(s_log_system) <<"main end";
}
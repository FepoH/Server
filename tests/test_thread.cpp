/*
 * @Author: fepo_h
 * @Date: 2022-11-19 23:37:34
 * @LastEditors: fepo_h
 * @LastEditTime: 2022-11-20 00:00:15
 * @FilePath: /fepoh/workspace/fepoh_server/tests/test_thread.cpp
 * @Description: 
 * 
 * Copyright (c) 2022 by FepoH Fepo_H@163.com, All Rights Reserved. 
 * @version: V1.0.0
 * @Mailbox: Fepo_H@163.com
 * @Descripttion: 
 */
#include "thread/thread.h"
#include "log/log.h"

#include <iostream>
#include <unistd.h>
#include <vector>
#include <atomic>

using namespace fepoh;


static Logger::ptr s_log_system = FEPOH_LOG_NAME("root");

void test(){
    int count =0;
    while(true){
        //测试日志多线程
        FEPOH_LOG_FATAL(s_log_system)<<"test 1";
        if(count > 10000){
            break;
        }
        ++count;
    }
}

void test_thread(){
    fepoh::Logger::ptr s_log_system = FEPOH_LOG_NAME("root");

    std::cout << "test_thread start" <<std::endl;

    std::vector<Thread::ptr> thrs;
    for(int i=0;i<7;++i){
        thrs.push_back(Thread::ptr(new Thread(test,"fepoh_"+std::to_string(i))));
    }
    for(int i=0;i<7;++i){
        thrs[i]->join();
    }

    std::cout << "test_thread end" <<std::endl;
    
}


int main(){
    test_thread();
    std::cout << "bbb" <<std::endl;
    return 0;
}
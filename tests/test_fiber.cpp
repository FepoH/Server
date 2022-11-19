/*
 * @Author: fepo_h
 * @Date: 2022-11-20 03:16:25
 * @LastEditors: fepo_h
 * @LastEditTime: 2022-11-20 03:17:49
 * @FilePath: /fepoh/workspace/fepoh_server/tests/test_fiber.cpp
 * @Description: 
 * 
 * Copyright (c) 2022 by FepoH Fepo_H@163.com, All Rights Reserved. 
 * @version: V1.0.0
 * @Mailbox: Fepo_H@163.com
 * @Descripttion: 
 */
#include "fiber.h"
#include "log/log.h"
#include "thread/thread.h"

#include <vector>

using namespace fepoh;

static Logger::ptr s_log_system = FEPOH_LOG_NAME("system");

//本文件失效,通过IOManager测试Fiber,不单独测试Fiber

// void test(){
//     //FEPOH_LOG_DEBUG(s_log_system) <<"test fiber,id = " << Fiber::GetThis()->getId();
// }

// void test_fiber(){
//     Fiber::GetThis();

//     std::vector<Fiber::ptr> fibers;
//     for(int i=0;i<10;++i){
//         fibers.push_back(Fiber::ptr(new Fiber(test)));
//     }
//     for(int i=0;i<10;++i){
//         fibers[i]->swapIn();
//     }
// }

// void test_thread(){
//     std::vector<Thread::ptr> thrs;
//     for(int i=0;i<3;++i){
//         thrs.push_back(Thread::ptr(new Thread(test_fiber,"fepoh" + std::to_string(i))));
//     }

// }

// int main(){
//     FEPOH_LOG_DEBUG(s_log_system) << "main start";

//     test_thread();
//     FEPOH_LOG_DEBUG(s_log_system) <<"main end";
//     return 0;
// }
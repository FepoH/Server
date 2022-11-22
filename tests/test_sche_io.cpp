/*
 * @Author: fepo_h
 * @Date: 2022-11-22 14:43:09
 * @LastEditors: fepo_h
 * @LastEditTime: 2022-11-22 14:43:09
 * @FilePath: /fepoh/workspace/fepoh_server/tests/test_sche_io.cpp
 * @Description: 
 * 
 * Copyright (c) 2022 by FepoH Fepo_H@163.com, All Rights Reserved. 
 * @version: V1.0.0
 * @Mailbox: Fepo_H@163.com
 * @Descripttion: 
 */
#include "schedule.h"
#include "io_manager.h"


void test(){
    IOManager::GetThis()->addTimer()
}

int main(){
    IOManager iom;
    iom.schedule(test);
    iom.start();


}
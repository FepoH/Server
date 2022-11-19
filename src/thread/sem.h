/*
 * @Author: fepo_h
 * @Date: 2022-11-19 20:17:29
 * @LastEditors: fepo_h
 * @LastEditTime: 2022-11-19 20:17:30
 * @FilePath: /fepoh/workspace/fepoh_server/src/thread/sem.h
 * @Description: 
 * 
 * Copyright (c) 2022 by FepoH Fepo_H@163.com, All Rights Reserved. 
 * @version: V1.0.0
 * @Mailbox: Fepo_H@163.com
 * @Descripttion: 
 */
#pragma once

#include <pthread.h>
#include <semaphore.h>
#include "noncopyable.h"

namespace fepoh{

class Sem : Noncopyable{
public:
    Sem(int value = 0,int pshared = 0);
    ~Sem();

    void post();
    void wait();
    //查看当前信号量的值
    int getValue();

private:
    sem_t  m_sem;
};

}//namespace
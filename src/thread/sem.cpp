/*
 * @Author: fepo_h
 * @Date: 2022-11-19 20:17:32
 * @LastEditors: fepo_h
 * @LastEditTime: 2022-11-19 20:17:35
 * @FilePath: /fepoh/workspace/fepoh_server/src/thread/sem.cpp
 * @Description: 
 * 
 * Copyright (c) 2022 by FepoH Fepo_H@163.com, All Rights Reserved. 
 * @version: V1.0.0
 * @Mailbox: Fepo_H@163.com
 * @Descripttion: 
 */
#include "sem.h"

namespace fepoh{

Sem::Sem(int value,int pshared ){
    sem_init(&m_sem,value,pshared);
}
Sem::~Sem(){
    sem_destroy(&m_sem);
}

void Sem::post(){
    sem_post(&m_sem);
}
void Sem::wait(){
    sem_wait(&m_sem);
}
int Sem::getValue(){
    int value = 0;
    sem_getvalue(&m_sem,&value);
    return value;
}



}//namespace
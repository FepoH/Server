/*
 * @Author: fepo_h
 * @Date: 2022-11-10 09:09:28
 * @LastEditors: fepo_h
 * @LastEditTime: 2022-11-19 19:51:55
 * @FilePath: /fepoh/workspace/fepoh_server/src/thread/mutex.cpp
 * @Description: 
 * 
 * Copyright (c) 2022 by FepoH Fepo_H@163.com, All Rights Reserved. 
 * @version: V1.0.0
 * @Mailbox: Fepo_H@163.com
 * @Descripttion: 
 */
#include "mutex.h"

namespace fepoh{
    
Mutex::Mutex(){
    pthread_mutex_init(&m_mutex,NULL);
}

Mutex::~Mutex(){
    pthread_mutex_destroy(&m_mutex);
}

void Mutex::lock(){
    pthread_mutex_lock(&m_mutex);
}

void Mutex::unlock(){
    pthread_mutex_unlock(&m_mutex);
}

MutexLock::MutexLock(Mutex& mutex):m_mutex(mutex){
    lock();
}
MutexLock::~MutexLock(){
    unlock();
}
void MutexLock::lock(){
    if(!m_isLock){
        m_mutex.lock();
        m_isLock = true;
    }
}
void MutexLock::unlock(){
    if(m_isLock){
        m_mutex.unlock();
        m_isLock = false;
    }
}

ReadLock::ReadLock(RWMutex& mutex):m_mutex(mutex){
    lock();
}
ReadLock::~ReadLock(){
    unlock();
}
void ReadLock::lock(){
    if(!m_isLock){
        m_mutex.readLock();
        m_isLock = true;
    }
}
void ReadLock::unlock(){
    if(m_isLock){
        m_mutex.unlock();
        m_isLock = false;
    }
}

WriteLock::WriteLock(RWMutex& mutex):m_mutex(mutex){
    lock();
}
WriteLock::~WriteLock(){
    unlock();
}
void WriteLock::lock(){
    if(!m_isLock){
        m_mutex.writeLock();
        m_isLock = true;
    }
}
void WriteLock::unlock(){
    if(m_isLock){
        m_mutex.unlock();
        m_isLock = false;
    }
}

RWMutex::RWMutex(){
    pthread_rwlock_init(&m_mutex,NULL);
}
RWMutex::~RWMutex(){
    pthread_rwlock_destroy(&m_mutex);
}
void RWMutex::readLock(){
    pthread_rwlock_rdlock(&m_mutex);
}
void RWMutex::writeLock(){
    pthread_rwlock_wrlock(&m_mutex);
}
void RWMutex::unlock(){
    pthread_rwlock_unlock(&m_mutex);
}

}//namespace
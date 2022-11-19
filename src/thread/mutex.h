/*
 * @Author: fepo_h
 * @Date: 2022-11-10 09:11:23
 * @LastEditors: fepo_h
 * @LastEditTime: 2022-11-19 19:50:41
 * @FilePath: /fepoh/workspace/fepoh_server/src/thread/mutex.h
 * @Description: 锁封装,锁哨兵实现自动加锁解锁
 * 
 * Copyright (c) 2022 by FepoH Fepo_H@163.com, All Rights Reserved. 
 * @version: V1.0.0
 * @Mailbox: Fepo_H@163.com
 * @Descripttion: 
 */
#pragma once

#include <pthread.h>
#include <mutex>
#include <memory>
#include <thread>
#include <pthread.h>
#include "noncopyable.h"

namespace fepoh{
//互斥锁
class Mutex {
    public:
        Mutex();
        ~Mutex();
        void lock();
        void unlock();
    private:
        pthread_mutex_t m_mutex;
};
//互斥锁适配
//通过Lock对锁进行操作,有效防止发生未释放锁和连续锁几次的问题
class MutexLock{
    public:
        MutexLock(Mutex& mutex);
        ~MutexLock();
        void lock();
        void unlock();
    private:
        Mutex& m_mutex;
        bool m_isLock = false;
};

//读写锁
class RWMutex : Noncopyable{
    public:
        RWMutex();
        ~RWMutex();
        void readLock();
        void writeLock();
        void unlock();
    private:
        pthread_rwlock_t m_mutex;
};

//读锁适配
class ReadLock {
    public:
        ReadLock(RWMutex& mutex);
        ~ReadLock();

        void lock();
        void unlock();
    private:
        bool m_isLock = false;
        RWMutex& m_mutex;
};
//写锁适配
class WriteLock{
    public:
        WriteLock(RWMutex& m_mutex);
        ~WriteLock();

        void lock();
        void unlock();
    private:
        bool m_isLock = false;
        RWMutex& m_mutex;
};


}
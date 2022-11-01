#pragma once

#include <pthread.h>
#include <mutex>
#include <memory>
#include <thread>
#include <pthread.h>
#include <boost/noncopyable.hpp>

namespace fepoh{
//互斥锁
class Mutex{
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
        MutexLock(Mutex& mutex):m_mutex(mutex){
            lock();
        }
        ~MutexLock(){
            unlock();
        }
        void lock(){
            if(!m_isLock){
                m_mutex.lock();
                m_isLock = true;
            }
        }
        void unlock(){
            if(m_isLock){
                m_mutex.unlock();
                m_isLock = false;
            }
        }
    private:
        Mutex& m_mutex;
        bool m_isLock = false;
};

//读写锁
class RWMutex : public boost::noncopyable{
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
class ReadLock{
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
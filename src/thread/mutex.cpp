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
/*
 * @Author: fepo_h
 * @Date: 2022-11-19 20:19:21
 * @LastEditors: fepo_h
 * @LastEditTime: 2022-11-20 02:01:08
 * @FilePath: /fepoh/workspace/fepoh_server/src/thread/thread.cpp
 * @Description: 
 * 
 * Copyright (c) 2022 by FepoH Fepo_H@163.com, All Rights Reserved. 
 * @version: V1.0.0
 * @Mailbox: Fepo_H@163.com
 * @Descripttion: 
 */
#include "thread.h"
#include "macro.h"
#include "util.h"

#include <errno.h>
#include <string.h>
#include <atomic>

namespace fepoh{
/**
 * @description: 当前线程
 * @return {*}
 */
static thread_local Thread* t_thread = nullptr;
/**
 * @description: 当前线程名
 * @return {*}
 */
static thread_local std::string t_thread_name = "UNKOWN";

static fepoh::Logger::ptr s_log_system = FEPOH_LOG_NAME("system");


Thread::Thread(std::function<void()>cb, const std::string& name):m_cb(cb){
    static std::atomic<uint32_t> count = {0};
    if(name.empty()){
        m_name = "thr_" + count;
    }else{
        m_name = name;
    }
    int rt = pthread_create(&m_thread,nullptr,&Thread::Run,this);
    if(rt){
        FEPOH_ASSERT1(false,"Thread::Thread error.pthread_create error,rt " + std::to_string(rt));
    }
    m_sem.wait();
}

Thread::~Thread(){
    if(m_thread){
        detach();
    }
}

void Thread::join(){
    if(m_thread){
        FEPOH_LOG_INFO(s_log_system) << "Thread join.thread name = " << m_name 
                                     << ",thread id = " << m_id;
        int rt = pthread_join(m_thread,nullptr);
        if(rt){
            FEPOH_ASSERT1(false,"Thread::Thread error.pthread_join error,rt = " + std::to_string(rt) + "." + strerror(rt));
        }
        m_thread = 0;
    }
}

void Thread::detach(){
    if(m_thread){
        FEPOH_LOG_INFO(s_log_system) << "Thread detach.thread name = " << m_name 
                                     << ",thread id = " << m_id;
        int rt = pthread_detach(m_thread);
        if(rt){
            FEPOH_ASSERT1(false,"Thread::Thread error.pthread_detach error,rt = " + std::to_string(rt) + ".");
        }
    }
}

Thread* Thread::GetThis(){
    return t_thread;
}

const std::string& Thread::GetName(){
    if(t_thread){
        t_thread_name = t_thread ->m_name;
    }
    return t_thread_name;
}
void Thread::SetName(const std::string& name){
    if(!name.empty()){
        if(t_thread){
            t_thread->m_name = name;
        }
        t_thread_name = name;
    }
}

void* Thread::Run(void* arg){
    //正式进入线程
    Thread* cur = (Thread*)arg;
    t_thread = cur;
    SetName(cur->m_name);
    cur->m_id = fepoh::GetThreadId();
    std::function<void()> cb;
    cb.swap(cur->m_cb);
    cur->m_sem.post();
    FEPOH_LOG_INFO(s_log_system) << "Thread run.thread name = " << cur->m_name
                                    << ",thread id = " << cur->m_id;
    try{
        cb();
    }catch(...){
        FEPOH_LOG_ERROR(s_log_system) << "Thread::Run error." << "thread name:" 
            << cur->m_name << "thread id = " << cur->m_id;
    }
    return 0;
}

}//namespace
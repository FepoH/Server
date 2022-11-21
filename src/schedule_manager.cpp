/*
 * @Author: fepo_h
 * @Date: 2022-11-20 02:49:31
 * @LastEditors: fepo_h
 * @LastEditTime: 2022-11-20 14:10:35
 * @FilePath: /fepoh/workspace/fepoh_server/src/schedule_manager.cpp
 * @Description: 
 * 
 * Copyright (c) 2022 by FepoH Fepo_H@163.com, All Rights Reserved. 
 * @version: V1.0.0
 * @Mailbox: Fepo_H@163.com
 * @Descripttion: 
 */
#include "schedule_manager.h"
#include "log/log.h"
#include "macro.h"
#include "hook.h"
#include "config.h"

namespace fepoh{
/**
 * @description: 管理协程
 */
static thread_local Fiber::ptr t_root_fiber = nullptr;       //用于切换的调度器

//是否已经被use_caller的线程
// bool ScheduleManager::has_use_caller = false;

/**
 * @description: 当前线程的协程管理器
 */
static thread_local ScheduleManager* t_scheduler = nullptr; 

static Logger::ptr s_log_system = FEPOH_LOG_NAME("system");

static ConfigVar<uint32_t>::ptr g_fiber_root_stacksize = 
        Config::Lookup<uint32_t>( 128 *1024,"system.fiber.root.stacksize","fiber root stack size");


ScheduleManager::ScheduleManager(const std::string& name,uint32_t thrCount,bool use_caller)
        :m_name(name),m_useCaller(use_caller){
    m_threadCount = thrCount > 0 ? thrCount:1;
    t_scheduler = this;
    //if((!has_use_caller)&&use_caller){
    if(use_caller){
        Thread::SetName(m_name);
        //has_use_caller = true;
        --m_threadCount;
        m_threadIds.push_back(fepoh::GetThreadId());
        //该线程纳入调度器
        Fiber::GetThis();
        t_root_fiber.reset(new Fiber(std::bind(&ScheduleManager::run,this),g_fiber_root_stacksize->getValue(),true));
    }else{
        t_root_fiber = Fiber::GetThis();
    }  
}

ScheduleManager::~ScheduleManager(){
    if(!m_isStop){
        stop();
    }
    FEPOH_ASSERT(m_isStop);
    if(GetThis() == this){
        t_scheduler = nullptr;
    }
}
//启动调度器
void ScheduleManager::start(){
    FEPOH_LOG_DEBUG(s_log_system)<< "ScheduleManager::start";
    MutexLock lock(m_mutex);
    //防止启动后再次启动,只能启动-停止-启动...
    FEPOH_ASSERT(m_isStop);
    FEPOH_ASSERT(m_threads.empty());
    m_isStop = false;
    for(int i=0;i<m_threadCount;++i){
        m_threads.push_back(Thread::ptr(new Thread(std::bind(&ScheduleManager::run,this)
                            ,(m_name.empty() ? "thr_" : m_name + "thr_") + std::to_string(i))));
        m_threadIds.push_back(m_threads[i]->getId());
    }
    // if(m_useCaller){
    //     t_root_fiber->call();
    // }
}

void ScheduleManager::stop(){
    FEPOH_LOG_DEBUG(s_log_system)<< "ScheduleManager::stop";
    m_isStop=true;
    //有多少个线程,notice多少次
    for(size_t i=0;i<m_threadCount;++i){
        notice();
    }
    if(t_root_fiber != Fiber::GetMainFiber()){
        notice();
    }
    if(t_root_fiber != Fiber::GetMainFiber()){
        if(!isStop()){
            //root_fiber-->main_fiber
            t_root_fiber->call();
        }
    }
    //减少智能指针引用计数
    std::vector<Thread::ptr> thrs;
    {
        MutexLock lock(m_mutex);
        thrs.swap(m_threads);
    }
    for(auto& i:thrs){
        i->join();
    }
}

void ScheduleManager::SetThis(ScheduleManager* val){
    t_scheduler = val;
}

void ScheduleManager::run(){
    //多线程操作
    set_hook_enable(true);
    SetThis(this);
    FEPOH_LOG_INFO(s_log_system)<< "ScheduleManager::run";
    if(t_root_fiber == nullptr){
        //创建main_fiber
        t_root_fiber = Fiber::GetThis();
    }
    Fiber::ptr idle_fiber(new Fiber(std::bind(&ScheduleManager::idle,this)));
    Fiber::ptr cb_fiber;
    Task fc;
    while(true){
        fc.reset();
        bool is_active=false;
        {
            MutexLock locker(m_mutex);
            auto it=m_tasks.begin();
            while(it!=m_tasks.end()){
                if(it->getTask()->getState()==Fiber::EXEC){
                    ++it;
                    continue;
                }
                fc=*it;
                m_tasks.erase(it);
                //任务加1
                ++m_activeThreadCount;
                //有任务说明此线程是活跃的,主要是epoll
                is_active=true;
                break;
            }
        }
        Fiber::ptr ft = fc.getTask();
        //如果是协程，且不处于终止状态
        if(ft != nullptr && ft->getState()!=Fiber::TERM && ft->getState() != Fiber::EXCEPT){
            ft->swapIn();
            --m_activeThreadCount;
            fc.reset();
        }else{//没有任务执行，执行idle
            if(is_active){
                --m_activeThreadCount;
                continue;
            }
            if((idle_fiber->getState()==Fiber::TERM) || (idle_fiber->getState() == Fiber::EXCEPT)){
                FEPOH_LOG_INFO(s_log_system)<<"idle term";
                break;
            }
            ++m_idleThreadCount;
            idle_fiber->swapIn();
            if(idle_fiber->getState()!=Fiber::EXCEPT
                &&idle_fiber->getState()!=Fiber::TERM){
                idle_fiber->setState(Fiber::State::HOLD);
            }
            --m_idleThreadCount;
        }
    }
} 

void ScheduleManager::notice(){
    FEPOH_LOG_DEBUG(s_log_system)<<"notice";
}

//空闲任务
void ScheduleManager::idle(){
    while(!isStop()){
        Fiber::GetThis()->swapOutHold();
    }
}

//单个添加任务
void ScheduleManager::schedule(Task task){
    MutexLock locker(m_mutex);
    scheduleNoLock(task);
}

void ScheduleManager::schedule(std::function<void()> cb){
    Task task(cb);
    scheduleNoLock(task);
}

void ScheduleManager::schedule(Fiber::ptr fiber){
    Task task(fiber);
    scheduleNoLock(task);
}

void ScheduleManager::scheduleNoLock(Task task){
    m_tasks.push_back(task);
}

void ScheduleManager::scheduleNoLock(std::function<void()> cb){
    Task task(cb);
    m_tasks.push_back(task);
}

Fiber* ScheduleManager::GetRootFiber(){
    return t_root_fiber.get();
}

bool ScheduleManager::isStop(){
    return m_isStop
         &&m_tasks.empty()
         &&m_activeThreadCount==0;
}

ScheduleManager* ScheduleManager::GetThis(){
    return t_scheduler;
}


/*
    Task
*/
Task::Task(std::function<void()> cb):m_cb(cb),m_fiber(nullptr){
}

Task::Task(Fiber::ptr fiber):m_fiber(fiber),m_cb(nullptr){
}

Fiber::ptr Task::getTask(){
    if(isEmpty()){
        return nullptr;
    }
    if(m_fiber){
        return m_fiber;
    }
    if(m_cb){
        return Fiber::ptr(new Fiber(m_cb));
    }
    return nullptr;
}

void Task::reset(){
    std::function<void()> cb;
    cb.swap(m_cb);
    m_fiber = nullptr;
}

bool Task::isEmpty(){
    if((m_cb)||(m_fiber)){
        return false;
    }
    return true;
}

bool Task::setTask(Fiber::ptr fiber){
    reset();
    if(fiber){
        m_fiber = fiber;
        return true;
    }
    return false;
}
bool Task::setTask(std::function<void()> cb){
    reset();
    if(cb){
        m_cb = cb;
        return true;
    }
    return false;
}

}//namespace
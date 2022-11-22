/*
 * @Author: fepo_h
 * @Date: 2022-11-10 00:37:26
 * @LastEditors: fepo_h
 * @LastEditTime: 2022-11-22 16:30:05
 * @FilePath: /fepoh/workspace/fepoh_server/src/schedule_manager.h
 * @Description: 
 * 
 * Copyright (c) 2022 by FepoH Fepo_H@163.com, All Rights Reserved. 
 * @version: V1.0.0
 * @Mailbox: Fepo_H@163.com
 * @Descripttion: 
 */
#pragma once

#include "fiber.h"
#include "thread/mutex.h"
#include "thread/thread.h"
#include "thread/sem.h"
#include "noncopyable.h"

#include <string>
#include <atomic>
#include <functional>
#include <vector>
#include <list>


namespace fepoh{

/**
 * @description: 任务类,
 * @return {*}
 */
class Task{
    public:
        friend class ScheduleManager;

        Task(std::function<void()> cb);
        Task(Fiber::ptr fiber);
        /**
         * @description: 获取协程:如果是回调函数,直接创建Fiber并返回
         * @return {*}
         */
        Fiber::ptr getTask();
        //重置
        void reset();
        //判断是否有任务
        bool isEmpty();
        bool setTask(Fiber::ptr fiber);
        bool setTask(std::function<void()> cb);
    private:
        //空的构造函数,供stl构造,....
        Task(){} 
        std::function<void()> m_cb;
        Fiber::ptr m_fiber;
};

//执行流程的线程只能被一个调度器纳入.
class ScheduleManager : public Noncopyable{
    public:
        typedef std::shared_ptr<ScheduleManager> ptr;
        /**
         * @description: 构造函数
         * @return {*}
         * @param {string&} name 调度器名称
         * @param {uint32_t} thrCount 线程数
         * @param {bool} use_caller
         */        
        ScheduleManager(const std::string& name="fepoh_scheduler",uint32_t thrCount = 1,bool use_caller = false);
        /**
         * @description: 析构函数
         * @return {*}
         */        
        virtual ~ScheduleManager();
        /**
         * @description: 启动协程调度器:分配线程
         * @return {*}
         */        
        void start();
        /**
         * @description: 停止协程调度器:在stop中完成任务
         * @return {*}
         */        
        void stop();
        /**
         * @description: 批量添加任务:回调函数或协程
         * @return {*}
         */        
        template<class Iterator>
        void schedule(Iterator begin,Iterator end){
            MutexLock lock(m_mutex);
            while(begin != end){
                scheduleNoLock(*begin);
                ++begin;
            }
        }
        /**
         * @description: 添加单个任务
         * @return {*}
         * @param {Task} task
         */        
        void schedule(Task task);
        /**
         * @description: 添加任务
         * @return {*}
         * @param {function<void()>} cb
         */        
        void schedule(std::function<void()> cb);
        /**
         * @description: 添加任务
         * @return {*}
         * @param {ptr} fiber
         */        
        void schedule(Fiber::ptr fiber);
    public:
        /**
         * @description: 获取当前线程的协程调度器
         * @return {*}
         */        
        static ScheduleManager* GetThis();
        /**
         * @description: 设置线程的协程调度器
         * @return {*}
         * @param {ScheduleManager*} val
         */        
        static void SetThis(ScheduleManager* val);
        /**
         * @description: 获取管理协程
         * @return {*}
         */        
        static Fiber* GetRootFiber();
        /**
         * @description: 是否有空闲协程
         * @return {*}
         */        
        bool  hasIdleThread() const {return m_idleThreadCount>0;}
    protected:
        /**
         * @description: idle回调
         * @return {*}
         */        
        virtual void idle();
        /**
         * @description: 唤醒线程
         * @return {*}
         */        
        virtual void notice();
        /**
         * @description: 停止
         * @return {*}
         */        
        bool m_stopping = true;
        bool m_autoStop = false;
        /**
         * @description: 停止:用于子类
         * @return {*}
         */        
        virtual bool stopping();
    private:
        /**
         * @description: 无锁添加任务
         * @return {*}
         */
        void scheduleNoLock(Task task);
        /**
         * @description: 无锁添加任务
         * @return {*}
         * @param {function<void()>} cb
         */        
        void scheduleNoLock(std::function<void()> cb);
        /**
         * @description: run回调函数
         * @return {*}
         */        
        void run();
      
        // static bool has_use_caller;
    private:
        bool m_useCaller;           
        Mutex m_mutex;                      //锁
        std::string m_name;                 //调度器名称
        uint32_t m_threadCount;             //线程数
        std::list<Task> m_tasks;            //任务池,回调函数或协程,封装后,返回的都是协程
        std::vector<uint64_t> m_threadIds;  //线程id
        std::vector<Thread::ptr> m_threads; //线程池
        std::atomic<uint32_t> m_activeThreadCount = {0};    //活跃的线程数
        std::atomic<uint32_t> m_idleThreadCount = {0};      //空闲的线程数
};


}//namespace

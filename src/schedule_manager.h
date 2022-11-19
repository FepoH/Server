/*
 * @Author: fepo_h
 * @Date: 2022-11-10 00:37:26
 * @LastEditors: fepo_h
 * @LastEditTime: 2022-11-20 03:15:42
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
//TODO,需要优化,不把回调函数返回成Fiber,这样性能损失过大,协程执行一堆操作,而回调函数拿来即可用
class Task{
    public:
        friend class ScheduleManager;

        Task(std::function<void()> cb);
        Task(Fiber::ptr fiber);
        //如果是回调函数,直接创建Fiber,并返回
        Fiber::ptr getTask();
        //重置,回收高效利用
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

        ScheduleManager(const std::string& name="fepoh_scheduler",uint32_t thrCount = 1,bool use_caller = false);
        virtual ~ScheduleManager();
        //启动调度器
        void start();
        //停止调度器,在停止调度器中执行任务
        void stop();
        //批量添加任务
        template<class Iterator>
        void schedule(Iterator begin,Iterator end){
            MutexLock lock(m_mutex);
            while(begin != end){
                scheduleNoLock(*begin);
                ++begin;
            }
        }
        //单个添加任务
        void schedule(Task task);
        void schedule(std::function<void()> cb);
        void schedule(Fiber::ptr fiber);
    public:
        //获取线程的协程调度器
        static ScheduleManager* GetThis();
        //设置线程的协程调度器
        static void SetThis(ScheduleManager* val);
        //获取root协程
        static Fiber* GetRootFiber();
        //是否有空闲线程
        bool  hasIdleThread() const {return m_idleThreadCount>0;}
    protected:
        //空闲任务
        virtual void idle();
        //是否停止
        virtual bool isStop();
        //通知,唤醒线程
        virtual void notice();

        bool m_isStop = true;               //停止
        bool m_isStopping = false;
    private:
        //无锁添加任务
        void scheduleNoLock(Task task);
        void scheduleNoLock(std::function<void()> cb);
        //线程执行函数
        void run();
        //是否已经有use_caller
        static bool has_use_caller;
        
    private:
        
        //是否执行流程的线程也纳入进去,默认纳入应该好一些,有利于资源的利用
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

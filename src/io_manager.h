/*
 * @Author: fepo_h
 * @Date: 2022-11-20 15:48:39
 * @LastEditors: fepo_h
 * @LastEditTime: 2022-11-20 16:00:38
 * @FilePath: /fepoh/workspace/fepoh_server/src/io_manager.h
 * @Description: 
 * 
 * Copyright (c) 2022 by FepoH Fepo_H@163.com, All Rights Reserved. 
 * @version: V1.0.0
 * @Mailbox: Fepo_H@163.com
 * @Descripttion: 
 */
#pragma once
#include "schedule_manager.h"
#include "timer.h"
#include "fiber.h"
#include "thread/mutex.h"

#include <unistd.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <atomic>


namespace fepoh{

class IOManager : public ScheduleManager, public TimerManager{
public:
    typedef std::shared_ptr<IOManager> ptr;
    /**
     * @description: 事件类型
     */    
    enum Event{
        NONE = 0x0,     //无事件
        READ = 0x1,     //读事件
        WRITE = 0x4     //写事件
    };
private:
    /**
     * @description: 文件描述符上下文
     */
    struct FdContext{
        //事件内容
        struct EventContext{
            //事件所属调度器
            ScheduleManager* scheduler = nullptr;
            //协程,即事件回调
            Fiber::ptr fiber;
        };
        //获取事件回调:每次只能获取一个事件
        EventContext& getContext(Event ev);
        //重置事件回调
        void resetContext(EventContext& ev_ctx);
        //触发事件回调
        void triggerContext(Event ev);
        //读事件回调
        EventContext read;
        //写事件回调
        EventContext write;
        //事件
        Event events = Event::NONE;
        //文件描述符
        int fd = 0;
        //锁,多线程资源竞争
        Mutex mutex;
    };

public:
    /**
     * @description: 构造函数
     * @return {*}
     * @param {string&} name 调度器名称
     * @param {size_t} threadCount 线程数
     * @param {bool} use_caller 
     */
    IOManager(const std::string& name="",size_t threadCount=1,bool use_caller=true);
    /**
     * @description: 析构函数
     */    
    ~IOManager();
    /**
     * @description: 添加IO事件
     * @return {*} -1失败,0成功
     * @param {int} fd 文件描述符
     * @param {Event} event 事件类型
     * @param {function<void()>} cb 事件回调
     */    
    int addEvent(int fd,Event event,std::function<void()> cb=nullptr);
    /**
     * @description: 删除IO事件:不会触发事件
     * @return {*}
     * @param {int} fd 文件描述符
     * @param {Event} event 事件类型
     */    
    int delEvent(int fd,Event event);
    /**
     * @description: 取消IO事件:会触发回调
     * @return {*}
     * @param {int} fd 文件描述符
     * @param {Event} event 事件类型
     */    
    int cancelEvent(int fd,Event event);
    /**
     * @description: 取消文件描述符上的所有事件:会触发事件
     * @return {*}
     * @param {int} fd
     */    
    int cancelAll(int fd);
public:
    /**
     * @description: 获取当前线程的IO调度器
     * @return {*}
     */    
    static IOManager* GetThis();
protected:
    /**
     * @description: 唤醒线程
     * @return {*}
     */    
    void notice() override;
    /**
     * @description: 是否停止
     * @return {*}
     */    
    bool isStop() override;
    /**
     * @description: 空闲回调:基于epoll
     * @return {*}
     */    
    void idle() override;
    /**
     * @description: 
     * @return {*}
     * @param {size_t} size
     */    
    /**
     * @description: 分配大小
     * @return {*}
     * @param {size_t} size 大小
     */    
    void contextResize(size_t size);
private:
    int m_epollfd;          //epoll fd
    int m_pipefd[2];        //管道,用来notice,唤醒线程
    RWMutex m_mutex;        //锁
    std::vector<FdContext*> m_fdContexts;   //任务
    std::atomic<size_t> m_pendingEventCount = {0};  //等待事件数量,即在队列中的事件数
};

}//namespace
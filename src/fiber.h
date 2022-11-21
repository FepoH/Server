/*
 * @Author: fepo_h
 * @Date: 2022-11-08 20:30:04
 * @LastEditors: fepo_h
 * @LastEditTime: 2022-11-20 13:06:41
 * @FilePath: /fepoh/workspace/fepoh_server/src/fiber.h
 * @Description: 协程封装,适配IOManager
 * 
 * Copyright (c) 2022 by FepoH Fepo_H@163.com, All Rights Reserved. 
 * @version: V1.0.0
 * @Mailbox: Fepo_H@163.com
 * @Descripttion: 
 */
#pragma once

#include <memory>
#include <string>
#include <functional>
#include <ucontext.h>

namespace fepoh{

/*
    协程,不能单独使用,未定义单独使用的api,必须联合scheduler使用
    若想单独使用,可仿照写出单独使用的api.非常简单.
    
    单线程操作,不涉及多线程

    心得:此处必须非常注意ptr和裸指针的使用,一不小心就会犯下只能指针未释放的错误;
*/
class Fiber : public std::enable_shared_from_this<Fiber>{
public:
    typedef std::shared_ptr<Fiber> ptr;
    /**
     * @description: 协程状态信息
     * @return {*}
     */    
    enum State{
        INIT=0,         //初始化状态
        HOLD,           //暂停状态
        EXEC,           //正在执行
        EXCEPT,         //异常状态
        TERM            //结束状态
    };

    /**
     * @description: 约定:
     *  use_caller:     main_fiber != root_fiber<----->other_fiber  使用root_fiber管理协程
     *  非use_caller:   main_fiber  = root_fiber<----->other_fiber  使用main_fiber管理协程
     *  以下以管理协程指代,须区分所指
     */    

    /**
     * @description: 构造函数
     * @return {*}
     * @param {function<void()>} cb 回调函数
     * @param {uint32_t} stacksize 栈大小
     * @param {bool} use_caller 该协程是否纳入调度器
     */    
    Fiber(std::function<void()> cb,uint32_t stacksize = 0,bool use_caller = false);
    ~Fiber();
    /**
     * @description: 协程切入:用于管理协程---->other_fiber切换
     * @return {*}
     */    
    void swapIn();
    /**
     * @description: 协程切出:用于other_fiber<---->管理协程切换
     * @return {*}
     */    
    void swapOut();
    /**
     * @description: 协程以暂停状态切出:用于other_fiber---->管理协程切换
     * @return {*}
     */    
    void swapOutHold();
    /**
     * @description: 协程切入:用于use_caller:main_fiber----->root_fiber切换
     * @return {*}
     */    
    void call();
    /**
     * @description: 协程切出:用于use_caller协程:root_fiber----->main_fiber切换
     * @return {*}
     */    
    void back();
    /**
     * @description: 协程以暂停状态切出:root_fiber----->main_fiber切换
     * @return {*}
     */    
    void backHold();
    /**
     * @description: 重置协程
     * @return {*}
     * @param {function<void()>} cb 回调函数
     */    
    void reset(std::function<void()> cb);
    /**
     * @description: 获取当前线程的管理协程
     * @return {*}
     */    
    static Fiber::ptr GetMainFiber();
    /**
     * @description: 获取协程id
     * @return {*}
     */    
    static uint32_t GetFiberId();
    /**
     * @description: 获取当前协程:当此时没有main_fiber时,创建main_fiber(此时相当于单例)
     *                           当此时有main_fiber协程时,获取当前执行协程
     * @return {*}
     */    
    static Fiber::ptr GetThis();
    /**
     * @description: 设置当前协程
     * @return {*}
     * @param {Fiber*} val 协程
     */    
    static void SetThis(Fiber* val);
    /**
     * @description: 
     * @return {*}
     */    
    static Fiber::ptr GetThreadMainFiber();
public:
    //获取协程id
    uint32_t getId() const {return m_id;}
    //获取栈大小
    uint32_t getStacksize() const {return m_stacksize;}
    //获取协程执行状态
    State getState() const {return m_state;}
    void setState(State val) {m_state = val;}
private:
    //main_fiber为单例模式
    Fiber();
    //不使用调度器api
    static void Run();
    //使用调度器api
    static void ScheduleRun();
private:
    uint64_t m_id=0;                    //协程id
    uint32_t m_stacksize=0;             //栈大小
    ucontext_t m_ctx;                   //上下文
    State m_state = State::INIT;        //状态
    void* m_stack=nullptr;              //栈指针
    std::function<void()> m_cb=nullptr; //回调函数
};

}//namespace

//root_fiber -->manager_fiber
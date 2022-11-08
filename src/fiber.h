#pragma once

#include "parameter.h"

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
    enum State{
        INIT=0,         //初始化状态
        HOLD,           //暂停状态
        EXEC,           //正在执行
        EXCEPT,         //异常状态
        TERM            //结束状态
    };
    //默认非use_caller
    Fiber(std::function<void()> cb,uint32_t stacksize = 0,bool use_caller = false);
    ~Fiber();
    //切换root fiber
    void swapIn();
    void swapOut();
    void swapOutHold();
    //切换main fiber
    void call();
    void back();
    void backHold();
    //获取main协程
    static Fiber::ptr GetMainFiber();
    //获取当前执行协程
    static Fiber::ptr GetThis();
    //设置当前执行协程
    static void SetThis(Fiber* val);
    //对于非use_caller的线程,返回为main协程,否则返回线程root协程
    static Fiber::ptr GetThreadMainFiber();
public:
    //获取协程id
    uint32_t getId() const {return m_id;}
    //获取栈大小
    uint32_t getStacksize() const {return m_stacksize;}
    //获取协程执行状态
    State getState() const {return m_state;}
    void setState(State val) {m_state = val;}
    //用于测试...
    // static void test(const std::string& str = "");
    // static void test(Fiber::ptr fiber);
private:
    //main协程为单例模式
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
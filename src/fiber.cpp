/*
 * @Author: fepo_h
 * @Date: 2022-11-20 02:48:07
 * @LastEditors: fepo_h
 * @LastEditTime: 2022-11-22 19:54:44
 * @FilePath: /fepoh/workspace/fepoh_server/src/fiber.cpp
 * @Description: 
 * 
 * Copyright (c) 2022 by FepoH Fepo_H@163.com, All Rights Reserved. 
 * @version: V1.0.0
 * @Mailbox: Fepo_H@163.com
 * @Descripttion: 
 */
#include "fiber.h"
#include "log/log.h"
#include "macro.h"
#include "config.h"
#include "schedule_manager.h"

#include <atomic>

namespace fepoh{

//智能指针
//注意main fiber和root fiber的区别
static thread_local Fiber::ptr t_main_fiber;    //main_fiber
//裸指针,用智能指针应该更好...
static thread_local Fiber* t_fiber = nullptr;   //当前执行协程
/**
 * @description: 协程数量:main函数结束后,t_fiber_count必须为0,其余情况都视为内存泄漏
 * @return {*}
 */
static std::atomic<uint64_t> t_fiber_count = {0};
/**
 * @description: 协程id:一直递增
 * @return {*}
 */
static std::atomic<uint64_t> t_fiber_id = {0};

static Logger::ptr s_log_system = FEPOH_LOG_NAME("system");

static ConfigVar<uint32_t>::ptr g_fiber_stacksize = 
        Config::Lookup<uint32_t>(50 * 1024,"fiber.stacksize","fiber stack size");

/**
 * @description: main协程只负责本线程内协程的调度,故没有回调函数和占空间,且将它一直置于执行状态
 *               注:use_caller使用root_fiber调度,main_fiber负责流程执行,但类似上面也没有回调函数,占空间等...
 * @return {*}
 */
Fiber::Fiber():m_stacksize(0),m_state(State::EXEC){
    m_id = ++t_fiber_id;
    ++t_fiber_count;
    SetThis(this);
    //获取上下文
    if(getcontext(&m_ctx)){
        FEPOH_ASSERT1(false,std::string("Fiber::Fiber error.getcontext error"));
    }
    FEPOH_LOG_DEBUG(s_log_system) << "Fiber::Fiber.mainfiber id = " << m_id;
}

Fiber::Fiber(std::function<void()> cb,uint32_t stacksize,bool use_caller)
        :m_cb(cb){
    //最少分配10k,太少了容易栈溢出导致错误,为防止不必要的麻烦,故定义最小栈空间;
    m_stacksize = stacksize >= 128 * 1024 ? stacksize : g_fiber_stacksize->getValue();
    m_id = ++t_fiber_id;
    ++t_fiber_count;
    m_state = State::INIT;
    m_stack = (void*)malloc(m_stacksize);
    if(getcontext(&m_ctx)){
        FEPOH_ASSERT1(false,std::string("Fiber::Fiber error.getcontext error"));
    }
    m_ctx.uc_stack.ss_sp = m_stack;         //内存
    m_ctx.uc_stack.ss_size = m_stacksize;   //栈大小
    m_ctx.uc_stack.ss_flags = 0;
    /**
     * @description: 本项目采取做法为:执行完毕后返回管理协程,后续再通过管理协程调度,不采用链接的方式
     * @return {*}
     */           
    m_ctx.uc_link = nullptr;
    if(!use_caller){
        //非use_caller使用
        makecontext(&m_ctx,&Fiber::Run,0);
    }else{
        //use_caller使用
        makecontext(&m_ctx,&Fiber::ScheduleRun,0);
    }
    //FEPOH_LOG_DEBUG(s_log_system) << "Fiber::Fiber.fiber id = " << m_id;
}

Fiber::~Fiber(){
    //释放主协程
    if((m_state == State::EXEC)
        &&(m_stacksize == 0)
        &&(m_stack == nullptr )){
        --t_fiber_count;
        FEPOH_LOG_INFO(s_log_system) << "~main fiber.fiber count = " << t_fiber_count << ",my id = " << m_id;
    }else{
    //释放其他协程
        if(m_state == State::EXEC){
            FEPOH_ASSERT1(false,"Fiber::~Fiber error.Destructor the fiber which state is EXEC.");
        }
        --t_fiber_count;
        //释放栈空间
        free(m_stack);
    }
}

void Fiber::swapIn(){
    //设置当前协程为other_fiber
    t_fiber = this;
    //设置状态
    m_state = State::EXEC;
    //管理协程--->other_fiber
    int rt = swapcontext(&(ScheduleManager::GetRootFiber()->m_ctx),&(this->m_ctx));
    if(rt){
        m_state = State::EXCEPT;
        FEPOH_LOG_ERROR(s_log_system) << "Fiber::swapOut error.rt = " << rt 
                << ".fiber id = " <<m_id;
    }
}

void Fiber::swapOut(){
    //设置当前执行协程为管理协程
    t_fiber = ScheduleManager::GetRootFiber();
    //other_fiber-->管理协程
    int rt = swapcontext(&(this->m_ctx),&(t_fiber->m_ctx));
    if(rt){
        m_state = State::EXCEPT;
        FEPOH_LOG_ERROR(s_log_system) << "Fiber::swapOut error.rt = " << rt
                << ".fiber id = " <<m_id;
    }
    //此代码不可达
    FEPOH_LOG_ERROR(s_log_system) << "Fiber::swapOut.never reach";
}

void Fiber::swapOutHold(){
    //设置当前协程为管理协程
    t_fiber = ScheduleManager::GetRootFiber();
    m_state = State::HOLD;
    //other_fiber-->管理协程
    int rt = swapcontext(&(this->m_ctx),&(t_fiber->m_ctx));
    if(rt){
        m_state = State::EXCEPT;
        FEPOH_LOG_ERROR(s_log_system) << "Fiber::swapOut error.rt = " << rt
                << ".fiber id = " <<m_id;
    }
}


void Fiber::call(){
    t_fiber = this;
    Fiber* mainFiber = t_main_fiber.get();
    //root_fiber-->main_fiber
    FEPOH_LOG_INFO(s_log_system) << "root_fiber --> main_fiber call";
    int rt = swapcontext(&(mainFiber->m_ctx),&(this->m_ctx));
    if(rt){
        m_state = State::EXCEPT;
        FEPOH_LOG_ERROR(s_log_system) << "Fiber::swapOut error.rt = " << rt 
                << ".fiber id = " <<m_id;
    }
}

void Fiber::back(){
    t_fiber = t_main_fiber.get();
    FEPOH_LOG_INFO(s_log_system) << "root_fiber --> main_fiber back";
    //main_fiber-->root_fiber
    int rt = swapcontext(&(this->m_ctx),&(t_fiber->m_ctx));
    if(rt){
        m_state = State::EXCEPT;
        FEPOH_LOG_ERROR(s_log_system) << "Fiber::swapOut error.rt = " << rt
                << ".fiber id = " <<m_id;
    }
    //此代码不可达
    FEPOH_LOG_ERROR(s_log_system) << "Fiber::back.never reach";
}

void Fiber::backHold(){
    t_fiber = t_main_fiber.get();
    m_state = HOLD;
    //root_fiber-->main_fiber
    int rt = swapcontext(&(this->m_ctx),&(t_fiber->m_ctx));
    if(rt){
        m_state = State::EXCEPT;
        FEPOH_LOG_ERROR(s_log_system) << "Fiber::swapOut error.rt = " << rt
                << ".fiber id = " <<m_id;
    }
}

void Fiber::reset(std::function<void()> cb){
    FEPOH_ASSERT(m_stack);
    FEPOH_ASSERT(m_state != Fiber::EXEC);
    m_cb = cb;
    if(getcontext(&m_ctx)) {
        FEPOH_ASSERT1(false, "getcontext");
    }

    m_ctx.uc_link = nullptr;
    m_ctx.uc_stack.ss_sp = m_stack;
    m_ctx.uc_stack.ss_size = m_stacksize;

    makecontext(&m_ctx, &Fiber::Run, 0);
    m_state = INIT;
}

Fiber::ptr Fiber::GetMainFiber(){
    return t_main_fiber;
}

void Fiber::SetThis(Fiber* val){
    t_fiber = val;
}

Fiber::ptr Fiber::GetThis(){
    if(t_fiber){
        return t_fiber->shared_from_this();
    }
    t_main_fiber.reset(new Fiber());
    t_fiber = t_main_fiber.get();
    return t_main_fiber;
}

void Fiber::Run(){
    Fiber::ptr cur = Fiber::GetThis();
    FEPOH_ASSERT(cur);
    try{
        std::function<void()> cb;
        cb.swap(cur->m_cb);
        cb();
        cur->m_state = State::TERM;
    }catch(...){
        cur->m_state = State::EXCEPT;
        FEPOH_LOG_ERROR(s_log_system) << "Fiber::Run error.fiber id = " << cur->m_id;
    }
    //由于此处切出后，此函数不可能结束，必须将cur置空，将智能指针计数减一
    Fiber* tmp=cur.get();
    cur.reset();
    tmp->swapOut();
    //切出后，此代码不可达
    FEPOH_LOG_ERROR(s_log_system)<<"Fiber::Run error.never reach";
}

void Fiber::ScheduleRun(){
    Fiber::ptr cur = Fiber::GetThis();
    FEPOH_ASSERT(cur);
    try{
        std::function<void()> cb;
        cb.swap(cur->m_cb);
        cb();
        cur->m_state = State::TERM;
    }catch(...){
        cur->m_state = State::EXCEPT;
        FEPOH_LOG_ERROR(s_log_system) << "Fiber::Run error.fiber id = " << cur->m_id;
    }
    Fiber* tmp=cur.get();
    cur.reset();
    tmp->back();
    //切出后，此代码不可达
    FEPOH_LOG_ERROR(s_log_system)<<"Fiber::ScheduleRun error.never reach";
}

uint32_t Fiber::GetFiberId(){
    if(t_fiber){
        return t_fiber->getId();
    }
    return 0;
}



}//namespace
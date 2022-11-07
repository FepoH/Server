#include "fiber.h"
#include "log/log.h"
#include "macro.h"
#include "schedule_manager.h"

#include <atomic>

namespace fepoh{
//智能指针
//注意main fiber和root fiber的区别
static thread_local Fiber::ptr t_main_fiber;    //main协程
//裸指针
static thread_local Fiber* t_fiber = nullptr;   //当前执行协程
//main函数结束后,t_fiber_count必须为0
static std::atomic<uint64_t> t_fiber_count = {0};    //协程数量
static std::atomic<uint64_t> t_fiber_id = {0};       //协程id,一直递增

static Logger::ptr s_log_system = FEPOH_LOG_NAME("system");

// static ConfigVar<uint32_t>::ptr g_fiber_stacksize = 
//         Config::Lookup<uint32_t>(128*1024,"fiber.stacksize","fiber stack size");

//main协程负责切换,无栈大小和栈空间,且一直处于执行状态;
Fiber::Fiber():m_stacksize(0),m_state(State::EXEC){
    m_id = ++t_fiber_id;
    ++t_fiber_count;

    SetThis(this);
    if(getcontext(&m_ctx)){
        FEPOH_ASSERT1(false,std::string("Fiber::Fiber error.getcontext error"));
    }
    FEPOH_LOG_DEBUG(s_log_system) << "Fiber::Fiber.mainfiber id = " << m_id;
}

Fiber::Fiber(std::function<void()> cb,uint32_t stacksize,bool use_caller)
        :m_cb(cb){
    //最少分配50k,太少了容易栈溢出导致错误,为防止不必要的麻烦,故定义;
    m_stacksize = stacksize > 50*1028 ? stacksize : g_fiber_stacksize->getValue();
    //m_stacksize = stacksize > 50*1028 ? stacksize : g_fiber_stacksize;
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
    m_ctx.uc_link = nullptr;                //执行此上下文后,直接结束,不定义链接的协程
    if(!use_caller){
        //非use_caller使用
        makecontext(&m_ctx,&Fiber::Run,0);
    }else{
        //use_caller使用
        makecontext(&m_ctx,&Fiber::ScheduleRun,0);
        
    }
    FEPOH_LOG_DEBUG(s_log_system) << "Fiber::Fiber.fiber id = " << m_id;
}

Fiber::~Fiber(){
    //释放主协程
    if((m_state == State::EXEC)
        &&(m_stacksize == 0)
        &&(m_stack ==nullptr )){
        --t_fiber_count;
        FEPOH_LOG_DEBUG(s_log_system) << "~main fiber";
    }else{
    //释放非主协程
        if(m_state == State::EXEC){
            FEPOH_ASSERT1(false,"Fiber::~Fiber error.Destructor the fiber which state is EXEC.");
        }
        --t_fiber_count;
        //释放栈空间
        free(m_stack);
    }
    FEPOH_LOG_DEBUG(s_log_system) << "Fiber::~Fiber.fiber count = " << t_fiber_count << " id = " << m_id ;
}

void Fiber::swapIn(){
    t_fiber = this;
    m_state = State::EXEC;
    //切换协程
    int rt = swapcontext(&(ScheduleManager::GetRootFiber()->m_ctx),&(this->m_ctx));
    if(rt){
        m_state = State::EXCEPT;
        FEPOH_LOG_ERROR(s_log_system) << "Fiber::swapOut error.rt = " << rt 
                << ".fiber id = " <<m_id;
    }
}

void Fiber::swapOut(){
    t_fiber = ScheduleManager::GetRootFiber();
    int rt = swapcontext(&(this->m_ctx),&(t_fiber->m_ctx));
    if(rt){
        m_state = State::EXCEPT;
        FEPOH_LOG_ERROR(s_log_system) << "Fiber::swapOut error.rt = " << rt
                << ".fiber id = " <<m_id;
    }
    FEPOH_LOG_ERROR(s_log_system) << "Fiber::swapOut.never reach";
}

void Fiber::swapOutHold(){
    t_fiber = ScheduleManager::GetRootFiber();
    m_state = State::HOLD;
    int rt = swapcontext(&(this->m_ctx),&(t_fiber->m_ctx));
    if(rt){
        m_state = State::EXCEPT;
        FEPOH_LOG_ERROR(s_log_system) << "Fiber::swapOut error.rt = " << rt
                << ".fiber id = " <<m_id;
    }
}

//切换main fiber
void Fiber::call(){
    t_fiber = this;
    Fiber* mainFiber = GetMainFiber().get();
    int rt = swapcontext(&(mainFiber->m_ctx),&(this->m_ctx));
    if(rt){
        m_state = State::EXCEPT;
        FEPOH_LOG_ERROR(s_log_system) << "Fiber::swapOut error.rt = " << rt 
                << ".fiber id = " <<m_id;
    }
}

void Fiber::back(){
    t_fiber = t_main_fiber.get();
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
    int rt = swapcontext(&(this->m_ctx),&(t_fiber->m_ctx));
    if(rt){
        m_state = State::EXCEPT;
        FEPOH_LOG_ERROR(s_log_system) << "Fiber::swapOut error.rt = " << rt
                << ".fiber id = " <<m_id;
    }
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
    //FEPOH_LOG_DEBUG(s_log_system) << "Fiber::Run.id=" << cur->m_id;
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
    FEPOH_LOG_DEBUG(s_log_system)<<"Fiber::Run error.never reach";
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
    FEPOH_LOG_DEBUG(s_log_system)<<"Fiber::ScheduleRun error.never reach";
}

// void Fiber::test(const std::string& str){
//     std::cout << t_main_fiber.use_count() << "  " << str <<std::endl;
// }

// void Fiber::test(Fiber::ptr fiber){

// }

}//namespace
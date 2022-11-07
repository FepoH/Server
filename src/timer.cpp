#include "timer.h"
#include "util.h"
#include "log/log.h"

namespace fepoh{

static Logger::ptr s_log_system = FEPOH_LOG_NAME("system");

bool Timer::TimerCmp::operator()(Timer::ptr lhs,Timer::ptr rhs){
    if(!lhs&&!rhs){
        return false;
    }
    if(!lhs){
        return true;
    }
    if(!rhs){
        return false;
    }
    if(lhs->getTrigger()<=rhs->getTrigger()){
        return true;
    }
    return lhs.get() < rhs.get();
}

Timer::Timer(std::function<void()> cb,uint64_t ms,TimerManager* manager,bool recurring)
        :m_cb(cb),m_ms(ms),m_recurring(recurring),m_manager(manager){
    m_trigger = fepoh::GetCurTimeMs() + m_ms;
}

Timer::Timer(uint64_t trigger):m_trigger(trigger){

}

void Timer::refresh(uint64_t ms,bool from_now){
    if(!m_manager || !m_cb){
        return ;
    }
    MutexLock locker(m_manager->m_mutex);
    auto it = m_manager->m_timers.find(this->shared_from_this());
    if(it != m_manager->m_timers.end()){
        m_manager->m_timers.erase(it);
    }
    refreshNoLock(ms,from_now);
    m_manager->m_timers.insert(this->shared_from_this());
}

void Timer::refreshNoLock(uint64_t ms ,bool from_now){
    uint64_t begin = from_now ? fepoh::GetCurTimeMs() : m_trigger - m_ms;
    m_ms = ms==~0ull ? m_ms : ms;
    m_trigger = begin + m_ms;

}

void Timer::reset(std::function<void()> cb,uint64_t ms,bool from_now){
    if(!m_manager || !cb){
        return ;
    }
    MutexLock m_mutex(m_manager->m_mutex);
    m_cb = cb;
    refreshNoLock(ms,from_now);
}

void Timer::cancel(){
    if(!m_manager){
        m_cb = nullptr;
        return ;
    }
    MutexLock locker(m_manager->m_mutex);
    auto it = m_manager->m_timers.find(this->shared_from_this());
    if(it != m_manager->m_timers.end()){
        m_manager->m_timers.erase(it);
    }
    m_cb = nullptr;
    m_manager = nullptr;
}


bool Timer::isExpired(){
    return (m_trigger <= fepoh::GetCurTimeMs())&&(!m_cb);
}

//已经过时的和无回调函数的timer不能插入
void TimerManager::addTimer(Timer::ptr timer){
    if(timer->isExpired()){
        return ;
    }
    MutexLock lock(m_mutex);
    m_timers.insert(timer);
}

void TimerManager::delTimer(Timer::ptr timer){
    MutexLock lock(m_mutex);
    auto it = m_timers.find(timer);
    if(it != m_timers.end()){
        (*it)->m_manager = nullptr;
        m_timers.erase(it);
    }
}

void TimerManager::clrTimer(){
    MutexLock lock(m_mutex);
    for(auto it= m_timers.begin();it!=m_timers.end();++it){
        (*it)->m_manager = nullptr;
        (*it)->m_cb = nullptr;
    }
    m_timers.clear();
}


std::list<std::function<void()>> TimerManager::listAllExpired(){
    uint64_t cur = fepoh::GetCurTimeMs();
    std::list<std::function<void()>> cbs;
    std::list<Timer::ptr> timers;
    Mutex locker(m_mutex);
    if(m_timers.empty()){
        return cbs;
    }
    auto it = m_timers.begin();
    for(;it != m_timers.end();++it){
        if((*it)->getTrigger() <= cur){
            timers.push_back(*it);
        }else{
            break;
        }
    }
    m_timers.erase(m_timers.begin(),it);
    for(auto timer:timers){
        if(!timer->m_cb){
            continue;
        }
        cbs.push_back(timer->m_cb);
        if(!timer->m_recurring){
            continue;
        }
        timer->refreshNoLock();
        m_timers.insert(timer);
    }
    return cbs;
}

bool TimerManager::hasTimer(){
    MutexLock lock(m_mutex);
    return m_timers.empty();
}

}//namespace
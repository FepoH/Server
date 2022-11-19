#include "timer.h"
#include "util.h"
#include "log/log.h"

namespace fepoh{

static Logger::ptr s_log_system = FEPOH_LOG_NAME("system");


bool Timer::Comparator::operator()(const Timer::ptr& lhs
                        ,const Timer::ptr& rhs) const {
    if(!lhs && !rhs) {
        return false;
    }
    if(!lhs) {
        return true;
    }
    if(!rhs) {
        return false;
    }
    if(lhs->m_next < rhs->m_next) {
        return true;
    }
    if(rhs->m_next < lhs->m_next) {
        return false;
    }
    return lhs.get() < rhs.get();
}


Timer::Timer(uint64_t ms, TimerManager* manager, std::function<void()> cb,
             bool recurring)
    :m_recurring(recurring)
    ,m_ms(ms)
    ,m_cb(cb)
    ,m_manager(manager) {
    m_next =  fepoh::GetCurTimeMs() + m_ms;
}

Timer::Timer(uint64_t next)
    :m_next(next) {
}

bool Timer::cancel() {
    WriteLock lock(m_manager->m_mutex);
    if(m_cb) {
        m_cb = nullptr;
        auto it = m_manager->m_timers.find(shared_from_this());
        m_manager->m_timers.erase(it);
        return true;
    }
    return false;
}

bool Timer::refresh() {
    WriteLock lock(m_manager->m_mutex);
    if(!m_cb) {
        return false;
    }
    auto it = m_manager->m_timers.find(shared_from_this());
    if(it == m_manager->m_timers.end()) {
        return false;
    }
    m_manager->m_timers.erase(it);
    m_next =  fepoh::GetCurTimeMs() + m_ms;
    m_manager->m_timers.insert(shared_from_this());
    return true;
}

bool Timer::reset(uint64_t ms, bool from_now) {
    if(ms == m_ms && !from_now) {
        return true;
    }
    WriteLock lock(m_manager->m_mutex);
    if(!m_cb) {
        return false;
    }
    auto it = m_manager->m_timers.find(shared_from_this());
    if(it == m_manager->m_timers.end()) {
        return false;
    }
    m_manager->m_timers.erase(it);
    uint64_t start = 0;
    if(from_now) {
        start =  fepoh::GetCurTimeMs();
    } else {
        start = m_next - m_ms;
    }
    m_ms = ms;
    m_next = start + m_ms;
    m_manager->addTimer(shared_from_this(), lock);
    return true;

}

TimerManager::TimerManager() {
    m_previouseTime =  fepoh::GetCurTimeMs();
}

TimerManager::~TimerManager() {
}

Timer::ptr TimerManager::addTimer(uint64_t ms, std::function<void()> cb
                                  ,bool recurring) {
    Timer::ptr timer(new Timer(ms, this, cb, recurring));
    WriteLock lock(m_mutex);
    addTimer(timer, lock);
    return timer;
}

static void OnTimer(std::weak_ptr<void> weak_cond, std::function<void()> cb) {
    std::shared_ptr<void> tmp = weak_cond.lock();
    if(tmp) {
        cb();
    }
}

Timer::ptr TimerManager::addConditionTimer(uint64_t ms, std::function<void()> cb
                                    ,std::weak_ptr<void> weak_cond
                                    ,bool recurring) {
    return addTimer(ms, std::bind(&OnTimer, weak_cond, cb), recurring);
}

uint64_t TimerManager::getNextTimer() {
    ReadLock lock(m_mutex);
    m_tickled = false;
    if(m_timers.empty()) {
        return ~0ull;
    }

    const Timer::ptr& next = *m_timers.begin();
    uint64_t now_ms =  fepoh::GetCurTimeMs();
    if(now_ms >= next->m_next) {
        return 0;
    } else {
        return next->m_next - now_ms;
    }
}

void TimerManager::listExpiredCb(std::vector<std::function<void()> >& cbs) {
    uint64_t now_ms =  fepoh::GetCurTimeMs();
    std::vector<Timer::ptr> expired;
    {
        ReadLock lock(m_mutex);
        if(m_timers.empty()) {
            return;
        }
    }
    WriteLock lock(m_mutex);
    if(m_timers.empty()) {
        return;
    }
    bool rollover = detectClockRollover(now_ms);
    if(!rollover && ((*m_timers.begin())->m_next > now_ms)) {
        return;
    }

    Timer::ptr now_timer(new Timer(now_ms));
    auto it = rollover ? m_timers.end() : m_timers.lower_bound(now_timer);
    while(it != m_timers.end() && (*it)->m_next == now_ms) {
        ++it;
    }
    expired.insert(expired.begin(), m_timers.begin(), it);
    m_timers.erase(m_timers.begin(), it);
    cbs.reserve(expired.size());

    for(auto& timer : expired) {
        cbs.push_back(timer->m_cb);
        if(timer->m_recurring) {
            timer->m_next = now_ms + timer->m_ms;
            m_timers.insert(timer);
        } else {
            timer->m_cb = nullptr;
        }
    }
}

void TimerManager::addTimer(Timer::ptr val, WriteLock& lock) {
    auto it = m_timers.insert(val).first;
    bool at_front = (it == m_timers.begin()) && !m_tickled;
    if(at_front) {
        m_tickled = true;
    }
    lock.unlock();

    // if(at_front) {
    //     onTimerInsertedAtFront();
    // }
}

bool TimerManager::detectClockRollover(uint64_t now_ms) {
    bool rollover = false;
    if(now_ms < m_previouseTime &&
            now_ms < (m_previouseTime - 60 * 60 * 1000)) {
        rollover = true;
    }
    m_previouseTime = now_ms;
    return rollover;
}

bool TimerManager::hasTimer() {
    ReadLock lock(m_mutex);
    return !m_timers.empty();
}





// bool Timer::TimerCmp::operator()(Timer::ptr lhs,Timer::ptr rhs){
//     if(!lhs&&!rhs){
//         return false;
//     }
//     if(!lhs){
//         return true;
//     }
//     if(!rhs){
//         return false;
//     }
//     if(lhs->m_trigger < rhs->m_trigger){
//         return true;
//     }
//     if(rhs->m_trigger < lhs->m_trigger) {
//         return false;
//     }
//     return lhs.get() < rhs.get();
// }

// Timer::Timer(uint64_t ms,TimerManager* manager,std::function<void()> cb,bool recurring)
//         :m_cb(cb),m_ms(ms),m_recurring(recurring),m_manager(manager){
//     m_trigger = fepoh::GetCurTimeMs() + m_ms;
// }

// Timer::Timer(uint64_t trigger):m_trigger(trigger){

// }

// bool Timer::refresh(){
//     if(!m_manager || !m_cb){
//         return false;
//     }
//     MutexLock locker(m_manager->m_mutex);
//     auto it = m_manager->m_timers.find(shared_from_this());
//     if(it == m_manager->m_timers.end()){
//         return false;
//     }
//     m_manager->m_timers.erase(it);
//     m_trigger = fepoh::GetCurTimeMs() + m_ms;
//     m_manager->m_timers.insert(shared_from_this());
//     return true;
// }

// void Timer::cancel(){
//     if(!m_manager){
//         m_cb = nullptr;
//         return ;
//     }
//     MutexLock locker(m_manager->m_mutex);
//     auto it = m_manager->m_timers.find(shared_from_this());
//     if(it != m_manager->m_timers.end()){
//         m_manager->m_timers.erase(it);
//     }
//     m_cb = nullptr;
//     m_manager = nullptr;
// }


// bool Timer::isExpired(){
//     return (m_trigger <= fepoh::GetCurTimeMs())&&(!m_cb);
// }

// //已经过时的和无回调函数的timer不能插入
// void TimerManager::addTimer(Timer::ptr timer){
//     MutexLock lock(m_mutex);
//     m_timers.insert(timer);
// }

// Timer::ptr TimerManager::addTimer(uint64_t ms, std::function<void()> cb
//                                   ,bool recurring) {
//     Timer::ptr timer(new Timer(ms,this ,cb ,recurring));
//     addTimer(timer);
//     return timer;
// }

// static void OnTimer(std::weak_ptr<void> weak_cond, std::function<void()> cb) {
//     std::shared_ptr<void> tmp = weak_cond.lock();
//     if(tmp) {
//         cb();
//     }
// }

// Timer::ptr TimerManager::addConditionTimer(uint64_t ms, std::function<void()> cb
//         ,std::weak_ptr<void> weak_cond,bool recurring){
//     return addTimer(ms, std::bind(&OnTimer, weak_cond, cb), recurring);
// }


// void TimerManager::delTimer(Timer::ptr timer){
//     MutexLock lock(m_mutex);
//     auto it = m_timers.find(timer);
//     if(it != m_timers.end()){
//         (*it)->m_manager = nullptr;
//         m_timers.erase(it);
//     }
// }

// void TimerManager::clrTimer(){
//     MutexLock lock(m_mutex);
//     for(auto it= m_timers.begin();it!=m_timers.end();++it){
//         (*it)->m_manager = nullptr;
//         (*it)->m_cb = nullptr;
//     }
//     m_timers.clear();
// }

// uint64_t TimerManager::getMinTrigger(){
//     if(hasTimer()){
//         uint64_t minTime = (*(m_timers.begin()))->getTrigger();
//         return minTime > GetCurTimeMs() ? minTime - GetCurTimeMs() : 0 ;
//     }
//     return ~0ull;
// }


// void TimerManager::listAllExpired(std::list<std::function<void()>>& listCbs){
//     Mutex locker(m_mutex);
//     uint64_t cur = fepoh::GetCurTimeMs();
//     std::list<Timer::ptr> timers;
//     if(m_timers.empty()){
//         return ;
//     }
//     auto it = m_timers.begin();
//     for(;it != m_timers.end();++it){
//         if((*it)->getTrigger() < cur){
//             timers.push_back(*it);
//             continue;
//         }
//         break;
//     }
//     m_timers.erase(m_timers.begin(),it);
//     for(auto timer:timers){
//         listCbs.push_back(timer->m_cb);
//         if(!timer->m_recurring){
//             continue;
//         }
//         timer->m_trigger = cur + timer->m_ms;
//         m_timers.insert(timer);
//     }
// }

// bool TimerManager::hasTimer(){
//     MutexLock lock(m_mutex);
//     return !m_timers.empty();
// }

}//namespace
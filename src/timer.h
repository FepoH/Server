#pragma once

#include "thread/mutex.h"

#include <string>
#include <memory>
#include <stdint.h>
#include <vector>
#include <set>
#include <list>
#include <sys/time.h>
#include <functional>


namespace fepoh{

class TimerManager;
//毫秒级
class Timer : public std::enable_shared_from_this<Timer>{
public:
    typedef std::shared_ptr<Timer> ptr;
    friend class TimerManager;
    Timer(std::function<void()> cb,uint64_t ms,TimerManager* manager,bool recurring = false);
    //刷新定时器时间,from_now:用以前的定时开始时间,还是当前时间
    void refresh(uint64_t ms = ~0ull,bool from_now =true);
    //重置定时器
    void reset(std::function<void()> cb,uint64_t ms = ~0ull,bool from_now = true);
    //取消定时器
    void cancel();
    //是否已经过期
    bool isExpired();
public:
    Timer(uint64_t trigger);

    void recurring();

    void setRecurring(bool recurring) {m_recurring = recurring;}
    bool getRecurring() const {return m_recurring;}

    uint64_t getMs() const {return m_ms;}
    uint64_t getTrigger() const {return m_trigger;}
    std::function<void()> getCb() const {return m_cb;}

    struct TimerCmp{
        bool operator()(Timer::ptr lhs,Timer::ptr rhs);
    };

private:
    //在manager里会非常常用,故设置无锁
    void refreshNoLock(uint64_t ms = ~0ull,bool from_now = true);

private:

    bool m_recurring = false;       //执行完后时候再次加入定时器,默认不加入
    uint64_t m_ms = 0;              //定时毫秒数
    uint64_t m_trigger = 0;         //触发时间
    std::function<void()> m_cb;     //回调函数
    TimerManager* m_manager = nullptr;        //定时管理器
};



class TimerManager{
public:
    friend class Timer;
    typedef std::shared_ptr<TimerManager> ptr;

    TimerManager(){}
    ~TimerManager(){}
    //添加定时器
    void addTimer(Timer::ptr timer);
    //删除定时器
    void delTimer(Timer::ptr timer);
    //清理定时器
    void clrTimer();
    //返回所有的过时定时器的回调函数
    void listAllExpired(std::list<std::function<void()>>& listCbs);
    //是否有定时器
    bool hasTimer();
    //获取最小的定时器与当前时间的差距
    uint64_t getMinTrigger();
protected:
    Mutex m_mutex;      //锁
    std::set<Timer::ptr,Timer::TimerCmp> m_timers;
};

}//namespace
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

// class TimerManager;
// //毫秒级
// class Timer : public std::enable_shared_from_this<Timer>{
// public:
//     typedef std::shared_ptr<Timer> ptr;
//     friend class TimerManager;
//     Timer(uint64_t ms,TimerManager* manager,std::function<void()> cb,bool recurring = false);
//     Timer(uint64_t trigger);

//     //刷新定时器时间,from_now:用以前的定时开始时间,还是当前时间
//     bool refresh();
//    //取消定时器
//     void cancel();
//     //是否已经过期
//     bool isExpired();
// public:
//     void recurring();
//     void setRecurring(bool recurring) {m_recurring = recurring;}
//     bool getRecurring() const {return m_recurring;}

//     uint64_t getMs() const {return m_ms;}
//     uint64_t getTrigger() const {return m_trigger;}
//     std::function<void()> getCb() const {return m_cb;}

//     struct TimerCmp{
//         bool operator()(Timer::ptr lhs,Timer::ptr rhs);
//     };

// private:

//     bool m_recurring = false;       //执行完后时候再次加入定时器,默认不加入
//     uint64_t m_ms = 0;              //定时毫秒数
//     uint64_t m_trigger = 0;         //触发时间
//     std::function<void()> m_cb;     //回调函数
//     TimerManager* m_manager = nullptr;        //定时管理器
// };



// class TimerManager{
// public:
//     friend class Timer;
//     typedef std::shared_ptr<TimerManager> ptr;

//     TimerManager(){}
//     ~TimerManager(){}
//     //添加定时器
//     void addTimer(Timer::ptr timer);
//     //添加定时器
//     Timer::ptr addTimer(uint64_t ms, std::function<void()> cb
//                                   ,bool recurring);
//     //添加条件定时器
//     Timer::ptr addConditionTimer(uint64_t ms, std::function<void()> cb
//                     ,std::weak_ptr<void> weak_cond
//                     ,bool recurring = false);
//     //删除定时器
//     void delTimer(Timer::ptr timer);
//     //清理定时器
//     void clrTimer();
//     //返回所有的过时定时器的回调函数
//     void listAllExpired(std::list<std::function<void()>>& listCbs);
//     //是否有定时器
//     bool hasTimer();
//     //获取最小的定时器与当前时间的差距
//     uint64_t getMinTrigger();
// protected:
//     Mutex m_mutex;      //锁
//     std::set<Timer::ptr,Timer::TimerCmp> m_timers;
// };


class TimerManager;
/**
 * @brief 定时器
 */
class Timer : public std::enable_shared_from_this<Timer> {
friend class TimerManager;
public:
    /// 定时器的智能指针类型
    typedef std::shared_ptr<Timer> ptr;

    /**
     * @brief 取消定时器
     */
    bool cancel();

    /**
     * @brief 刷新设置定时器的执行时间
     */
    bool refresh();

    /**
     * @brief 重置定时器时间
     * @param[in] ms 定时器执行间隔时间(毫秒)
     * @param[in] from_now 是否从当前时间开始计算
     */
    bool reset(uint64_t ms, bool from_now);
public:
    /**
     * @brief 构造函数
     * @param[in] ms 定时器执行间隔时间
     * @param[in] cb 回调函数
     * @param[in] recurring 是否循环
     * @param[in] manager 定时器管理器
     */
    Timer(uint64_t ms, TimerManager* manager, std::function<void()> cb,
          bool recurring);
    /**
     * @brief 构造函数
     * @param[in] next 执行的时间戳(毫秒)
     */
    Timer(uint64_t next);
private:
    /// 是否循环定时器
    bool m_recurring = false;
    /// 执行周期
    uint64_t m_ms = 0;
    /// 精确的执行时间
    uint64_t m_next = 0;
    /// 回调函数
    std::function<void()> m_cb;
    /// 定时器管理器
    TimerManager* m_manager = nullptr;
private:
    /**
     * @brief 定时器比较仿函数
     */
    struct Comparator {
        /**
         * @brief 比较定时器的智能指针的大小(按执行时间排序)
         * @param[in] lhs 定时器智能指针
         * @param[in] rhs 定时器智能指针
         */
        bool operator()(const Timer::ptr& lhs, const Timer::ptr& rhs) const;
    };
};

/**
 * @brief 定时器管理器
 */
class TimerManager {
friend class Timer;
public:
    /// 读写锁类型
    typedef RWMutex RWMutexType;

    /**
     * @brief 构造函数
     */
    TimerManager();

    /**
     * @brief 析构函数
     */
    virtual ~TimerManager();

    /**
     * @brief 添加定时器
     * @param[in] ms 定时器执行间隔时间
     * @param[in] cb 定时器回调函数
     * @param[in] recurring 是否循环定时器
     */
    Timer::ptr addTimer(uint64_t ms, std::function<void()> cb
                        ,bool recurring = false);

    /**
     * @brief 添加条件定时器
     * @param[in] ms 定时器执行间隔时间
     * @param[in] cb 定时器回调函数
     * @param[in] weak_cond 条件
     * @param[in] recurring 是否循环
     */
    Timer::ptr addConditionTimer(uint64_t ms, std::function<void()> cb
                        ,std::weak_ptr<void> weak_cond
                        ,bool recurring = false);

    /**
     * @brief 到最近一个定时器执行的时间间隔(毫秒)
     */
    uint64_t getNextTimer();

    /**
     * @brief 获取需要执行的定时器的回调函数列表
     * @param[out] cbs 回调函数数组
     */
    void listExpiredCb(std::vector<std::function<void()> >& cbs);

    /**
     * @brief 是否有定时器
     */
    bool hasTimer();
protected:

    /**
     * @brief 当有新的定时器插入到定时器的首部,执行该函数
     */
    // virtual void onTimerInsertedAtFront() = 0;

    /**
     * @brief 将定时器添加到管理器中
     */
    void addTimer(Timer::ptr val, WriteLock& lock);
private:
    /**
     * @brief 检测服务器时间是否被调后了
     */
    bool detectClockRollover(uint64_t now_ms);
protected:
    /// 定时器集合
    std::set<Timer::ptr, Timer::Comparator> m_timers;
private:
    /// Mutex
    RWMutexType m_mutex;
    
    /// 是否触发onTimerInsertedAtFront
    bool m_tickled = false;
    /// 上次执行时间
    uint64_t m_previouseTime = 0;
};


}//namespace
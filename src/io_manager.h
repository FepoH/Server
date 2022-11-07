#pragma once
#include "scheduler_manager.h"
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

    enum Event{
        NONE = 0x0,     //无事件
        READ = 0x1,     //读事件
        WRITE = 0x4     //写事件
    };
private:
    struct FdContext{
        struct EventContext{
            SchedulerManager* scheduler = nullptr;
            Fiber::ptr fiber;
            std::function<void()> cb;
        };
        EventContext& getContext(Event ev);
        void resetContext();
        void triggerContext(Event ev);

        EventContext read;
        EventContext write;
        Event events = Event::NONE;
        int fd = 0;
        Mutex mutex;
    };

public:
    IOManager(const std::string& name="fepoh",size_t threadCount=1,bool use_caller=true);
    ~IOManager();

    int addEvent(int fd,Event event,std::function<void()> cb=nullptr);
    bool delEvent(int fd,Event event);
    bool cancelEvent(int fd,Event event);
    bool cancelAll(int fd);
public:
    static IOManager* GetThis();
protected:
    void notice() override;
    bool isStop() override;
    void idle() override;
    //void timerInsertOnFront()override;
    //bool stopping(uint64_t val);
    void contextResize(size_t size);
private:
    int m_epollfd;          //epoll fd
    int m_pidefd[2];        //管道
    RWMutex m_mutex;        //锁
    std::vector<FdContext*> m_fdContexts;   //任务
    //std::atomic<size_t> m_pendingEventCount = {0};
};

}//namespace
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

    enum Event{
        NONE = 0x0,     //无事件
        READ = 0x1,     //读事件
        WRITE = 0x4     //写事件
    };
private:
    //文件描述符上下文
    struct FdContext{
        //事件内容
        struct EventContext{
            //事件所属调度器
            ScheduleManager* scheduler = nullptr;
            //协程,即事件回调
            Fiber::ptr fiber;
        };
        //获取事件回调
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
    IOManager(const std::string& name="fepoh",size_t threadCount=1,bool use_caller=true);
    ~IOManager();
    //添加事件
    int addEvent(int fd,Event event,std::function<void()> cb=nullptr);
    //删除事件
    int delEvent(int fd,Event event);
    //取消事件,会触发事件回调
    int cancelEvent(int fd,Event event);
    //取消全部事件,指的的文件描述符上的事件
    int cancelAll(int fd);
public:
    static IOManager* GetThis();
protected:
    //唤醒线程
    void notice() override;
    //是否停止
    bool isStop() override;
    //空闲,主要内容在此中调用epoll_wait
    void idle() override;
    //重置大小
    void contextResize(size_t size);
private:
    int m_epollfd;          //epoll fd
    int m_pipefd[2];        //管道
    RWMutex m_mutex;        //锁
    std::vector<FdContext*> m_fdContexts;   //任务
    std::atomic<size_t> m_pendingEventCount = {0};  //等待事件数量,即在队列中的事件数
};

}//namespace
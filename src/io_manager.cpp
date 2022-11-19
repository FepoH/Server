#include "io_manager.h"
#include "parameter.h"
#include "macro.h"
#include "timer.h"

#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <vector>

namespace fepoh{

static Logger::ptr s_log_system = FEPOH_LOG_NAME("system");

IOManager::FdContext::EventContext& IOManager::FdContext::getContext(Event ev){
    switch(ev){
        case Event::READ:
            return read;
            break;
        case Event::WRITE:
            return write;
            break;
        default:
            FEPOH_ASSERT1(false,"IOManager::FdContext::getContext error.ev = " 
                    + std::to_string((int)ev) + "fd = "std::to_string(fd));
    }
}

void IOManager::FdContext::resetContext(EventContext& ctx){
    ctx.scheduler = nullptr;
    ctx.fiber.reset();
}

void IOManager::FdContext::triggerContext(Event ev){
    FEPOH_ASSERT(ev);
    ev = (Event)(ev&events);
    events = (Event)(events & (~ev));
    if(!ev){
        return ;
    }
    EventContext& ctx = getContext(ev);
    ctx.scheduler->schedule(ctx.fiber);
    ctx.scheduler = nullptr;
    ctx.fiber = nullptr;
}

IOManager::IOManager(const std::string& name,size_t threadCount,bool use_caller)
        :ScheduleManager(name,threadCount,use_caller){
    int rt = 0;
    m_epollfd = epoll_create(5000);
    FEPOH_ASSERT(m_epollfd != -1);
    rt = pipe(m_pipefd);
    FEPOH_ASSERT1(rt != -1,"IOManager pipefd create error");
    contextResize(64);
    epoll_event event;
    memset(&event,0,sizeof(epoll_event));
    event.data.fd = m_pipefd[0];
    //读事件和边缘触发
    event.events = EPOLLIN|EPOLLET;
    
    rt = epoll_ctl(m_epollfd,EPOLL_CTL_ADD,m_pipefd[0],&event);
    FEPOH_ASSERT1(rt != -1,"IOManager epoll control error");
}

IOManager::~IOManager(){
    if(!m_isStop){
        stop();
    }
    close(m_epollfd);
    close(m_pipefd[0]);
    close(m_pipefd[1]);

    for(size_t i = 0; i < m_fdContexts.size(); ++i) {
        if(m_fdContexts[i]) {
            delete m_fdContexts[i];
        }
    }
}

int IOManager::addEvent(int fd,Event event,std::function<void()> cb ){
    if(fd < 0||event == NONE){
        return -1;
    }
    FdContext* fd_ctx = nullptr;
    ReadLock lock(m_mutex);
    if(fd < m_fdContexts.size()){
        fd_ctx = m_fdContexts[fd];
        lock.unlock();
    }else{
        lock.unlock();
        WriteLock lock1(m_mutex);
        contextResize(fd*1.5);
        fd_ctx = m_fdContexts[fd];
    }

    MutexLock lock2(fd_ctx->mutex);
    FEPOH_ASSERT1(!(fd_ctx->events&event),"IOManager::addEvent error.fd_ctx->events = " 
                + (int)fd_ctx->events + ",event = " + event);
    int op = fd_ctx->events ? EPOLL_CTL_MOD : EPOLL_CTL_ADD;
    epoll_event ep_event;
    ep_event.events = EPOLLET | fd_ctx->events | event;

    ep_event.data.ptr = fd_ctx;
    int rt = epoll_ctl(m_epollfd,op,fd,&ep_event);
    if(rt){
        FEPOH_LOG_ERROR(s_log_system) <<"IOManager::addEvent error.ep_ctl error:m_epollfd = " << m_epollfd
                    << ",events = " << ep_event.events << ",fd = " << fd;
        return -1;
    }
    ++m_pendingEventCount;
    fd_ctx->events = (Event)(fd_ctx->events | event);

    FdContext::EventContext& event_ctx = fd_ctx->getContext(event);
    FEPOH_ASSERT(!event_ctx.scheduler
                && !event_ctx.fiber);

    event_ctx.scheduler = ScheduleManager::GetThis();
    if(cb) {
        Fiber::ptr f(new Fiber(cb));
        event_ctx.fiber = f;
    }else {
        event_ctx.fiber = Fiber::GetThis();
        FEPOH_ASSERT1(event_ctx.fiber->getState() == Fiber::EXEC
                      ,"state=" << event_ctx.fiber->getState());
    }
    return 0;
}

int IOManager::delEvent(int fd,Event event){
    ReadLock lock1(m_mutex);
    if(fd>=m_fdContexts.size() || fd < 0){
        FEPOH_LOG_WARN(s_log_system)<<"IOManager::delEvent error.fd > m_fdContexts.size(),fd = "
                    << fd << "m_fdContexts.size() = " << m_fdContexts.size();
        return -1;
    }
    lock1.unlock();
    FdContext* fd_ctx = nullptr;
    fd_ctx = m_fdContexts[fd];
    MutexLock lock(fd_ctx->mutex);
    FEPOH_ASSERT(fd_ctx->events&event);
    epoll_event ep_event;
    Event new_event = (Event)(fd_ctx->events&(~event));
    int op = new_event?EPOLL_CTL_MOD : EPOLL_CTL_DEL;
    ep_event.events = EPOLLET | new_event;
    ep_event.data.ptr = fd_ctx;
    int rt = epoll_ctl(m_epollfd,op,fd,&ep_event);
    if(rt){
        FEPOH_LOG_ERROR(s_log_system) <<"IOManager::delEvent error.ep_ctl error:m_epollfd = " << m_epollfd
                    << ",events = " << ep_event.events << ",fd = " << fd;
    }
    --m_pendingEventCount;
    fd_ctx->events = new_event;
    FdContext::EventContext& event_ctx = fd_ctx->getContext(event);
    fd_ctx->resetContext(event_ctx);
    return 0;
}

int IOManager::cancelEvent(int fd,Event event){
    ReadLock lock(m_mutex);
    if(fd>=m_fdContexts.size() || fd < 0){
        FEPOH_LOG_WARN(s_log_system)<<"IOManager::delEvent error.fd > m_fdContexts.size(),fd = "
                    << fd << "m_fdContexts.size() = " << m_fdContexts.size();
        return -1;
    }
    FdContext* fd_ctx = nullptr;
    fd_ctx = m_fdContexts[fd];
    lock.unlock();
    
    MutexLock lock1(fd_ctx->mutex);

    FEPOH_ASSERT(fd_ctx->events&event);
    epoll_event ep_event;
    Event new_event = (Event)(fd_ctx->events&(~event));
    int op = new_event?EPOLL_CTL_MOD : EPOLL_CTL_DEL;
    ep_event.events = EPOLLET | new_event;
    ep_event.data.ptr = fd_ctx;
    int rt = epoll_ctl(m_epollfd,op,fd,&ep_event);
    if(rt){
        FEPOH_LOG_ERROR(s_log_system) <<"IOManager::cancelEvent error.ep_ctl error:m_epollfd = " + m_epollfd
                    << ",events = " << ep_event.events << ",fd = " << fd;
    }
    --m_pendingEventCount;
    fd_ctx->events = new_event;
    FdContext::EventContext& event_ctx = fd_ctx->getContext(event);
    fd_ctx->triggerContext(event);
    return 0;
}

int IOManager::cancelAll(int fd){
    ReadLock lock1(m_mutex);
    if(fd>=m_fdContexts.size() || fd < 0){
        FEPOH_LOG_ERROR(s_log_system)<<"IOManager::delEvent error.fd > m_fdContexts.size(),fd = "
                    << fd << "m_fdContexts.size() = " << m_fdContexts.size();
        return -1;
    }
    FdContext* fd_ctx = nullptr;
    fd_ctx = m_fdContexts[fd];
    lock1.unlock();
    
    MutexLock lock(fd_ctx->mutex);
    //FEPOH_ASSERT(fd_ctx->events);
    epoll_event ep_event;
    ep_event.events = 0;
    ep_event.data.ptr = fd_ctx;
    int rt = epoll_ctl(m_epollfd,EPOLL_CTL_DEL,fd,&ep_event);
    if(rt){
        FEPOH_LOG_ERROR(s_log_system) <<"IOManager::cancelAll error.ep_ctl error:m_epollfd = " + m_epollfd
                    << ",events = " << ep_event.events << ",fd = " << fd;
    }
    if(fd_ctx->events & READ) {
        fd_ctx->triggerContext(READ);
        --m_pendingEventCount;
    }
    if(fd_ctx->events & WRITE) {
        fd_ctx->triggerContext(WRITE);
        --m_pendingEventCount;
    }

    FEPOH_ASSERT(fd_ctx->events == 0);
    return true;
}

IOManager* IOManager::GetThis(){
    return dynamic_cast<IOManager*>(ScheduleManager::GetThis());
}

void IOManager::notice() {
    if(!hasIdleThread()){
        return ;
    }
    int rt = write(m_pipefd[1],"[NOTICE]",8);
    FEPOH_ASSERT(rt == 8);
    return ;
}
bool IOManager::isStop() {
    return ScheduleManager::isStop()
         &&m_pendingEventCount == 0
         &&m_timers.empty();
}
void IOManager::idle() {
    FEPOH_LOG_DEBUG(s_log_system) << "IOManager::Idle Run.";
    //最大事件处理数量
    const uint64_t MAX_EVENTS = 1024;
    epoll_event* events = new epoll_event[MAX_EVENTS];
    std::shared_ptr<epoll_event> shared_events(events,[](epoll_event* ptr){
        delete[] ptr;
    });

    while(true){
        if(isStop()){
            FEPOH_LOG_DEBUG(s_log_system) <<"idle break";
            break;
        }
        int rt = 0;
        do{
            //超时时间
            static const int MAX_TIMEOUT = 3000;
            uint64_t timeout = getNextTimer();
            if(timeout > MAX_TIMEOUT){
                timeout = MAX_TIMEOUT;
            }
            rt = epoll_wait(m_epollfd,events,MAX_EVENTS,(int)timeout);
            if(rt < 0 && errno == EINTR){

            }else{
                break;
            }
        }while(true);
        //已超时事件加入队列
        std::vector<std::function<void()>> listCbs;
        listExpiredCb(listCbs);
        if(!listCbs.empty()){
            this->schedule(listCbs.begin(),listCbs.end());
            listCbs.clear();
        }
        for(int i = 0; i < rt; ++i){
            epoll_event& event = events[i];
            if(event.data.fd == m_pipefd[0]){
                uint8_t dummy[256];
                while(read(m_pipefd[0],dummy,sizeof(dummy)) > 0);
                continue;
            }

            FdContext* fd_ctx = (FdContext*)event.data.ptr;
            MutexLock lock(fd_ctx->mutex);
            //出现连接被对方关闭或挂起,事件全部触发
            if(event.events & (EPOLLERR | EPOLLHUP)){
                event.events |=(EPOLLIN | EPOLLOUT) & fd_ctx->events;
            }
            int real_events = NONE;
            if(event.events & EPOLLIN){
                real_events |= READ;
            }
            if(event.events & EPOLLOUT){
                real_events |= WRITE;
            }

            if((fd_ctx->events & real_events) == NONE){
                continue;
            }

            int left_events = (fd_ctx->events & ~real_events);
            int op = left_events ? EPOLL_CTL_MOD : EPOLL_CTL_DEL;
            event.events = EPOLLET | left_events;

            int rt2 = epoll_ctl(m_epollfd, op, fd_ctx->fd, &event);
            if(rt2){
                FEPOH_LOG_ERROR(s_log_system) <<"IOManager::idle error.fd = " <<fd_ctx->fd << ",event = " 
                            <<event.events <<",m_epollfd = " <<m_epollfd;
                continue;
            }

            if(real_events & READ) {
                fd_ctx->triggerContext(READ);
                --m_pendingEventCount;
            }
            if(real_events & WRITE){
                fd_ctx->triggerContext(WRITE);
                --m_pendingEventCount;
            }
        }
        Fiber::ptr cur = Fiber::GetThis();
        auto raw_ptr = cur.get();
        cur.reset();
        raw_ptr->swapOutHold();
    }

}

void IOManager::contextResize(size_t size){
    m_fdContexts.resize(size);
    for(int i=0;i<size;++i){
        if(!m_fdContexts[i]){
            m_fdContexts[i] = new FdContext();
            m_fdContexts[i]->fd = i;
        }
    }
}


}//namespace
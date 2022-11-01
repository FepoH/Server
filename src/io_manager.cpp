#include "io_manager.h"
#include "parameter.h"

namespace fepoh{

EventContext& IOManager::FdContext::getContext(Event ev){
    switch(ev){
        case Event::READ:
            return read;
            break;
        case Event::WRITE:
            return write;
            break;
        default:
            FEPOH_ASSERT1(false,"IOManager::FdContext::getContext error.ev = " + std::to_string((int)ev));
    }
}

void IOManager::FdContext::resetContext(EventContext& ctx){
    ctx.schedule = nullptr;
    ctx.fiber = nullptr;
    ctx.cb = nullptr;
}

void IOManager::FdContext::triggerContext(Event ev){
    FEPOH_ASSERT(ev);
    events = (Event)(event&events);
    EventContext& ctx = getContext(events);
    if(ctx.cb){
        ctx.schedule(&ctx.cb);
    }else{
        ctx.schedule(&ctx.fiber);
    }
    ctx.schedule = nullptr;
}

IOManager::IOManager(const std::string& name,size_t threadCount,bool use_caller)
        :ScheduleManager(name,threadCount,use_caller){
    setThis();
    int rt = 0;
    m_epollfd = epoll_create(5000);
    FEPOH_ASSERT1(m_epollfd == -1)
    rt = pipe(m_pipefd);
    FEPOH_ASSERT1(rt == -1,"IOManager pipefd create error");

    epoll_event event;
    memset(&event,0,sizeof(epoll_event));
    event.data.fd = m_pipefd[0];
    //读事件和边缘触发
    event.events = EPOLLIN|EPOLLET;
    rt = epoll_ctl(m_epollfd,EPOLL_CTL_ADD,m_pipefd[0],&event);
    FEPOH_ASSERT1(ret != -1,"IOManager epoll control error");
}
IOManager::~IOManager();

int IOManager::addEvent(int fd,Event event,std::function<void()> cb=nullptr);
bool IOManager::delEvent(int fd,Event event);
bool IOManager::cancelEvent(int fd,Event event);
bool IOManager::cancelAll(int fd);

static IOManager* IOManager::GetThis();

void IOManager::notice() override;
bool IOManager::isStop() override;
void IOManager::idle() override;
//void timerInsertOnFront()override;
//bool stopping(uint64_t val);
void IOManager::contextResize(size_t size){
    WriteLock locker(m_mutex);
    m_fdContexts.resize(size);
    for(int i=0;i<size;++i){
        m_fdContexts[i] = new FdContext();
    }
}


}//namespace
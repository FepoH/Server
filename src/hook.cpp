/*
 * @Author: fepo_h
 * @Date: 2022-11-20 18:09:14
 * @LastEditors: fepo_h
 * @LastEditTime: 2022-11-21 18:09:41
 * @FilePath: /fepoh/workspace/fepoh_server/src/hook.cpp
 * @Description: 
 * 
 * Copyright (c) 2022 by FepoH Fepo_H@163.com, All Rights Reserved. 
 * @version: V1.0.0
 * @Mailbox: Fepo_H@163.com
 * @Descripttion: 
 */
#include "hook.h"
#include "fiber.h"
#include "io_manager.h"
#include "timer.h"
#include "fd_manager.h"
#include "config.h"
#include <dlfcn.h>
#include <atomic>

static fepoh::Logger::ptr s_log_system = FEPOH_LOG_NAME("system");

namespace fepoh{

static thread_local bool t_hook_enable = false;

static fepoh::ConfigVar<int>::ptr s_tcp_connect_timeout =
        fepoh::Config::Lookup<int>(5000 ,"tcp.connect.timeout","tcp connect timeout");

#define HOOK_FUN(XX) \
    XX(sleep) \
    XX(usleep) \
    XX(nanosleep) \
    XX(socket) \
    XX(connect) \
    XX(accept) \
    XX(read) \
    XX(readv) \
    XX(recv) \
    XX(recvfrom) \
    XX(recvmsg) \
    XX(write) \
    XX(writev) \
    XX(send) \
    XX(sendto) \
    XX(sendmsg) \
    XX(close) \
    XX(fcntl) \
    XX(ioctl) \
    XX(getsockopt) \
    XX(setsockopt)

void hook_init(){
    static bool is_init = false;
    if(is_init){
        return ;
    }
//对XX_f进行初始化
#define XX(name) name ## _f = (name ## _fun)dlsym(RTLD_NEXT, #name);
    HOOK_FUN(XX);
#undef XX
    is_init = true;
}

static int s_connect_timeout = -1;

struct __HookInit__{
    __HookInit__(){
        hook_init();
        s_connect_timeout = s_tcp_connect_timeout->getValue();
        s_tcp_connect_timeout->addListener([](const int& old_value,const int& new_value){
            s_connect_timeout = new_value;
        });
    }
};

static __HookInit__  _hook_init_;

bool is_hook_enable(){
    return t_hook_enable;
}

void set_hook_enable(bool flag){
    t_hook_enable = flag;
}

}

struct timer_info{
    int canceled = 0;
};

static std::atomic<int> counta = {0};

template<typename OriginFun,typename ... Args>
static ssize_t do_io(int fd,OriginFun fun,const char* hook_fun_name
        ,uint32_t event, int timeout_so , Args&& ... args) {
    //没有被hook
    ++counta;
    std::cout << "111-" << counta << std::endl;
    if(!fepoh::t_hook_enable){
        return fun(fd, std::forward<Args>(args)...);
    }
    std::cout << "222-" << counta <<std::endl;
    fepoh::FdCtx::ptr fd_ctx = fepoh::FdManager::GetInstance()->get(fd);
    std::cout << "333-" << counta <<std::endl;
    //没有获取到fd_ctx
    if(!fd_ctx){
        return fun(fd, std::forward<Args>(args)...);
    }
    std::cout << "444-" << counta <<std::endl;
    //文件被关闭
    if(fd_ctx->isClose()){
        errno = EBADF;
        return -1;
    }
    std::cout << "555-" <<std::endl;
    //不是socket或者被用户设置了nonblock
    //用户已经设置非阻塞,就已经不会阻塞,直接返回即可
    if(!fd_ctx->isSocket() || fd_ctx->getUserNonblock()){
        return fun(fd, std::forward<Args>(args)...);
    }
    //获取超时时间
    uint64_t timeout = fd_ctx->getTimeout(timeout_so);
    std::shared_ptr<timer_info> tinfo(new timer_info());
retry:
    //先尝试读数据
    ssize_t n = fun(fd,std::forward<Args>(args)...);
    //当出现中断错误,继续重试
    while(n == -1 && errno == EINTR){
        return fun(fd, std::forward<Args>(args)...);
    }
    //当出现EAGAIN(非阻塞时,无数据可读,系统提示再试一次)
    if(n == -1 && errno == EAGAIN){
        fepoh::IOManager* iom = fepoh::IOManager::GetThis();
        fepoh::Timer::ptr timer;
        //条件定时器条件
        std::weak_ptr<timer_info> winfo(tinfo);
        //设置过超时时间
        if(timeout != (uint64_t)-1) {
            //添加条件定时器
            timer = iom->addConditionTimer(timeout,[winfo,fd ,iom , event](){
                //唤醒后,1.查看条件是否被取消,如果取消直接返回
                //2.若没有取消,设置为超时错误,取消事件
                auto t = winfo.lock();
                if(!t || t->canceled){
                    return ;
                }
                t->canceled = ETIMEDOUT;
                iom->cancelEvent(fd,(fepoh::IOManager::Event)(event));
            },winfo);
        }
        //添加事件
        int rt = iom->addEvent(fd,(fepoh::IOManager::Event)(event));
        if(rt){
            //添加事件错误,直接log并返回
            FEPOH_LOG_DEBUG(s_log_system) << hook_fun_name << " addEvent("
                <<fd <<", " <<event <<")";
            if(timer) {
                timer->cancel();
            }
            return -1;
        }else{
            //添加事件成功,陷入Hold状态
            fepoh::Fiber::GetThis()->swapOutHold();
            //从Hold状态返回,一下两种情况会返回
            //1.从定时器返回,取消的时候会触发事件
            //2.事件触发返回
            if(timer){
                //如果定时器存在,取消定时器,说明没有超时
                timer->cancel();
            }
            //超时了,直接返回
            if(tinfo->canceled){
                errno = tinfo->canceled;
                return -1;
            }
            //重新去读数据
            goto retry;
        }
    }
    return n;
}

extern "C"{
//定义XX_f
#define XX(name) name ## _fun  name ## _f = nullptr;
    HOOK_FUN(XX)
#undef XX
}

unsigned int sleep(unsigned int seconds){
    if(!fepoh::t_hook_enable){
        return sleep_f(seconds);
    }
    fepoh::Fiber::ptr fiber = fepoh::Fiber::GetThis();
    fepoh::IOManager* iom = fepoh::IOManager::GetThis();
    
    //这个也可以用
    iom->addTimer(seconds * 1000,[iom,fiber](){
        iom->schedule(fiber);
    },false);

    //::*,指向数据成员的指针类型
    // iom->addTimer(fepoh::Timer::ptr(new fepoh::Timer(seconds * 1000,
    //         std::bind((void(fepoh::ScheduleManager::*)(fepoh::Fiber::ptr))&fepoh::IOManager::schedule,iom,fiber),false)));
    fiber->swapOutHold();
    return 0;
}

int usleep(useconds_t usec){
    if(!fepoh::t_hook_enable){
        return usleep_f(usec);
    }
    fepoh::Fiber::ptr fiber = fepoh::Fiber::GetThis();
    fepoh::IOManager* iom = fepoh::IOManager::GetThis();
    iom->addTimer(usec / 1000,[iom,fiber](){
        iom->schedule(fiber);
    },false);
    fiber->swapOutHold();
    return 0;
}


int nanosleep(const struct timespec *rqtp, struct timespec *rmtp){
    if(!fepoh::t_hook_enable){
        return nanosleep_f(rqtp,rmtp);
    }
    uint64_t timeout = rqtp->tv_sec * 1000 + rmtp->tv_nsec / 1000 / 1000;
    fepoh::Fiber::ptr fiber = fepoh::Fiber::GetThis();
    fepoh::IOManager* iom = fepoh::IOManager::GetThis();
    iom->addTimer(timeout,[iom,fiber](){
        iom->schedule(fiber);
    },false);
    fiber->swapOutHold();
    return 0;
}


int socket(int domain, int type, int protocol){
    if(!fepoh::t_hook_enable){
        return socket_f(domain,type,protocol);
    }
    int fd = socket_f(domain,type,protocol);
    if(fd == -1){
        return fd;
    }
    fepoh::FdManager::GetInstance()->get(fd,true);
    return fd;
}


int connect_with_timeout(int fd, const struct sockaddr* addr, socklen_t addrlen, uint64_t timeout_ms) {
    if(!fepoh::t_hook_enable) {
        return connect_f(fd, addr, addrlen);
    }
    fepoh::FdCtx::ptr ctx = fepoh::FdManager::GetInstance()->get(fd);
    if(!ctx || ctx->isClose()) {
        errno = EBADF;
        return -1;
    }

    if(!ctx->isSocket()) {
        return connect_f(fd, addr, addrlen);
    }

    if(ctx->getUserNonblock()) {
        return connect_f(fd, addr, addrlen);
    }

    int n = connect_f(fd, addr, addrlen);
    if(n == 0) {
        return 0;
    } else if(n != -1 || errno != EINPROGRESS) {
        return n;
    }

    fepoh::IOManager* iom = fepoh::IOManager::GetThis();
    fepoh::Timer::ptr timer;
    std::shared_ptr<timer_info> tinfo(new timer_info);
    std::weak_ptr<timer_info> winfo(tinfo);

    if(timeout_ms != (uint64_t)-1) {
        timer = iom->addConditionTimer(timeout_ms, [winfo, fd, iom]() {
                auto t = winfo.lock();
                if(!t || t->canceled) {
                    return;
                }
                t->canceled = ETIMEDOUT;
                iom->cancelEvent(fd, fepoh::IOManager::WRITE);
        }, winfo);
    }

    int rt = iom->addEvent(fd, fepoh::IOManager::WRITE);
    if(rt == 0) {
        fepoh::Fiber::GetThis()->swapOutHold();
        if(timer) {
            timer->cancel();
        }
        if(tinfo->canceled) {
            errno = tinfo->canceled;
            return -1;
        }
    } else {
        if(timer) {
            timer->cancel();
        }
        FEPOH_LOG_ERROR(s_log_system) << "connect addEvent(" << fd << ", WRITE) error";
    }

    int error = 0;
    socklen_t len = sizeof(int);
    if(-1 == getsockopt(fd, SOL_SOCKET, SO_ERROR, &error, &len)) {
        return -1;
    }
    if(!error) {
        return 0;
    } else {
        errno = error;
        return -1;
    }
}

int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
    return connect_with_timeout(sockfd, addr, addrlen, fepoh::s_connect_timeout);
}
int accept(int s, struct sockaddr *addr, socklen_t *addrlen){
    int fd = do_io(s ,accept_f ,"accept" , fepoh::IOManager::READ ,SO_RCVTIMEO ,addr ,addrlen);
    if(fd >= 0){
        fepoh::FdManager::GetInstance()->get(fd ,true);
    }
    return fd;
}

ssize_t read(int fd, void *buf, size_t count){
    return do_io(fd ,read_f ,"read" ,fepoh::IOManager::READ ,SO_RCVTIMEO ,buf ,count);
}

ssize_t readv(int fd, const struct iovec *iov, int iovcnt){
    return do_io(fd ,readv_f ,"readv" ,fepoh::IOManager::READ ,SO_RCVTIMEO ,iov ,iovcnt);
}

ssize_t recv(int sockfd, void *buf, size_t len, int flags){
    return do_io(sockfd ,recv_f ,"recv" ,fepoh::IOManager::READ ,SO_RCVTIMEO ,buf ,len ,flags);
}

ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags, struct sockaddr *src_addr, socklen_t *addrlen){
    return do_io(sockfd ,recvfrom_f ,"recvfrom" ,fepoh::IOManager::READ ,SO_RCVTIMEO ,buf ,len ,flags ,src_addr ,addrlen);
}

ssize_t recvmsg(int sockfd, struct msghdr *msg, int flags){
    return do_io(sockfd ,recvmsg_f ,"recvmsg" ,fepoh::IOManager::READ ,SO_RCVTIMEO ,msg ,flags);
}

ssize_t write(int fd, const void *buf, size_t count){
    return do_io(fd ,write_f ,"write" ,fepoh::IOManager::WRITE ,SO_SNDTIMEO ,buf ,count);
}

ssize_t writev(int fd, const struct iovec *iov, int iovcnt){
    return do_io(fd ,writev_f ,"writev" ,fepoh::IOManager::WRITE ,SO_SNDTIMEO ,iov ,iovcnt);
}

ssize_t send(int s, const void *msg, size_t len, int flags){
    return do_io(s ,send_f ,"send" ,fepoh::IOManager::WRITE ,SO_SNDTIMEO ,msg ,len ,flags);
}

ssize_t sendto(int s, const void *msg, size_t len, int flags, const struct sockaddr *to, socklen_t tolen){
    return do_io(s ,sendto_f ,"sendto" ,fepoh::IOManager::WRITE ,SO_SNDTIMEO ,msg ,len ,flags ,to ,tolen);
}

ssize_t sendmsg(int s, const struct msghdr *msg, int flags){
    return do_io(s ,sendmsg_f ,"sendmsg" ,fepoh::IOManager::WRITE ,SO_SNDTIMEO ,msg ,flags);
}

int close(int fd){
    if(!fepoh::t_hook_enable){
        return close_f(fd);
    }
    fepoh::FdCtx::ptr fd_ctx = fepoh::FdManager::GetInstance()->get(fd);
    if(fd_ctx){
        auto iom = fepoh::IOManager::GetThis();
        if(iom){
            iom->cancelAll(fd);
        }
        fepoh::FdManager::GetInstance()->del(fd);
    }
    return close_f(fd);
}


int fcntl(int fd, int cmd, ... /* arg */ ) {
    va_list va;
    va_start(va, cmd);
    switch(cmd) {
        case F_SETFL:
            {
                int arg = va_arg(va, int);
                va_end(va);
                fepoh::FdCtx::ptr ctx = fepoh::FdManager::GetInstance()->get(fd);
                if(!ctx || ctx->isClose() || !ctx->isSocket()) {
                    return fcntl_f(fd, cmd, arg);
                }
                ctx->setUserNonblock(arg & O_NONBLOCK);
                if(ctx->getSysNonblock()) {
                    arg |= O_NONBLOCK;
                } else {
                    arg &= ~O_NONBLOCK;
                }
                return fcntl_f(fd, cmd, arg);
            }
            break;
        case F_GETFL:
            {
                va_end(va);
                int arg = fcntl_f(fd, cmd);
                fepoh::FdCtx::ptr ctx = fepoh::FdManager::GetInstance()->get(fd);
                if(!ctx || ctx->isClose() || !ctx->isSocket()) {
                    return arg;
                }
                if(ctx->getUserNonblock()) {
                    return arg | O_NONBLOCK;
                } else {
                    return arg & ~O_NONBLOCK;
                }
            }
            break;
        case F_DUPFD:
        case F_DUPFD_CLOEXEC:
        case F_SETFD:
        case F_SETOWN:
        case F_SETSIG:
        case F_SETLEASE:
        case F_NOTIFY:
#ifdef F_SETPIPE_SZ
        case F_SETPIPE_SZ:
#endif
            {
                int arg = va_arg(va, int);
                va_end(va);
                return fcntl_f(fd, cmd, arg); 
            }
            break;
        case F_GETFD:
        case F_GETOWN:
        case F_GETSIG:
        case F_GETLEASE:
#ifdef F_GETPIPE_SZ
        case F_GETPIPE_SZ:
#endif
            {
                va_end(va);
                return fcntl_f(fd, cmd);
            }
            break;
        case F_SETLK:
        case F_SETLKW:
        case F_GETLK:
            {
                struct flock* arg = va_arg(va, struct flock*);
                va_end(va);
                return fcntl_f(fd, cmd, arg);
            }
            break;
        case F_GETOWN_EX:
        case F_SETOWN_EX:
            {
                struct f_owner_exlock* arg = va_arg(va, struct f_owner_exlock*);
                va_end(va);
                return fcntl_f(fd, cmd, arg);
            }
            break;
        default:
            va_end(va);
            return fcntl_f(fd, cmd);
    }
}

int ioctl(int d, unsigned long int request, ...) {
    va_list va;
    va_start(va, request);
    void* arg = va_arg(va, void*);
    va_end(va);

    if(FIONBIO == request) {
        bool user_nonblock = !!*(int*)arg;
        fepoh::FdCtx::ptr ctx = fepoh::FdManager::GetInstance()->get(d);
        if(!ctx || ctx->isClose() || !ctx->isSocket()) {
            return ioctl_f(d, request, arg);
        }
        ctx->setUserNonblock(user_nonblock);
    }
    return ioctl_f(d, request, arg);
}

int getsockopt(int sockfd, int level, int optname, void *optval, socklen_t *optlen) {
    return getsockopt_f(sockfd, level, optname, optval, optlen);
}

int setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen) {
    if(!fepoh::t_hook_enable) {
        return setsockopt_f(sockfd, level, optname, optval, optlen);
    }
    if(level == SOL_SOCKET) {
        if(optname == SO_RCVTIMEO || optname == SO_SNDTIMEO) {
            fepoh::FdCtx::ptr ctx = fepoh::FdManager::GetInstance()->get(sockfd);
            if(ctx) {
                const timeval* v = (const timeval*)optval;
                ctx->setTimeout(optname, v->tv_sec * 1000 + v->tv_usec / 1000);
            }
        }
    }
    return setsockopt_f(sockfd, level, optname, optval, optlen);
}



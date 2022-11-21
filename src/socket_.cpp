/*
 * @Author: fepo_h
 * @Date: 2022-11-20 19:22:03
 * @LastEditors: fepo_h
 * @LastEditTime: 2022-11-21 15:58:18
 * @FilePath: /fepoh/workspace/fepoh_server/src/socket_.cpp
 * @Description: 
 * 
 * Copyright (c) 2022 by FepoH Fepo_H@163.com, All Rights Reserved. 
 * @version: V1.0.0
 * @Mailbox: Fepo_H@163.com
 * @Descripttion: 
 */
#include "socket_.h"
#include "hook.h"
#include "log/log.h"
#include "macro.h"

#include "fd_manager.h"

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/tcp.h>



namespace fepoh{

static Logger::ptr s_log_system = FEPOH_LOG_NAME("system");    

Socket::ptr Socket::CreateTCP(fepoh::Address::ptr address) {
    Socket::ptr sock(new Socket(address->getFamily(), TCP, 0));
    return sock;
}

Socket::ptr Socket::CreateUDP(fepoh::Address::ptr address) {
    Socket::ptr sock(new Socket(address->getFamily(), UDP, 0));
    sock->newSock();
    sock->m_isConnected = true;
    return sock;
}

Socket::ptr Socket::CreateTCPSocket() {
    Socket::ptr sock(new Socket(IPv4, TCP, 0));
    return sock;
}

Socket::ptr Socket::CreateUDPSocket() {
    Socket::ptr sock(new Socket(IPv4, UDP, 0));
    sock->newSock();
    sock->m_isConnected = true;
    return sock;
}

Socket::ptr Socket::CreateTCPSocket6() {
    Socket::ptr sock(new Socket(IPv6, TCP, 0));
    return sock;
}

Socket::ptr Socket::CreateUDPSocket6() {
    Socket::ptr sock(new Socket(IPv6, UDP, 0));
    sock->newSock();
    sock->m_isConnected = true;
    return sock;
}

Socket::ptr Socket::CreateUnixTCPSocket() {
    Socket::ptr sock(new Socket(UNIX, TCP, 0));
    return sock;
}

Socket::ptr Socket::CreateUnixUDPSocket() {
    Socket::ptr sock(new Socket(UNIX, UDP, 0));
    return sock;
}



Socket::Socket(int family ,int type , int protocol):m_sock(-1),m_family(family),
        m_type(type),m_protocol(protocol),m_isConnected(false){
}

Socket::~Socket(){
    close();
}

bool Socket::init(int sock){
    FdCtx::ptr fd_ctx = FdManager::GetInstance()->get(sock,true);
    if(fd_ctx && fd_ctx->isSocket() && !fd_ctx->isClose()){
        m_sock = sock;
        m_isConnected = true;
        initSock();
        getLocalAddress();
        getRemoteAddress();
        return true;
    }
    return false;
}

void Socket::initSock(){
    int val = 1;
    //重用端口
    setOption(SOL_SOCKET,SO_REUSEADDR,&val);
    if(m_type == SOCK_STREAM){
        //头文件netinet/tcp.h
        setOption(IPPROTO_TCP ,TCP_NODELAY ,val);
    }
}

void Socket::newSock(){
    m_sock = ::socket(m_family,m_type,m_protocol);
    if(FEPOH_LIKELY(m_sock != -1)){
        initSock();
    }else{
        FEPOH_LOG_ERROR(s_log_system) << "sock = " << m_sock <<",errno = " <<errno <<":"<<strerror(errno);
    }
}

uint64_t Socket::getSendTimeout(){
    FdCtx::ptr fd_ctx = FdManager::GetInstance()->get(m_sock);
    if(fd_ctx){
        return fd_ctx->getSendTimeout();
    }
    return -1;
}

void Socket::setSendTimeout(uint64_t value){
    FdCtx::ptr fd_ctx = FdManager::GetInstance()->get(m_sock,true);
    if(fd_ctx){
        fd_ctx->setSendTimeout(value);
    }
    struct timeval tv{int(value / 1000),int(value % 1000 * 1000)};
    setOption(SOL_SOCKET ,SO_SNDTIMEO ,tv);
}

uint64_t Socket::getRcvTimeout(){
    FdCtx::ptr fd_ctx = FdManager::GetInstance()->get(m_sock);
    if(fd_ctx){
        return fd_ctx->getRecvTimeout();
    }
    return -1;
}

void Socket::setRcvTimeout(uint64_t value){
    FdCtx::ptr fd_ctx = FdManager::GetInstance()->get(m_sock,true);
    if(fd_ctx){
        fd_ctx->setRecvTimeout(value);
    }
    struct timeval tv{int(value / 1000),int(value % 1000 * 1000)};
    setOption(SOL_SOCKET ,SO_RCVTIMEO ,tv);
}

bool Socket::getOption(int level,int option,void* res,socklen_t* len){
    int rt = getsockopt(m_sock,level,option,res,len);
    if(rt){
        FEPOH_LOG_ERROR(s_log_system) << "Socket::getOption error.sock = " 
                    << m_sock <<",option = " <<option;
        return false;
    }
    return true;
}

bool Socket::setOption(int level,int option,void* opVal,socklen_t* len){
    int rt = setsockopt(m_sock,level,option,opVal,*len);
    if(rt){
        FEPOH_LOG_ERROR(s_log_system) << "Socket::setOption error.sock = " 
                    << m_sock <<",option = " <<option;
        return false;
    }
    return true;
}

Socket::ptr Socket::accept(){
    Socket::ptr sock(new Socket(m_family,m_type,m_protocol));
    int rt = ::accept(m_sock,nullptr,nullptr);
    if(rt == -1){
        FEPOH_LOG_ERROR(s_log_system) << "Socket::accept error.errno = " 
                    << errno <<":" <<strerror(errno);
        return nullptr;
    }
    if(sock->init(rt)){
        return sock;
    }
    return nullptr;
}

bool Socket::bind(const Address::ptr addr){
    if(FEPOH_UNLIKELY(!isValid())){
        newSock();
        if(FEPOH_UNLIKELY(!isValid())){
            return false;
        }
    }
    if(FEPOH_UNLIKELY(addr->getFamily() != m_family)){
        FEPOH_LOG_ERROR(s_log_system) << "m_family = " << m_family << ",addr->family = " << addr->getFamily();
        return false;
    }
    int rt = ::bind(m_sock,addr->getAddr(),addr->getAddrLen());
    if(FEPOH_UNLIKELY(rt != 0)){
        FEPOH_LOG_ERROR(s_log_system) << "sock = " << m_sock 
                    << ",addr = " << addr->tostring();
        return false;
    }
    m_localAddr.reset();
    getLocalAddress();
    return true;
}

bool Socket::connect(const Address::ptr addr,uint64_t timeout){
    if(FEPOH_UNLIKELY(!isValid())){
        newSock();
        if(FEPOH_UNLIKELY(!isValid())){
            return false;
        }
    }
    if(FEPOH_UNLIKELY(addr->getFamily() != m_family)){
        FEPOH_LOG_ERROR(s_log_system) << "m_family = " << m_family << ",addr->family = " << addr->getFamily();
    }
    int rt = 0;
    if(timeout == (uint64_t)-1){
        rt = ::connect(m_sock ,addr->getAddr(),addr->getAddrLen());
    }else{
        rt = ::connect_with_timeout(m_sock,addr->getAddr(),addr->getAddrLen(),timeout);
    }
    if(FEPOH_UNLIKELY(rt != 0)){
        FEPOH_LOG_ERROR(s_log_system) << "sock = " << m_sock 
                    << ",addr = " << addr->tostring() <<",errno = " <<errno << strerror(errno);
        close();
        return false;
    }
    m_remoteAddr.reset();
    m_localAddr.reset();
    getLocalAddress();
    getRemoteAddress();
    m_isConnected = true;
    return true;
}

bool Socket::listen(int backlog){
    if(!isValid() && !m_localAddr){
        FEPOH_LOG_ERROR(s_log_system) << "m_sock = " << m_sock;
        return false;
    }
    int rt = ::listen(m_sock,backlog);
    if(FEPOH_UNLIKELY(rt != 0)){
        FEPOH_LOG_ERROR(s_log_system) << "sock = " << m_sock;
        return false;
    }
    return true;
}

bool Socket::close(){
    if(!m_isConnected && (m_sock ==-1)){
        return true;
    }
    m_isConnected = false;
    if(m_sock != -1){
        FdManager::GetInstance()->del(m_sock);
        ::close(m_sock);
        m_sock = -1;
    }
    return true;
}

ssize_t Socket::send(const void* buf,size_t len,int flags){
    if(isConnected()){
        return ::send(m_sock,buf,len,flags);
    }
    return -1;
}

ssize_t Socket::send(const iovec* buffers,size_t len,int flags){
    if(isConnected()){
        msghdr msg;
        bzero(&msg,sizeof(msg));
        msg.msg_iov = (iovec*)buffers;
        msg.msg_iovlen = len;
        return ::sendmsg(m_sock,&msg,flags);
    }
    return -1;
}

ssize_t Socket::sendto(const void* buf,size_t len,const Address::ptr toAddr,int flags){
    if((m_sock != -1)&& m_remoteAddr){
        return ::sendto(m_sock,buf,len,flags,toAddr->getAddr(),toAddr->getAddrLen());
    }
    return -1;
}

ssize_t Socket::sendto(struct iovec* buffers,size_t len,const Address::ptr toAddr,int flags){
    if((m_sock != -1)&& m_remoteAddr){
        struct msghdr msg;
        struct sockaddr addr = *(toAddr->getAddr());
        bzero(&msg,sizeof(msg));
        msg.msg_name = &addr;
        msg.msg_namelen = toAddr->getAddrLen();
        msg.msg_iov = buffers;
        msg.msg_iovlen = len;
        return ::sendmsg(m_sock,&msg,flags);
    }
    return -1;
}

ssize_t Socket::sendmsg(struct msghdr* msg,int flags){
    if((m_sock != -1)&& m_remoteAddr){
        return ::sendmsg(m_sock,msg,flags);
    }
    return -1;
}

ssize_t Socket::recv(void* buf , size_t len , int flag){
    if(isConnected()){
        return ::recv(m_sock,buf,len,flag);
    }
    return -1;
}

ssize_t Socket::recv(iovec* buffers,size_t len,int flags){
    if(isConnected()){
        struct msghdr msg;
        bzero(&msg,sizeof(msg));
        msg.msg_iov = buffers;
        msg.msg_iovlen = len;
        return ::recvmsg(m_sock,&msg,flags);
    }
    return -1;
}

ssize_t Socket::recvfrom(void* buf,size_t len,const Address::ptr fromAddr,int flags){
    if((m_sock != -1)&& m_remoteAddr){
        socklen_t len = fromAddr->getAddrLen();
        return ::recvfrom(m_sock,buf,len,flags,fromAddr->getAddr(),&len);
    }
    return -1;
}

ssize_t Socket::recvfrom(struct iovec* buffers,size_t len,const Address::ptr fromAddr,int flags){
    if((m_sock != -1)&& m_remoteAddr){
        struct msghdr msg;
        bzero(&msg,sizeof(msg));
        msg.msg_name = fromAddr->getAddr();
        msg.msg_namelen = fromAddr->getAddrLen();
        msg.msg_iov = buffers;
        msg.msg_iovlen = len;
        return ::recvmsg(m_sock,&msg,flags);
    }
    return -1;
}

ssize_t Socket::recvmsg(struct msghdr* msg,int flags){
    if((m_sock != -1)&& m_remoteAddr){
        return ::recvmsg(m_sock,msg,flags);
    }
    return -1;
}

Address::ptr  Socket::getRemoteAddress(){
    if(m_remoteAddr){
        return m_remoteAddr;
    }
    struct sockaddr addr;
    socklen_t len;
    switch (m_family){
        case AF_INET:
            len = sizeof(sockaddr_in);
            break;
        case AF_INET6:
            len = sizeof(sockaddr_in6);
            break;
        case AF_UNIX:
            len = offsetof(sockaddr_un,sun_path) + sizeof((sockaddr_un*)0)->sun_path -1;
            break;
        default:
            return nullptr;
            break;
    }
    int rt = getpeername(m_sock,&addr,&len);
    if(rt == -1){
        FEPOH_LOG_ERROR(s_log_system) <<"socket = " << m_sock <<",errno = " <<
                    errno <<":" << strerror(errno);
        return nullptr;
    }
    m_remoteAddr = Address::Create(&addr,len);
    return nullptr;
}

Address::ptr  Socket::getLocalAddress(){
    if(m_localAddr){
        return m_localAddr;
    }
    struct sockaddr addr;
    socklen_t len;
    switch (m_family){
        case AF_INET:
            len = sizeof(sockaddr_in);
            break;
        case AF_INET6:
            len = sizeof(sockaddr_in6);
            break;
        case AF_UNIX:
            len = offsetof(sockaddr_un,sun_path) + sizeof((sockaddr_un*)0)->sun_path -1;
            break;
        default:
            return nullptr;
            break;
    }
    int rt = getsockname(m_sock,&addr,&len);
    if(rt == -1){
        FEPOH_LOG_ERROR(s_log_system) <<"socket = " << m_sock <<",errno = " <<
                    errno <<":" << strerror(errno);
        return nullptr;
    }
    m_localAddr = Address::Create(&addr,len);
    return m_localAddr;
}

std::ostream& Socket::dump(std::ostream& os) const{
    os << "[Socket = " << m_sock
       << ",Family = " << m_family
       << ",Protocol = " << m_protocol
       << ",Type = " << m_type
       << ",Connected = " << m_isConnected;
    if(m_localAddr){
        os << ",Local address = " << m_localAddr->tostring();
    }
    if(m_remoteAddr){
        os << ",Remote address = " << m_remoteAddr->tostring();
    }
    os << "]";
    return os;
}

std::string Socket::tostring(){
    std::stringstream ss;
    dump(ss);
    return ss.str();
}

bool Socket::isValid() const{
    return -1 != m_sock;
}

//getsockopt 中的SO_ERROR,会获取并清除错误,所以不能用const
int  Socket::getError(){
    int error = 0;
    size_t len = sizeof(error);
    getOption(SOL_SOCKET,SO_ERROR,(void*)&error,(socklen_t*)&len);
    return error;
}

bool Socket::cancelRead(){
    return !IOManager::GetThis()->cancelEvent(m_sock,IOManager::READ);
}

bool Socket::cancelWrite(){
    return !IOManager::GetThis()->cancelEvent(m_sock,IOManager::WRITE);
}

bool Socket::cancelAccept(){
    return !IOManager::GetThis()->cancelEvent(m_sock,IOManager::READ);
}

bool Socket::cancelAll(){
    return !IOManager::GetThis()->cancelAll(m_sock);
}






} // namespace fepoh

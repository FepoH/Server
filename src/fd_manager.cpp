/*
 * @Author: fepo_h
 * @Date: 2022-11-20 17:56:15
 * @LastEditors: fepo_h
 * @LastEditTime: 2022-11-20 21:34:03
 * @FilePath: /fepoh/workspace/fepoh_server/src/fd_manager.cpp
 * @Description: 
 * 
 * Copyright (c) 2022 by FepoH Fepo_H@163.com, All Rights Reserved. 
 * @version: V1.0.0
 * @Mailbox: Fepo_H@163.com
 * @Descripttion: 
 */
#include "fd_manager.h"
#include "hook.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


namespace fepoh{
    
FdCtx::FdCtx(int fd)
        :m_isInit(false)
        ,m_isSocket(false)
        ,m_sysNonblock(false)
        ,m_userNonblock(false)
        ,m_isClose(false)
        ,m_fd(fd)
        ,m_recvTimeout(-1)
        ,m_sendTimeout(-1){
    init();
}

FdCtx::~FdCtx(){
}

bool FdCtx::init(){
    if(m_isInit){
        return true;
    }
    m_recvTimeout = -1;
    m_sendTimeout = -1;
    struct stat fd_stat;
    if(-1 == fstat(m_fd,&fd_stat)){
        m_isInit = false;
        m_isSocket = false;
    }else{
        m_isInit = true;
        m_isSocket = S_ISSOCK(fd_stat.st_mode);
    }
    if(m_isSocket){
        int flags = fcntl_f(m_fd, F_GETFL ,0);
        if(!(flags & O_NONBLOCK)) {
            fcntl_f(m_fd, F_SETFL , flags | O_NONBLOCK);
        }
        m_sysNonblock = true;
    }else{
        m_sysNonblock = false;
    }
    m_userNonblock = false;
    return m_isInit;
}

uint64_t FdCtx::getTimeout(int type){
    if(type == SO_RCVTIMEO){
        return m_recvTimeout;
    }else{
        return m_sendTimeout;
    }
}

void FdCtx::setTimeout(int type, uint64_t timeout){
    if(type == SO_RCVTIMEO){
        m_recvTimeout = timeout;
    }else{
        m_sendTimeout = timeout;
    }
}

FdManager::FdManager(){
    m_fds.resize(64);
}

FdCtx::ptr FdManager::get(int fd,bool auto_create){
    ReadLock lock(m_mutex);
    if(fd >= m_fds.size()){
        if(!auto_create){
            return nullptr;
        }
        lock.unlock();
        WriteLock lock1(m_mutex);
        m_fds.resize(fd*1.5);
    }else{
        if(m_fds[fd] || !auto_create){
            return m_fds[fd];
        }
        lock.unlock();
    }
    
    FdCtx:: ptr fd_ctx(new FdCtx(fd));
    WriteLock lock2(m_mutex);
    m_fds[fd] = fd_ctx;
    return fd_ctx;
}

void FdManager::del(int fd){
    WriteLock lock(m_mutex);
    m_fds[fd].reset();
}

FdManager::ptr FdManager::GetInstance(){
    static FdManager::ptr sing(new FdManager());
    return sing;
}

} // namespace fepoh

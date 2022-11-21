/*
 * @Author: fepo_h
 * @Date: 2022-11-17 18:14:57
 * @LastEditors: fepo_h
 * @LastEditTime: 2022-11-20 20:36:48
 * @FilePath: /fepoh/workspace/fepoh_server/src/socket_stream.cpp
 * @Description: 
 * 
 * Copyright (c) 2022 by FepoH Fepo_H@163.com, All Rights Reserved. 
 * @version: V1.0.0
 * @Mailbox: Fepo_H@163.com
 * @Descripttion: 
 */
#include "socket_stream.h"
#include "log/log.h"
#include <vector>
#include <sys/types.h>
#include <sys/socket.h>

namespace fepoh{

static Logger::ptr s_log_system = FEPOH_LOG_NAME("system");

SocketStream::SocketStream(Socket::ptr sock,bool owner)
    :m_socket(sock),m_owner(owner){
}

SocketStream::~SocketStream(){
    if(m_owner && m_socket){
        m_socket->close();
    }
}

int SocketStream::read(void* buffer,size_t length) {
    if(!isConnected()){
        return -1;
    }
    int rt = m_socket->recv(buffer,length);
    if(rt <= 0){
        FEPOH_LOG_ERROR(s_log_system) << "read error,errno = " << errno 
                << ":" <<strerror(errno);
        return -1;
    }
    return rt;
}

int SocketStream::read(ByteArray::ptr ba,size_t length) {
    if(!isConnected()){
        return -1;
    }
    std::vector<iovec> iovs;
    ba->getWriteBuffers(iovs,length);
    int rt = m_socket->recv(&iovs[0],length);
    if(rt > 0){
        ba->setSize(ba->getSize() + rt);
    }
    return rt;
}

int SocketStream::write(const void* buffer,size_t length) {
    if(!isConnected()){
        return -1;
    }
    int rt = m_socket->send(buffer,length);
    if(rt <= 0){
        FEPOH_LOG_ERROR(s_log_system) << "read error,errno = " << errno 
                << ":" <<strerror(errno);
        return -1;
    }
    return rt;
}

int SocketStream::write(ByteArray::ptr ba,size_t length) {
    if(!isConnected()){
        return -1;
    }
    std::vector<iovec> iovs;
    ba->getReadBuffers(iovs,length);
    int rt = m_socket->send(&iovs[0],length);
    if(rt > 0){
        //改变已读数据大小
        ba->setReadPos(ba->getReadedSize() + rt);
    }
    return rt;
}

bool SocketStream::isConnected(){
    return m_socket && m_socket->isConnected();
}

void SocketStream::close(){
    if(m_socket){
        m_socket->close();
    }
}



}//namespace
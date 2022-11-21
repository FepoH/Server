/*
 * @Author: fepo_h
 * @Date: 2022-11-18 19:58:24
 * @LastEditors: fepo_h
 * @LastEditTime: 2022-11-20 20:38:00
 * @FilePath: /fepoh/workspace/fepoh_server/src/socket_stream.h
 * @Description: 封装socket读写相关操作
 * 
 * Copyright (c) 2022 by FepoH Fepo_H@163.com, All Rights Reserved. 
 * @version: V1.0.0
 * @Mailbox: Fepo_H@163.com
 * @Descripttion: 
 */
#pragma once

#include "stream.h"
#include "socket_.h"

namespace fepoh{

/**
 * @description: socket 流操作
 */
class SocketStream : public Stream{
public:
    typedef std::shared_ptr<SocketStream> ptr;

    SocketStream(Socket::ptr sock,bool owner = true);
    ~SocketStream();

    Socket::ptr getSocket(){return m_socket;}
    //读操作
    int read(void* buffer,size_t length) override;
    int read(ByteArray::ptr ba,size_t length) override;
    //写操作
    int write(const void* buffer,size_t length) override;
    int write(ByteArray::ptr ba,size_t length) override;
    //是否连接
    bool isConnected();
    //关闭socket
    void close() override;
private:
    Socket::ptr m_socket;
    bool m_owner;
};


}//namespace
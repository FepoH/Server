/*
 * @Author: fepo_h
 * @Date: 2022-11-20 19:11:07
 * @LastEditors: fepo_h
 * @LastEditTime: 2022-11-20 21:34:54
 * @FilePath: /fepoh/workspace/fepoh_server/src/socket_.h
 * @Description: 
 * 
 * Copyright (c) 2022 by FepoH Fepo_H@163.com, All Rights Reserved. 
 * @version: V1.0.0
 * @Mailbox: Fepo_H@163.com
 * @Descripttion: 
 */
#pragma once

#include <memory>
#include <sys/types.h>
#include <sys/socket.h>
#include "noncopyable.h"
#include "address.h"

namespace fepoh{
    
class Socket : Noncopyable{
public:
    typedef std::shared_ptr<Socket> ptr;
    typedef std::weak_ptr<Socket> weak_ptr;
    /**
     * @description: tcp或udp
     */    
    enum Type{
        TCP = SOCK_STREAM,
        UDP = SOCK_DGRAM
    };
    /**
     * @description: 协议族
     */    
    enum Family{
        IPv4 = AF_INET,
        IPv6 = AF_INET6,
        UNIX = AF_UNIX
    };
    //创建TCP Socket(满足地址类型)
    static Socket::ptr CreateTCP(fepoh::Address::ptr address);
    //创建UDP Socket(满足地址类型)
    static Socket::ptr CreateUDP(fepoh::Address::ptr address);
    //创建IPv4的TCP Socket
    static Socket::ptr CreateTCPSocket();
    //创建IPv4的UDP Socket
    static Socket::ptr CreateUDPSocket();
    //创建IPv6的TCP Socket
    static Socket::ptr CreateTCPSocket6();
    //创建IPv6的UDP Socket
    static Socket::ptr CreateUDPSocket6();
    //创建Unix的TCP Socket
    static Socket::ptr CreateUnixTCPSocket();
    //创建Unix的UDP Socket
    static Socket::ptr CreateUnixUDPSocket();

    Socket(int family ,int type , int protocol = 0);
    ~Socket();

    //获取和设置超时时间
    uint64_t getSendTimeout();
    void setSendTimeout(uint64_t val);
    uint64_t getRcvTimeout();
    void setRcvTimeout(uint64_t val);

    //getsockopt和setsockopt
    bool getOption(int level,int option,void* res,socklen_t* len);
    template<class T>
    bool getOption(int level,int option,const T& res){
        socklen_t len = sizeof(T);
        return getOption(level,option,(void*)&res,&len);
    }
    bool setOption(int level,int option,void* opVal,socklen_t* len);
    template<class T>
    bool setOption(int level,int option,const T& opVal){
        socklen_t len = sizeof(T);
        return setOption(level,option,(void*)&opVal,&len);
    }

    //网络相关操作
    Socket::ptr accept();
    bool bind(const Address::ptr addr);
    bool connect(const Address::ptr addr,uint64_t timeout = -1);
    bool listen(int backlog = SOMAXCONN);
    bool close();

    //发送和接受数据
    ssize_t send(const void* buf,size_t len,int flags = 0);
    ssize_t send(const iovec* buffers,size_t len,int flags = 0);
    ssize_t sendto(const void* buf,size_t len,const Address::ptr toAddr,int flags = 0);
    ssize_t sendto(struct iovec* buffers,size_t len,const Address::ptr toAddr,int flags = 0);
    ssize_t sendmsg(struct msghdr* msg,int flags = 0);

    ssize_t recv(void* buf , size_t len , int flag = 0);
    ssize_t recv(iovec* buffers,size_t len,int flags = 0);
    ssize_t recvfrom(void* buf,size_t len,const Address::ptr fromAddr,int flags = 0);
    ssize_t recvfrom(struct iovec* buffers,size_t len,const Address::ptr fromAddr,int flags = 0);
    ssize_t recvmsg(struct msghdr* msg,int flags);

    //获取本地地址和远端地址
    Address::ptr getRemoteAddress();
    Address::ptr getLocalAddress();

    //dump
    std::ostream& dump(std::ostream& os) const;
    std::string tostring();

    //IO事件相关
    bool cancelRead();
    bool cancelWrite();
    bool cancelAccept();
    bool cancelAll();

    //属性相关
    int  getSocket() const {return m_sock;}
    int  getFamily() const {return m_family;}
    int  getType() const {return m_type;}
    int  getProtocol() const {return m_protocol;}
    bool isConnected() const {return m_isConnected;}
public:
    //初始化
    bool init(int sock);
    bool isValid() const;

    //获取错误,通过getsockopt
    int  getError();
private:
    //初始化
    void initSock();
    //新创一个socket
    void newSock();
private:
    int m_sock = -1;                //文件描述符
    int m_family;                   //协议族
    int m_protocol;                 //具体协议
    int m_type;                     //type
    bool m_isConnected = false;     //是否已经连接
    Address::ptr m_localAddr;       //本地地址
    Address::ptr m_remoteAddr;      //远端地址
};



} // namespace fepoh

/*
 * @Author: fepo_h
 * @Date: 2022-11-21 14:00:51
 * @LastEditors: fepo_h
 * @LastEditTime: 2022-11-22 19:04:39
 * @FilePath: /fepoh/workspace/fepoh_server/src/tcp_server.cpp
 * @Description: 
 * 
 * Copyright (c) 2022 by FepoH Fepo_H@163.com, All Rights Reserved. 
 * @version: V1.0.0
 * @Mailbox: Fepo_H@163.com
 * @Descripttion: 
 */
#include "tcp_server.h"
#include "config.h"
#include "log/log.h"
#include "macro.h"


namespace fepoh{

fepoh::ConfigVar<uint64_t>::ptr g_tcp_server_read_timeout = 
        fepoh::Config::Lookup<uint64_t>(2 * 60 * 1000 , "tcp_server.read_timeout"
                                            ,"tcp server read time out");

static Logger::ptr s_log_system = FEPOH_LOG_NAME("system");

TcpServer::TcpServer(fepoh::IOManager* worker,fepoh::IOManager* accept_worker)
    :m_worker(worker)
    ,m_acceptWorker(accept_worker)
    ,m_name("fepoh/1.0.0")
    ,m_readTimeout(g_tcp_server_read_timeout->getValue())
    ,m_isStop(false){
    
}

TcpServer::~TcpServer() {
    for(auto& i : m_socks){
        i->close();
    }
    m_socks.clear();
}

bool TcpServer::bind(fepoh::Address::ptr addr){
    std::vector<Address::ptr> addrs;
    std::vector<Address::ptr> failAddrs;
    addrs.push_back(addr);
    return bind(addrs,failAddrs);
}


//绑定并监听
bool TcpServer::bind(const std::vector<Address::ptr>& addrs,std::vector<Address::ptr>& failAddrs){
    for(auto& addr : addrs){
        Socket::ptr sock = Socket::CreateTCP(addr);
        if(!sock->bind(addr)){
            FEPOH_LOG_ERROR(s_log_system) << "bind fail errno = " << errno
                    << ":" << strerror(errno) << " addr = [" << addr->tostring()
                    << "]";
            failAddrs.push_back(addr);
            continue;
        }
        if(!sock->listen()){
            FEPOH_LOG_ERROR(s_log_system) << "listen fail errno = " << errno
                    << ":" << strerror(errno) << " addr = [" << addr->tostring()
                    << "]";
            failAddrs.push_back(addr);
            continue;
        }
        m_socks.push_back(sock);
    }
    if(!failAddrs.empty()){
        m_socks.clear();
        return false;
    }
    for(auto& sock : m_socks){
        FEPOH_LOG_INFO(s_log_system) << "server bind success:" << sock->tostring();
    }
    return true;
}

bool TcpServer::start(){
    if(m_isStop){
        return true;
    }
    m_isStop = false;
    for(auto& sock : m_socks){
        m_acceptWorker->schedule(std::bind(&TcpServer::startAccept
                ,shared_from_this(),sock));
    }
    return true;
}

void TcpServer::stop(){
    FEPOH_ASSERT(false);
    m_isStop = true;
    auto self = shared_from_this();
    m_acceptWorker->schedule([this,self](){
        for(auto& sock : m_socks){
            //删除事件
            sock->cancelAll();
            sock->close();
        }
        m_socks.clear();
    });
}

void TcpServer::handleClient(Socket::ptr client){
    FEPOH_LOG_INFO(s_log_system) << "handle client: " << client->tostring();
}

void TcpServer::startAccept(Socket::ptr sock){
    std::cout << "sock = " << sock->getSocket() <<std::endl;
    while(!m_isStop){
        //注册读事件
        Socket::ptr client = sock->accept();
        if(client){
            m_worker->schedule(std::bind(&TcpServer::handleClient
                                ,shared_from_this(),client));
        }else{
            FEPOH_LOG_ERROR(s_log_system) << "accept errno = " << errno
                    << ":" << strerror(errno);
        }
    }
    FEPOH_ASSERT(false);
}




}//namespace
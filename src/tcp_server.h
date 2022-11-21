/*
 * @Author: fepo_h
 * @Date: 2022-11-21 13:35:20
 * @LastEditors: fepo_h
 * @LastEditTime: 2022-11-21 13:45:33
 * @FilePath: /fepoh/workspace/fepoh_server/src/tcp_server.h
 * @Description: 
 * 
 * Copyright (c) 2022 by FepoH Fepo_H@163.com, All Rights Reserved. 
 * @version: V1.0.0
 * @Mailbox: Fepo_H@163.com
 * @Descripttion: 
 */
#pragma once

#include "io_manager.h"
#include "socket_.h"
#include "address.h"
#include "noncopyable.h"
#include <memory>
#include <string>
#include <functional>
#include <vector>

namespace fepoh{

/**
 * @description: TCP服务封装
 * @return {*}
 */
class TcpServer : public std::enable_shared_from_this<TcpServer>
                , public Noncopyable{
public:
    typedef std::shared_ptr<TcpServer> ptr;
    /**
     * @description: 构造函数
     * @return {*}
     */    
    TcpServer(fepoh::IOManager* worker = fepoh::IOManager::GetThis(),
                fepoh::IOManager* accept_worker = fepoh::IOManager::GetThis());
    virtual ~TcpServer();
    /**
     * @description: 绑定并监听地址多个地址
     * @return {*}
     * @param {ptr} addr
     */    
    virtual bool bind(fepoh::Address::ptr addr);
    /**
     * @description: 绑定地址多个地址
     * @return {*}
     */    
    virtual bool bind(const std::vector<Address::ptr>& addrs,std::vector<Address::ptr>& failAddrs);
    /**
     * @description: 开启服务
     * @return {*}
     */    
    virtual bool start();
    /**
     * @description: 停止服务
     * @return {*}
     */    
    virtual void stop();
    /**
     * @description: 获取读超时时间
     * @return {*}
     */    
    uint64_t getReadTimeout() const {return m_readTimeout;}
    /**
     * @description: 获取名称
     * @return {*}
     */    
    std::string getName() const {return m_name;}
    /**
     * @description: 设置名称
     * @return {*}
     * @param {string&} v
     */    
    void setName(const std::string& v) {m_name = v;}
    /**
     * @description: 设置读超时时间
     * @return {*}
     * @param {uint64_t} v
     */    
    void setReadTimeout(uint64_t v) {m_readTimeout = v;}
    //是否停止
    bool isStop() const {return m_isStop;}
protected:
    /**
     * @description: 处理客户端
     * @return {*}
     * @param {ptr} client
     */
    virtual void handleClient(Socket::ptr client);
    /**
     * @description: 开始接受客户端
     * @return {*}
     * @param {ptr} sock
     */    
    virtual void startAccept(Socket::ptr sock);

private:
    //同时监听多地址
    std::vector<Socket::ptr> m_socks;
    IOManager* m_worker;            //处理数据等
    IOManager* m_acceptWorker;      //处理连接等
    uint64_t m_readTimeout;         //处理非活跃连接和非法连接等
    std::string m_name;             //方便调试和日志输出
    bool m_isStop;
};




}//namesapce
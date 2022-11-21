/*
 * @Author: fepo_h
 * @Date: 2022-11-17 22:11:38
 * @LastEditors: fepo_h
 * @LastEditTime: 2022-11-21 14:31:45
 * @FilePath: /fepoh/workspace/fepoh_server/src/http/http_server.h
 * @Description: http服务
 * 
 * Copyright (c) 2022 by FepoH Fepo_H@163.com, All Rights Reserved. 
 * @version: V1.0.0
 * @Mailbox: Fepo_H@163.com
 * @Descripttion: 
 */
#pragma once

#include "http/http_session.h"
#include "tcp_server.h"
#include "http.h"
#include "servlet.h"

namespace fepoh{
namespace http{
class HttpServer : public TcpServer{
public:
    typedef std::shared_ptr<HttpServer> ptr;
    /**
     * @description: 构造函数
     * @return {*}
     * @param {bool} keepalive 是否为长连接
     */    
    HttpServer(bool keepalive = false, fepoh::IOManager* worker = IOManager::GetThis(),
                IOManager* accept_worker = fepoh::IOManager::GetThis());
    
    ServletManager::ptr getDispath() {return m_dispath;}
    void setServletManager(ServletManager::ptr v) {m_dispath = v;}
protected:
    virtual void handleClient(Socket::ptr client) override;

private:
    bool m_isKeepalive = false;
    ServletManager::ptr m_dispath;
};

}
}//namespace
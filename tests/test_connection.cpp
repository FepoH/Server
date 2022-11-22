/*
 * @Author: fepo_h
 * @Date: 2022-11-19 02:15:08
 * @LastEditors: fepo_h
 * @LastEditTime: 2022-11-22 00:27:28
 * @FilePath: /fepoh/workspace/fepoh_server/tests/test_connection.cpp
 * @Description: 
 * 
 * Copyright (c) 2022 by FepoH Fepo_H@163.com, All Rights Reserved. 
 * @version: V1.0.0
 * @Mailbox: Fepo_H@163.com
 * @Descripttion: 
 */
#include "http/http_connection.h"
#include "log/log.h"
#include "io_manager.h"
#include "address.h"
#include "config.h"
#include <atomic>

using namespace fepoh;
using namespace fepoh::http;

static Logger::ptr s_log_system = FEPOH_LOG_NAME("system");

std::atomic<int> count = {0};

Timer::ptr timer;

void test_poll(){
    HttpConnectionPool::ptr pool(new HttpConnectionPool("www.sylar.top"
                    ,"",80,50,1000 * 30 , 500));
    timer = fepoh::IOManager::GetThis()->addTimer(10,[pool,timer](){
        HttpResult::ptr res = pool->doGet("/",1000);
        ++count;
        if(count >= 500){
            
            timer->cancel();
            
        }
    },true);
}


void run(){
    Address::ptr addr = Address::LookupIPAddr("www.sylar.top:80",0);
    if(!addr){
        FEPOH_LOG_ERROR(s_log_system) << "addr fail";
        return ;
    }
    Socket::ptr sock = Socket::CreateTCP(addr);
    bool rt = sock->connect(addr);
    if(!rt){
        FEPOH_LOG_ERROR(s_log_system) << "connection fail";
        sock->close();
        return ;
    }
    HttpConnection::ptr conn(new HttpConnection(sock));
    HttpRequest::ptr req(new HttpRequest());
    req->setPath("/blog/");
    req->setHeader("Host","www.sylar.top");
    conn->sendRequest(req);
    auto rsp = conn->recvResponse();
    if(!rsp){
        FEPOH_LOG_ERROR(s_log_system) << "recv error";
    }
    HttpResult::ptr res = HttpConnection::DoGet("http://www.baidu.com:80",300);
    if(!res->response){
        FEPOH_LOG_DEBUG(s_log_system) << res->result;
        return ;
    }
    FEPOH_LOG_DEBUG(s_log_system) << res->response->tostring();
    
}

void test1(){
    IOManager iom("",3);
    iom.schedule(test_poll);
    iom.start();
}

int main(){
    Config::LoadFromJson("/home/fepoh/workspace/fepoh_server/resource/config/log.json");
    test1();

    FEPOH_LOG_DEBUG(s_log_system) << HttpConnection::s_count;
}
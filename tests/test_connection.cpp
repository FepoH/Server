/*
 * @Author: fepo_h
 * @Date: 2022-11-19 02:15:08
 * @LastEditors: fepo_h
 * @LastEditTime: 2022-11-21 17:26:45
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

using namespace fepoh;
using namespace fepoh::http;

static Logger::ptr s_log_system = FEPOH_LOG_NAME("system");

int count = 0;

Timer::ptr timer;
HttpConnectionPool::ptr pool;
void test_poll(){
    pool.reset(new HttpConnectionPool("www.sylar.top"
                    ,"",80,100,1000 * 10 , 5));
    timer = fepoh::IOManager::GetThis()->addTimer(10,[pool,timer](){
        HttpResult::ptr res = pool->doGet("/",1000);
        //FEPOH_LOG_DEBUG(s_log_system) << "-------" << res->tostring() << "----------------";
        ++count;
        if(count == 500){
            timer->cancel();
        }
    },true);
    FEPOH_LOG_DEBUG(s_log_system) << pool->getTotalSize();
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
    std::cout << req->tostring() << std::endl << "-------------" << rsp->tostring() <<std::endl;
    std::cout <<  "-------------" <<std::endl;
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
    test1();
    FEPOH_LOG_DEBUG(s_log_system) << pool->getTotalSize() 
            << " "  << pool->m_conns.size() << "  " << HttpConnection::s_count;
    
}
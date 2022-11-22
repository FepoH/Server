/*
 * @Author: fepo_h
 * @Date: 2022-11-21 14:53:11
 * @LastEditors: fepo_h
 * @LastEditTime: 2022-11-22 19:56:47
 * @FilePath: /fepoh/workspace/fepoh_server/tests/test_http_server.cpp
 * @Description: 
 * 
 * Copyright (c) 2022 by FepoH Fepo_H@163.com, All Rights Reserved. 
 * @version: V1.0.0
 * @Mailbox: Fepo_H@163.com
 * @Descripttion: 
 */
#include "http/http_server.h"
#include "io_manager.h"
#include "log/log.h"
#include "config.h"

using namespace fepoh;
using namespace fepoh::http;

static Logger::ptr s_log_system = FEPOH_LOG_NAME("system");

void run(){
    fepoh::http::HttpServer::ptr serv(new http::HttpServer(true));
    Address::ptr addr = Address::LookupIPAddr("0.0.0.0:8020",0);
    while(!serv->bind(addr)){
        sleep(2);
    }
    auto dp = serv->getDispath();
    dp->addServlet("/zls/xx",[](HttpRequest::ptr request,HttpResponse::ptr response
                        ,HttpSession::ptr session){
        response->setBody(request->tostring()+ "asdfasdfasdfasdfsadf");
        return 0;
    });
    dp->addGlobServlet("/zls/*",[](HttpRequest::ptr request,HttpResponse::ptr response
            ,HttpSession::ptr session){
        response->setBody(request->tostring());
        return 0;
    });
    serv->start();
}

int main(){
    Config::LoadFromJson("/home/fepoh/workspace/fepoh_server/resource/config/log.json");
    
    IOManager iom("",1);
    iom.schedule(run);
    iom.start();
}

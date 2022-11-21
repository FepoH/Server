/*
 * @Author: fepo_h
 * @Date: 2022-11-20 20:39:54
 * @LastEditors: fepo_h
 * @LastEditTime: 2022-11-20 20:57:17
 * @FilePath: /fepoh/workspace/fepoh_server/tests/test_sockstream.cpp
 * @Description: 测试socket stream
 * 
 * Copyright (c) 2022 by FepoH Fepo_H@163.com, All Rights Reserved. 
 * @version: V1.0.0
 * @Mailbox: Fepo_H@163.com
 * @Descripttion: 
 */
#include "socket_stream.h"
#include "log/log.h"

using namespace fepoh;

static Logger::ptr s_log_system = FEPOH_LOG_NAME("system");

void test(){
    Address::ptr addr = Address::LookupIPAddr("192.168.159.151",8020);
    Socket::ptr sock = Socket::CreateTCP(addr);
    if(!sock->bind(addr)){
        FEPOH_LOG_ERROR(s_log_system) << "bind error";
    }
    sock->listen();
    Socket::ptr clie = sock->accept();
    FEPOH_LOG_DEBUG(s_log_system) << clie->tostring();
    if(clie){
        SocketStream::ptr ss(new SocketStream(clie));
        char buf[1000];
        int i = 0;
        while(1){
            bzero(buf,1000);
            sprintf(buf,"%s = %d","server send = ",i);
            int rt = ss->write(buf,strlen(buf));
            if(rt <= 0){
                FEPOH_LOG_ERROR(s_log_system) << "write error";
            }
            bzero(buf,1000);
            rt = ss->read(buf,1000);
            if(rt <= 0){
                FEPOH_LOG_ERROR(s_log_system) << "recv error";
            }else{
                FEPOH_LOG_DEBUG(s_log_system) << buf;
            }
            sleep(3);
            ++i;
        }
    }
}


int main(){
    test();
}
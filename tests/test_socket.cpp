/*
 * @Author: fepo_h
 * @Date: 2022-11-20 20:12:16
 * @LastEditors: fepo_h
 * @LastEditTime: 2022-11-20 20:29:58
 * @FilePath: /fepoh/workspace/fepoh_server/tests/test_socket.cpp
 * @Description: 
 * 
 * Copyright (c) 2022 by FepoH Fepo_H@163.com, All Rights Reserved. 
 * @version: V1.0.0
 * @Mailbox: Fepo_H@163.com
 * @Descripttion: 
 */
#include "socket_.h"
#include "log/log.h"

using namespace fepoh;

static Logger::ptr s_log_system = FEPOH_LOG_NAME("system");

void test(){
    Address::ptr addr = IPAddress::Lookup("www.baidu.com:http",0);
    if(!addr){
        FEPOH_LOG_ERROR(s_log_system) << "Look up error";
    }else{
        FEPOH_LOG_INFO(s_log_system) << addr->tostring();
    }

    Socket::ptr sock = Socket::CreateTCP(addr);
    sock->connect(addr);
    char buf[20000] = "GET / HTTP/1.0\r\n\r\n";
    sock->send(buf,strlen(buf));
    bzero(buf,20000);
    int n = sock->recv(buf,20000);
    if(n <= 0){
        FEPOH_LOG_ERROR(s_log_system) << "recv error";
    }else{
        FEPOH_LOG_INFO(s_log_system) << sock->tostring();
        FEPOH_LOG_INFO(s_log_system) << buf;
    }
}

void test_server(int port){
    Socket::ptr sock = Socket::CreateTCPSocket();
    Address::ptr addr = IPv4Address::Create("192.168.159.151",port);
    sock->bind(addr);
    sock->listen(1024);
    Socket::ptr cli = sock->accept();
    int serv = 0;
    char buf[1024];
    while(1){
        bzero(buf,1024);
        sprintf(buf,"%s = %d","serv send",serv);
        int n = cli->send(buf,strlen(buf));
        bzero(buf,1024);
        n = cli->recv(buf,1024);
        if(n <= 0){
            FEPOH_LOG_ERROR(s_log_system) << "recv error n = " << n <<": " <<strerror(errno);
        }else{
            //FEPOH_LOG_INFO(s_log_system) << sock->tostring();
            FEPOH_LOG_INFO(s_log_system) << buf;
        }
        sleep(3);
        ++serv;
    }
}

int main(int argc,char* argv[]){
    if(argc != 2){
        return -1;
    }
    int port = atoi(argv[1]);
    //test();
    test_server(port);
}